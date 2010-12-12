#include<iostream>
#include"selecttask.h"

SelectTasks::SelectTasks()
{
    pthread_mutex_init(&mutexRead, NULL);
    pthread_mutex_init(&mutexWrite, NULL);
}

void SelectTasks::addRead(tasks* newTask)
{
    pthread_mutex_lock(&mutexRead);
    if(NULL==newTask)
    {
	std::cout << "NULL" <<std::endl;
    }
    readTasks.push(newTask);
    //std::cout << "add read Task" <<std::endl;
    //std::cout << "Size: " << readTasks.size() <<std::endl;
    pthread_mutex_unlock(&mutexRead);
}

void SelectTasks::addWrite(tasks* newTask)
{
    pthread_mutex_lock(&mutexWrite);
    writeTasks.push(newTask);
    //std::cout << "add write Task" <<std::endl;
    //std::cout << "Size: " << writeTasks.size() <<std::endl;
    pthread_mutex_unlock(&mutexWrite);
}

tasks* SelectTasks::getRead()
{
    pthread_mutex_lock(&mutexRead);
    //std::cout << "getRead" <<std::endl;
    if(readTasks.empty())
    {
	pthread_mutex_unlock(&mutexRead);
	return NULL;
    }
    tasks* task = readTasks.front();
    readTasks.pop();
    //std::cout << "Size: " << readTasks.size() <<std::endl;
    pthread_mutex_unlock(&mutexRead);
    return task;
}

tasks* SelectTasks::getWrite()
{
    pthread_mutex_lock(&mutexWrite);
    if(writeTasks.empty())
    {
	pthread_mutex_unlock(&mutexWrite);
	return NULL;
    }
    tasks* task = writeTasks.front();
    writeTasks.pop();
    pthread_mutex_unlock(&mutexWrite);
    return task;
} 

SelectTasks::~SelectTasks()
{
    while(!readTasks.empty())
    {
	readTasks.pop();
    }
    while(!writeTasks.empty())
    {
	writeTasks.pop();
    }
    pthread_mutex_destroy(&mutexRead);
    pthread_mutex_destroy(&mutexWrite);
}
