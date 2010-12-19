#include "Task.h"
#include "SwitchControlledAsync.h"
#include "../AsyncDispatcher/AsyncDispatcher.h"
#include "../../libs/Logger/Logger.h"

class ConnectTask : public Task {
private:
    SwitchControlledAsync *_d;
public:

    ConnectTask(SwitchControlledAsync *async) : _d(async) {
    }

    void run() {
        try {
            _d->handle_connect();
        } catch (std::exception &ex) {
            _d->activate();
            throw;
        }
        _d->activate();
    }
};
