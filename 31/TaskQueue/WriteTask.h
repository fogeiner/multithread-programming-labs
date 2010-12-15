#include "Task.h"
#include "SwitchControlledAsync.h"
#include "../AsyncDispatcher/AsyncDispatcher.h"

class WriteTask: public Task{
private:
    SwitchControlledAsync *_d;
public:
    WriteTask(SwitchControlledAsync *async): _d(async){}
    void run(){
        _d->handle_write();
        _d->activate();
    }
};
