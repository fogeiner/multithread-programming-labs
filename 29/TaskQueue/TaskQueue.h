#pragma once
#include "../../libs/Semaphore/Semaphore.h"

#include <list>

class Task;

class TaskQueue {
private:
    bool _dropped;
    static const int MAX_QUEUE_SIZE = 8192;
    // mutex = i.e. binary semaphore
    Semaphore _mutex;
    // number of taken slots
    Semaphore _taken;
    // number of free slots
    Semaphore _free;

    std::list<Task*> _tasks;

public:

    TaskQueue(int queue_size = MAX_QUEUE_SIZE) :
    _dropped(false),
    _mutex(1),
    _taken(0),
    _free(queue_size) {
    }
    ~TaskQueue();

    Task *get();
    int put(Task *t);
    void drop();
    static void *process(void *task_queue_ptr);
};
