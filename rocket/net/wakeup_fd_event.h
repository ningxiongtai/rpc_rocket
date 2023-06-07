#ifndef ROCKET_NET_WAKEUP_FDEVENT_H
#define ROCKET_NET_WAKEUP_FDEVENT_H

#include "rocket/net/fd_event.h"
#include "rocket/common/log.h"

namespace rocket {

class WakeUpFdEvent :public Fdevent {

public:
    WakeUpFdEvent(int fd);

    ~WakeUpFdEvent();


    void wakeup();
private:

};

}

#endif
