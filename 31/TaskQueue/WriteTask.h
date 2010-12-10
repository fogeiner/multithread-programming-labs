#include "Task.h"
#include "../AsyncDispatcher/Async.h"
#include "../AsyncDispatcher/AsyncDispatcher.h"

class WriteTask: public Task{
private:
    Async *_d;
public:
    WriteTask(Async *async): _d(async){}
    void run(){
        _d->handle_write();
    }
};