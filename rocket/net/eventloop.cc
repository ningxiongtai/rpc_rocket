#include "rocket/common/log.h"
#include "rocket/net/eventloop.h"
#include <sys/socket.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <string.h>
#include "rocket/common/util.h"
#include "rocket/common/mutex.h"

#define ADD_TO_EPOLL() \
            auto it = m_listen_fds.find(event->getFd()); \
            int op = EPOLL_CTL_ADD;\
            if(it != m_listen_fds.end()) {\
                op = EPOLL_CTL_ADD;\
            }\
            epoll_event tmp = event->getEpollEvent();\
            int rt = epoll_ctl(m_epoll_fd, op, event->getFd(), &tmp); \
            if(rt == -1) {\
                ERRORLOG("failed epoll_ctl when add fd %d, errno = %d", errno, strerror(errno) );\
            }\
            DEBUGLOG("add event success ,fd [%d]", event->getFd());\

#define DELETE_TO_EPOLL() \
            auto it = m_listen_fds.find(event->getFd()); \
            if(it == m_listen_fds.end()) {\
                return;\
            }\
            int op = EPOLL_CTL_DEL;\
            epoll_event tmp = event->getEpollEvent();\
            int rt = epoll_ctl(m_epoll_fd, op, event->getFd(), &tmp); \
            if(rt == -1) {\
                ERRORLOG("failed epoll_ctl when add fd %d, errno = %d", errno, strerror(errno) );\
            }\
            DEBUGLOG("delete event success ,fd [%d]", event->getFd());\

namespace rocket {

static thread_local Eventloop* t_current_eventloop = NULL;
static int g_epoll_max_time_out =  10000;
static int g_epoll_max_events = 10;
    Eventloop::Eventloop() {
        if(t_current_eventloop != NULL) {
            ERRORLOG("failed to create event loop, this thread has created eventloop");
            exit(0);
        }
        m_thread_id = getThreadId();

        m_epoll_fd = epoll_create(10);
        if(m_epoll_fd == -1) {
            ERRORLOG("fail to create event loop, epoll create error, error info[%d]", errno);
            exit(0);
        }

        initWakeUpFdEvent();
        initTimer();
        
        INFOLOG("succ create event loop in thread %d", m_thread_id);
        t_current_eventloop = this;
    }

    Eventloop::~Eventloop() {
        close(m_epoll_fd);
        if(m_wake_up_fd_event) {
            delete m_wake_up_fd_event;
            m_wake_up_fd_event = NULL;
        }
        if(m_timer) {
            delete m_timer;
            m_timer = NULL;
        }
    }

    void Eventloop::initTimer() {
        m_timer = new Timer();
        addEpollEvent(m_timer);
    }

    void Eventloop::addTimerEvent(TimerEvent::s_ptr event){
        m_timer->addTimerEvent(event);
    }


    void Eventloop::initWakeUpFdEvent() {
        m_wakeup_fd = eventfd(0, EFD_NONBLOCK);
        if(m_wakeup_fd <= 0) {
            ERRORLOG("failed to create event loop, eventfd create error, error info[%d]", errno);
            exit(0);
        }

        m_wake_up_fd_event = new WakeUpFdEvent(m_wakeup_fd);
     
        m_wake_up_fd_event->listen(Fdevent::IN_EVENT, [this]() {
        char buf[8];
        while (read(m_wakeup_fd, buf, 8) == -1 && errno != EAGAIN)
        {
        }
        DEBUGLOG("read full bytes from wakeup fd[%d]", m_wakeup_fd);
    });
        addEpollEvent(m_wake_up_fd_event);
        
    }

    void Eventloop::loop() {
        while (!m_stop_flag)
        {   
            ScopeMutext<Mutex> lock(m_mutex);
            std::queue<std::function<void()>> tmp_tasks ;

            m_pending_tasks.swap(tmp_tasks);
            lock.unlock();

            while(!tmp_tasks.empty()) {
                std::function<void()> cb =  tmp_tasks.front();
                tmp_tasks.pop();
                if(cb) {
                    cb();
                }

            }

            int timeout = g_epoll_max_time_out;
            epoll_event result_events[g_epoll_max_events];
            //DEBUGLOG("now begin to epoll_wait");
            int rt = epoll_wait(m_epoll_fd, result_events, g_epoll_max_events, timeout);
            DEBUGLOG("now end to epoll_wait ,rt = %d", rt);

            if(rt < 0) {
                ERRORLOG("epoll_wait error, error = ", errno);
            } else {
                for(int i = 0; i < rt; ++i) {
                    epoll_event trigger_event = result_events[i];
                    Fdevent*  fd_event = static_cast<Fdevent*> (trigger_event.data.ptr);
                    if(fd_event == NULL) {
                        ERRORLOG("fd_event = NULL, continue");
                        continue;
                    }
                    if(trigger_event.events  & EPOLLIN) { //读事件
                        DEBUGLOG("fd %d trigger EPOLLIN event", fd_event->getFd())
                        addTask(fd_event->handler(Fdevent::IN_EVENT));
                    }
                    if(trigger_event.events  & EPOLLOUT) { //写事件
       
                        DEBUGLOG("fd %d trigger EPOLLOUT event", fd_event->getFd())
                        addTask(fd_event->handler(Fdevent::OUT_EVENT));
                    }
                }
            }

        } 
    }

    void Eventloop::wakeup() {
        INFOLOG("WAKE UP");
        m_wake_up_fd_event->wakeup();
    }
    
    void Eventloop::stop() {
        m_stop_flag = true;
    }

    void Eventloop::dealWakeup() {

    }

    void Eventloop::addEpollEvent(Fdevent* event) {
        if(isInLoopThread()) {
            ADD_TO_EPOLL();
            }else {
                auto cb = [this, event]() {
                    ADD_TO_EPOLL();
                };
                addTask(cb, true);
            }
        }
    

    void Eventloop::deleteEpollEvent(Fdevent* event) {
        if(isInLoopThread()) {
            DELETE_TO_EPOLL();
        } else {


            auto cb = [this, event] () {
                DELETE_TO_EPOLL();
            };
        addTask(cb, true);
        }
    }
    
    void Eventloop::addTask(std::function<void()> cb, bool is_wake_up  /*false*/) {
        ScopeMutext<Mutex> lock(m_mutex);
        m_pending_tasks.push(cb);
        lock.unlock();


        if(is_wake_up) {
            wakeup();
        }
    }
    bool Eventloop::isInLoopThread() {
        return getThreadId() == m_thread_id;
    }

    Eventloop* Eventloop::GetCurrentEventLoop() {
    if (t_current_eventloop) {
        return t_current_eventloop;
    }
    t_current_eventloop = new Eventloop();
    return t_current_eventloop;
    }

}
