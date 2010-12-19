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
        try {
            _d->handle_read();
        } catch (std::exception &ex) {
            _d->activate();
            throw;
        }
        _d->activate();
    }
};
