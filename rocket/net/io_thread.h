#ifndef ROCKET_NET_IO_THREAD_H 
#define ROCKET_NET_IO_THREAD_H 
#include "rocket/common/log.h"
#include <pthread.h>
#include <semaphore.h>
#include "rocket/net/eventloop.h"

namespace rocket{

class IOThread
{

public:
    IOThread();
    ~IOThread();

    Eventloop* getEventloop(); 
    void start();
    void join();
public:
    static void* Main(void* arg);

private:
    pid_t m_thread_id {0}; //线程号
    pthread_t m_thread {0}; //线程句柄


    Eventloop* m_event_loop {NULL}; //线程IO的eventloop对象
    sem_t m_init_semaphore;
    sem_t m_start_semaphore;
};





}












#endif