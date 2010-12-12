#include "Task.h"
#include "SwitchControlledAsync.h"
#include "../AsyncDispatcher/AsyncDispatcher.h"

class AcceptTask : public Task {
private:
    SwitchControlledAsync *_d;
public:

    AcceptTask(SwitchControlledAsync *async) : _d(async) {
    }

    void run() {
        _d->handle_accept();
        _d->activate();
    }
};