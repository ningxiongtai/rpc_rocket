#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <memory>
#include "rocket/common/log.h"
#include "rocket/common/config.h"
#include "rocket/net/eventloop.h"
#include "rocket/net/io_thread.h"
#include "rocket/net/fd_event.h"
#include "rocket/net/timer_event.h"
#include <string.h>
#include <unistd.h>

void test_io_thread() {

  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd == -1) {
    ERRORLOG("listenfd  = -1");
    exit(0);
  }

  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  
  addr.sin_port = htons(12330);
  addr.sin_family = AF_INET;
  inet_aton("127.0.0.1", &addr.sin_addr);
  int rt = bind(listenfd, reinterpret_cast<sockaddr*> (&addr), sizeof(addr));
  if(rt != 0) {
    ERRORLOG("bind error");
    exit(1);
  }
  rt = listen(listenfd, 100);
  if(rt != 0) {
    ERRORLOG("listen error");
    exit(1);
  }
  
  rocket::Fdevent event(listenfd);
  event.listen(rocket::Fdevent::IN_EVENT, [listenfd] () {
    sockaddr_in peer_addr;
    socklen_t addr_len = sizeof(peer_addr);
    memset(&peer_addr, 0, sizeof(peer_addr));
    int clientfd = accept(listenfd, reinterpret_cast<sockaddr*> (&peer_addr), &addr_len);

    inet_ntoa(peer_addr.sin_addr);
    DEBUGLOG("succeess get client fd[%d], peer addr [%s:%d]", clientfd, inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));

  });


    int i = 0;
    rocket::TimerEvent::s_ptr timer_event = std::make_shared<rocket::TimerEvent>(
      1000, true, [&i]() {
        INFOLOG("trigger timer event, count=%d", i++);
      }
    );

  

  rocket::IOThread io_thread;

  io_thread.getEventloop()->addEpollEvent(&event);

  io_thread.getEventloop()->addTimerEvent(timer_event);

  
  io_thread.start();
  io_thread.join();

}



int main() {

  rocket::Config::SetGlobalConfig("../conf/rocket.xml");

  rocket::Logger::InitGlobalLogger();

  test_io_thread();

  //rocket::Eventloop* eventloop = new rocket::Eventloop();

  // int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  // if (listenfd == -1) {
  //   ERRORLOG("listenfd  = -1");
  //   exit(0);
  // }

  // sockaddr_in addr;
  // memset(&addr, 0, sizeof(addr));
  
  // addr.sin_port = htons(12340);
  // addr.sin_family = AF_INET;
  // inet_aton("127.0.0.1", &addr.sin_addr);
  // int rt = bind(listenfd, reinterpret_cast<sockaddr*> (&addr), sizeof(addr));
  // if(rt != 0) {
  //   ERRORLOG("bind error");
  //   exit(1);
  // }
  // rt = listen(listenfd, 100);
  // if(rt != 0) {
  //   ERRORLOG("listen error");
  //   exit(1);
  // }
  
  // rocket::Fdevent event(listenfd);
  // event.listen(rocket::Fdevent::IN_EVENT, [listenfd] () {
  //   sockaddr_in peer_addr;
  //   socklen_t addr_len = sizeof(peer_addr);
  //   memset(&peer_addr, 0, sizeof(peer_addr));
  //   int clientfd = accept(listenfd, reinterpret_cast<sockaddr*> (&peer_addr), &addr_len);

  //   inet_ntoa(peer_addr.sin_addr);
  //   DEBUGLOG("succeess get client fd[%d], peer addr [%s:%d]", clientfd, inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));

  // });
  //   eventloop->addEpollEvent(&event);

  //   int i = 0;
  //   rocket::TimerEvent::s_ptr timer_event = std::make_shared<rocket::TimerEvent>(
  //     1000, true, [&i]() {
  //       INFOLOG("trigger timer event, count=%d", i++);
  //     }
  //   );
  //   eventloop->addTimerEvent(timer_event);
  //   eventloop->loop();




  return 0;
}