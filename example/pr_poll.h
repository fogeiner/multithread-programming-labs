#ifndef POLL_THREAD
#define POLL_THREAD
#include<pthread.h>
#include<string>
#include<queue>
#include"task.h"
#include"selecttask.h"

class Parametr
{
    public:
    std::queue<tasks*>& task;
    SelectTasks& sel;
    pthread_mutex_t* mutex_for_queue;
    pthread_cond_t* cond_for_queue;
    
    Parametr(std::queue<tasks*>& tasks, SelectTasks& select, pthread_mutex_t* mutex, pthread_cond_t* cond)
	: sel(select), task(tasks)
    {
	mutex_for_queue=mutex;
	cond_for_queue=cond;
    }
};

class threadPoll
{
    private:
    int size;
    pthread_t* pollThreads;
    std::queue<tasks*> queue_tasks;
    pthread_mutex_t mutex_for_queue;
    pthread_cond_t cond;
    Parametr* param;
    SelectTasks select;
    
    
    public:
    threadPoll(SelectTasks& sel, int thread_count=1);
    int performTask(tasks* task);
    
    ~threadPoll();
};

void* thread_body(void* param);

#endif
