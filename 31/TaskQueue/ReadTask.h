#include "Task.h"
#include "../AsyncDispatcher/Async.h"
#include "../AsyncDispatcher/AsyncDispatcher.h"

class ReadTask : public Task {
private:
    Async *_d;
public:

    ReadTask(Async *async) : _d(async) {
    }

    void run() {
        _d->handle_read();
    }
};