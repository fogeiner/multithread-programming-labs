#ifndef SELECTTASK
#define SELECTTASK
#include<queue>
#include"task.h"

class SelectTasks
{
    private:
    std::queue<tasks*> readTasks;
    std::queue<tasks*> writeTasks;
    pthread_mutex_t mutexRead;
    pthread_mutex_t mutexWrite;
    
    public:
    SelectTasks();
    void addRead(tasks* newTask);
    void addWrite(tasks* newTask);
    
    tasks* getRead();
    tasks* getWrite();
    ~SelectTasks();
};

#endif
