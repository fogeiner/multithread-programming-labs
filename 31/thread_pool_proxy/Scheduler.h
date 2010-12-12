#pragma once 
#include <queue>
#include "Task.h"
#include "../../libs/Mutex/Mutex.h"
class Scheduler {
    std::queue<Task*> _read_tasks;
    std::queue<Task*> _write_tasks;
    Mutex _read_mutex;
    Mutex _write_mutex;
    
public:
    Scheduler();
    Scheduler(const Scheduler& orig);
    virtual ~Scheduler();
};
