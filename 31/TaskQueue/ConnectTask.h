#include "Task.h"
#include "../AsyncDispatcher/Async.h"
#include "../AsyncDispatcher/AsyncDispatcher.h"
#include "../../libs/Logger/Logger.h"
class ConnectTask : public Task {
private:
    Async *_d;
public:

    ConnectTask(Async *async) : _d(async) {
    }

    void run() {
        _d->handle_connect();
    }
};