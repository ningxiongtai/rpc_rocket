#ifndef ROCKET_NET_EVENTLOOP
#define ROCKET_NET_EVENTLOOP

#include <set>
#include <pthread.h>
#include <functional>
#include <queue>
#include "rocket/net/fd_event.h"
#include "rocket/net/wakeup_fd_event.h"
namespace rocket {

class Eventloop

{
public:
    Eventloop();

    ~Eventloop();

    void loop();

    void wakeup();

    void stop();
    void addEpollEvent(Fdevent* event);
    void deleteEpollEvent(Fdevent* event);
    //判断当前是否还有线程
    bool isInLoopThread() ;

    void addTask(std::function<void()> cd, bool is_wake_up = false) ;
private:
    void dealWakeup();
    void initWakeUpFdEvent();
private:
  pid_t m_thread_id;

  int m_epoll_fd {0};

  int m_wakeup_fd {0};

  WakeUpFdEvent* m_wake_up_fd_event {NULL};

  bool m_stop_flag {false};

  std::set<int> m_listen_fds;

  //所有待执行任务队列
  std::queue<std::function<void()>> m_pending_tasks;
  
  Mutex m_mutex;

};



}








#endif