#pragma once
#include <vector>
#include <queue>

#include "../../libs/Thread/Thread.h"
#include "../../libs/CondVar/CondVar.h"

class ThreadPool {
    std::vector<Thread> _threads;
    std::vector<Task*> _tasks;
    Mutex _tasks_mutex;
    CondVar _tasks_cv;
public:
    ThreadPool();
    ThreadPool(const ThreadPool& orig);
    virtual ~ThreadPool();

};
