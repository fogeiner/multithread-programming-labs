#include "Task.h"
#include "../AsyncDispatcher/Async.h"
#include "../AsyncDispatcher/AsyncDispatcher.h"

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