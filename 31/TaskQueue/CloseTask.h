#include "Task.h"
#include "SwitchControlledAsync.h"
#include "../AsyncDispatcher/AsyncDispatcher.h"

class CloseTask : public Task {
private:
    SwitchControlledAsync *_d;
public:

    CloseTask(SwitchControlledAsync *async) : _d(async) {
    }

    void run() {
        _d->handle_close();
        _d->activate();
    }
};
