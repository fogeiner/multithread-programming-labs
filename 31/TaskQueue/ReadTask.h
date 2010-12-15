#include "Task.h"
#include "SwitchControlledAsync.h"
#include "../AsyncDispatcher/AsyncDispatcher.h"

class ReadTask : public Task {
private:
    SwitchControlledAsync *_d;
public:

    ReadTask(SwitchControlledAsync *async) : _d(async) {
    }

    void run() {
        _d->handle_read();
        _d->activate();
    }
};
