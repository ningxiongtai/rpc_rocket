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
        OUT_EVENT = EPOLLOUT

    };

    Fdevent(int fd);

    ~Fdevent();

    std::function<void()> handler(FdTriggerEvent event_type);

    void listen(FdTriggerEvent event_type, std::function<void()> callback);
    
    int getFd() {
        return m_mid;
    }
    
    epoll_event getEpollEvent() {
        return m_listen_events;
    }
protected:
    int m_mid {-1};
    
    epoll_event m_listen_events;

    std::function<void()> m_read_callback;
    std::function<void()> m_write_callback;


};





}






#endif