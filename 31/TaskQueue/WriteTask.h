#include "Task.h"
#include "../AsyncDispatcher/Async.h"
#include "../AsyncDispatcher/AsyncDispatcher.h"

class WriteTask: public Task{
private:
    Async *_d;
    TaskQueue *_tq;
public:
    WriteTask(Async *async, TaskQueue *task_queue): _d(async), _tq(task_queue){}
    void run(){
        _d->handle_write();
    }
};