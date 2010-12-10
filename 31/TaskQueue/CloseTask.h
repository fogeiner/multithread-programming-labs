#include "Task.h"
#include "../AsyncDispatcher/Async.h"
#include "../AsyncDispatcher/AsyncDispatcher.h"

class CloseTask : public Task {
private:
    Async *_d;
public:

    CloseTask(Async *async) : _d(async) {
    }

    void run() {
        _d->handle_close();
    }
};