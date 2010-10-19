/* 
 * File:   ThreadPool.h
 * Author: alstein
 *
 * Created on 17 Сентябрь 2010 г., 12:34
 */

#ifndef _THREADPOOL_H
#define	_THREADPOOL_H
#include <pthread.h>
#include <csignal>
#include <cstdlib>
#include <cerrno>
#include <list>



class ThreadPool {
    struct Job{
        
    };
public:
    ThreadPool();
    ThreadPool(const ThreadPool& orig);
    virtual ~ThreadPool();
    int queue(void *(*f)(void*), void *arg);
    void wait();
    int destroy();
private:

};

#endif	/* _THREADPOOL_H */
