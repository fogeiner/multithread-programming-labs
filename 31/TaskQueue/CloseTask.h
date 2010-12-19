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
        try {
            _d->handle_close();
        } catch (std::exception &ex) {
            _d->activate();
            throw;
        }
        _d->activate();
    }
};
