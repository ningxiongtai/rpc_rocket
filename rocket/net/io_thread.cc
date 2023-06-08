#include "rocket/common/log.h"
#include "rocket/net/io_thread.h"
#include "rocket/common/util.h"
#include <pthread.h>
#include <assert.h>
namespace rocket {
  IOThread::IOThread() {
    int rt = sem_init(&m_init_semaphore, 0, 0);
    assert(rt == 0);
    pthread_create(&m_thread, NULL,&IOThread::Main, this);
    //wait, 一直等到新线程执行完Main函数的前置
    sem_wait(&m_init_semaphore);
    
    DEBUGLOG("IOThread %d create success", m_thread_id);
  }

  IOThread::~IOThread() {
    m_event_loop->stop();//停loop
    sem_destroy(&m_init_semaphore);//摧毁信号量
    sem_destroy(&m_start_semaphore);

    pthread_join(m_thread, NULL);//等待当前线程结束

    if (m_event_loop) {
        delete m_event_loop;
        m_event_loop = NULL;
    }
  }


  void* IOThread::Main(void* arg) {
    IOThread* thread = static_cast<IOThread*> (arg); //静态转换

    thread->m_event_loop = new Eventloop();
    thread->m_thread_id = getThreadId();

    //唤醒等待线程
    sem_post(&thread->m_init_semaphore);
 
    DEBUGLOG("IOThread %d create, wait start semaphore", thread->m_thread_id);
    sem_wait(&thread->m_start_semaphore);
    DEBUGLOG("IOThread %d start loop", thread->m_thread_id);
    thread->m_event_loop->loop();

    DEBUGLOG("IOThread %d end loop", thread->m_thread_id);
    return NULL;
  }

  Eventloop* IOThread::getEventloop() {
    return m_event_loop;
  }

  void IOThread::start() {
    DEBUGLOG("Now invoke IOThread %d ", m_thread_id);
    sem_post(&m_start_semaphore);
  }

  void IOThread::join() {
    pthread_join(m_thread, NULL);
  }
}