#include "Task.h"

class TaskQueue;

class SelectTask : public Task {
private:
    TaskQueue *_tq;
public:

    SelectTask(TaskQueue *task_queue): _tq(task_queue) {
    }

    void run();
};