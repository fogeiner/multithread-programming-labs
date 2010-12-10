#include "Task.h"
#include "../AsyncDispatcher/Async.h"
#include "../AsyncDispatcher/AsyncDispatcher.h"

class SelectTask : public Task {
private:
    TaskQueue *_tq;
public:

    SelectTask(TaskQueue *task_queue): _tq(task_queue) {
    }

    void run();
};