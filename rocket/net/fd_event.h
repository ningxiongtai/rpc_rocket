#ifndef ROCKET_NET_FDEVENT_H
#define ROCKET_NET_FDEVENT_H

#include <functional>
#include <sys/epoll.h>

namespace rocket{

class Fdevent
{
public:

    enum FdTriggerEvent {
        IN_EVENT = EPOLLIN,
        OUT_EVENT = EPOLLOUT,
        ERROR_EVENT = EPOLLERR
    };

    Fdevent(int fd);
    Fdevent();
    ~Fdevent();

    void setNonBlock();

    std::function<void()> handler(FdTriggerEvent event_type);

    void listen(FdTriggerEvent event_type, std::function<void()> callback, std::function<void()> error_callback = nullptr );
    
    void cancle(FdTriggerEvent event_type);
    int getFd() {
        return m_fd;
    }
    
    epoll_event getEpollEvent() {
        return m_listen_events;
    }
protected:
    int m_fd {-1};
    
    epoll_event m_listen_events;

    std::function<void()> m_read_callback {nullptr};
    std::function<void()> m_write_callback {nullptr};
    std::function<void()> m_error_callback {nullptr};


};





}






#endif