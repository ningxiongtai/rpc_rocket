#include "rocket/net/fd_event.h"
#include "rocket/common/log.h"
#include <string.h>
namespace rocket
{

Fdevent::Fdevent(int fd) : m_fd(fd) {
    memset(&m_listen_events, 0, sizeof(m_listen_events));
}

Fdevent::~Fdevent() {


}

Fdevent::Fdevent() {
    memset(&m_listen_events, 0, sizeof(m_listen_events));
}

std::function<void()> Fdevent::handler(FdTriggerEvent event) {
    if(event == FdTriggerEvent::IN_EVENT) {
        return m_read_callback;
    }else {
        return m_write_callback;
    }
}

void Fdevent::listen(FdTriggerEvent event_type, std::function<void()> callback) {
    if(event_type == FdTriggerEvent::IN_EVENT) {
        m_listen_events.events |= EPOLLIN;
        m_read_callback = callback; 

    } else {
        m_listen_events.events = EPOLLOUT;
        m_write_callback = callback;
    } 
    m_listen_events.data.ptr = this;
}




} // namespace rocket
