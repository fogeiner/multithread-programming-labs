#include "Task.h"
#include "SwitchControlledAsync.h"
#include "../AsyncDispatcher/AsyncDispatcher.h"

class WriteTask : public Task {
private:
    SwitchControlledAsync *_d;
public:

    WriteTask(SwitchControlledAsync *async) : _d(async) {
    }

    void run() {
        try {
            _d->handle_write();
        } catch (std::exception &ex) {
            _d->activate();
            throw;
        }
        _d->activate();
    }
};
