#include"pr_poll.h"
#include<sys/select.h>
#include<malloc.h>
#include<iostream>

threadPoll::threadPoll(SelectTasks& sel, int thread_count)
{
    if(thread_count<=0)
    {
	this->size=1;
    } else {
	size=thread_count;
    }
    this->pollThreads = (pthread_t*) calloc(size, sizeof(pthread_t));
    if(!pollThreads)
    {
	size=0;
    }
    pthread_mutex_init(&mutex_for_queue, NULL);
    pthread_cond_init(&cond, NULL);
    param = new Parametr(queue_tasks, sel,&mutex_for_queue, &cond);
    for(int i=0; i<size; i++)
    {
	//std::cout << "create" << std::endl;
    	int code = pthread_create(pollThreads+i, NULL, thread_body, (void*) param);
	if(0!=code)
	{
	    size=i;
	}
    }
}

int threadPoll::performTask(tasks* task)
{
    pthread_mutex_lock(&mutex_for_queue);
    queue_tasks.push(task);
    pthread_mutex_unlock(&mutex_for_queue);
    pthread_cond_broadcast(&cond);
}

threadPoll::~threadPoll()
{
    free(pollThreads);
    while(!queue_tasks.empty())
    {
	queue_tasks.pop();
    }
}

int maxDes(SelectTasks& sel, std::queue<tasks*>& forRead, std::queue<tasks*>& forWrite,
	    fd_set& read_d, fd_set& write_d)
{
    int max=-1;
    tasks* task;
    while(NULL!=(task=sel.getRead()))
    {
	//std::cout << "Add read desc" << std::endl;
	forRead.push(task);
	FD_SET(task->getSocket(), &read_d);
	if(task->getSocket() > max)
	{
	    //std::cout << task->getSocket() << std::endl;
	    max=task->getSocket();
	}
    }
    while(NULL!=(task=sel.getWrite()))
    {
	//std::cout << "Add write desc" << std::endl;
	forWrite.push(task);
	FD_SET(task->getSocket(), &write_d);
	if(task->getSocket() > max)
	{
	    max=task->getSocket();
	}
    }
    return max;
}

void sortTasks(Parametr& param, std::queue<tasks*>& forRead, std::queue<tasks*>& forWrite,
		fd_set& read_d, fd_set& write_d)
{
    tasks* task;
    while(!forRead.empty())
    {
	task=forRead.front();
	forRead.pop();
	if(FD_ISSET(task->getSocket(), &read_d))
	{
	    pthread_mutex_lock(param.mutex_for_queue);
	    param.task.push(task);
	    pthread_mutex_unlock(param.mutex_for_queue);
	    pthread_cond_broadcast(param.cond_for_queue);
	} else {
	    param.sel.addRead(task);
	    //std::cout << "param.sel.addRead(task);" << std::endl;
	}
    }
    while(!forWrite.empty())
    {
	task=forWrite.front();
	forWrite.pop();
	if(FD_ISSET(task->getSocket(), &write_d))
	{
	    pthread_mutex_lock(param.mutex_for_queue);
	    param.task.push(task);
	    pthread_mutex_unlock(param.mutex_for_queue);
	    pthread_cond_broadcast(param.cond_for_queue);
	} else {
	    param.sel.addWrite(task);
	}
    }
}

bool selectForTasks(Parametr* param)
{
    fd_set read_d;
    fd_set write_d;
    FD_ZERO(&read_d);
    FD_ZERO(&write_d);
    std::queue<tasks*> forRead;
    std::queue<tasks*> forWrite;
    int max_d = maxDes(param->sel, forRead, forWrite, read_d, write_d);
    if(-1==max_d)
    {
	return false;
    }
    //std::cout << "select begin" << std::endl;
    if(0>=select(max_d+1, &read_d, &write_d, NULL, NULL))
    {
	std::cerr << "Error in select" << std::cerr;
    }
    //std::cout << "select end" << std::endl;
    sortTasks(*param, forRead, forWrite, read_d, write_d);
    //std::cout << "sortTask" << std::endl;
    return true;
}	

void* thread_body(void* param){
    Parametr* parametr = (Parametr*)(param);
    while(1)
    {
	//std::cout << "Begin init" << std::endl;
	bool readyDesc = selectForTasks(parametr);
	//std::cout << "End init. begin selected. result:" << readyDesc << std::endl;
	pthread_mutex_lock(parametr->mutex_for_queue);
	//std::cout << "Lock" << std::endl;
	if(parametr->task.empty() && !readyDesc)
	{
	    //std::cout << "Wait" << std::endl;
	    pthread_cond_wait(parametr->cond_for_queue, parametr->mutex_for_queue);
	    pthread_mutex_unlock(parametr->mutex_for_queue);
	    //std::cout << "No wait" << std::endl;
	    continue;
	}
	pthread_mutex_unlock(parametr->mutex_for_queue);
	//std::cout << "Run" << std::endl;
	while(1)
	{
	    tasks* task;
	    pthread_mutex_lock(parametr->mutex_for_queue);
	    if(!(parametr->task.empty()))
	    {
		task = parametr->task.front();
		parametr->task.pop();
	    } else {
		pthread_mutex_unlock(parametr->mutex_for_queue);
		//std::cout << "Break" << std::endl;
		break;
	    }
	    pthread_mutex_unlock(parametr->mutex_for_queue);
	    if(!(task->execThis()))
	    {
		delete(task);
	    }
	}
    }
    return NULL;
}
