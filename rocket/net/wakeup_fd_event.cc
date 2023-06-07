#include "rocket/net/wakeup_fd_event.h"
#include "rocket/common/log.h"
#include <unistd.h>
namespace rocket {

   WakeUpFdEvent::WakeUpFdEvent(int fd) : Fdevent(fd) {
 
   }

   WakeUpFdEvent::~WakeUpFdEvent() {

   }


    void WakeUpFdEvent::wakeup() {
        char buf[8] = {'a'};
        int rt = write(m_mid, buf, 8);
        if(rt != 8) {
            ERRORLOG("write to wake up fd less than 8 bytes, fd[%d]", m_mid);
        }
        DEBUGLOG("success read 8 bytes");
    }
} 

