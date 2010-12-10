#include "Task.h"
#include "../AsyncDispatcher/Async.h"
#include "../AsyncDispatcher/AsyncDispatcher.h"

class AcceptTask : public Task {
private:
    Async *_d;
public:

    AcceptTask(Async *async) : _d(async) {
    }

    void run() {
        _d->handle_accept();
    }
};