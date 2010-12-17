#include "../../libs/Logger/Logger.h"
#include "TaskQueue.h"
#include "Task.h"

#include <list>
#include <exception>

TaskQueue::~TaskQueue() {
}

Task *TaskQueue::get() {

    _taken--;
    _mutex--;

    if (_dropped) {
        _taken++;
        _mutex++;
        return NULL;
    }

    Task *t = _tasks.back();
    _tasks.pop_back();

    _mutex++;
    _free++;
    return t;
}

int TaskQueue::put(Task *t) {
    _free--;
    _mutex--;

    if (_dropped) {
        _free++;
        _mutex++;
        return 0;
    }

    _tasks.push_front(t);

    _mutex++;
    _taken++;
    return 1;
}

void TaskQueue::drop() {
    _dropped = true;

    _mutex--;
    // allowing all threads waiting for semaphores to unlock
    _free++;
    _taken++;

    _tasks.erase(_tasks.begin(), _tasks.end());

    _mutex++;
}

void *TaskQueue::process(void *task_queue_ptr) {
    TaskQueue *task_queue = static_cast<TaskQueue*> (task_queue_ptr);
    while (1) {
        Task *t = task_queue->get();
        Logger::debug("TaskQueue::process() got task");
        try {
            t->run();
        } catch (std::exception &ex) {
            Logger::error("TaskQueue::process() %s", ex.what());
        }
        Logger::debug("TaskQueue::process() finished task");
        delete t;
    }
    return NULL;
}
