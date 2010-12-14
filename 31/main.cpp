#include "../libs/Thread/Thread.h"
#include "../libs/Logger/Logger.h"

#include "TaskQueue/TaskQueue.h"
#include "TaskQueue/SelectTask.h"
#include "Cache/Cache.h"
#include "Proxy/Proxy.h"

#include "config.h"

#include <cstdlib>
#include <vector>
#include <csignal>

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);
    int threads_count;

    if (argc < 2) {
        threads_count = 1;
    } else {
        threads_count = atoi(argv[1]);
    }

    Logger::set_level(Logger::DEBUG);

    TaskQueue *task_queue = new TaskQueue();
    task_queue->put(new SelectTask(task_queue));

    Cache::init();
    new Proxy();

    std::vector<Thread> threads;
    try {
        for (int i = 0; i < threads_count; ++i) {
            threads.push_back(Thread(TaskQueue::process, task_queue));
            threads[i].run();
        }
    } catch (ThreadException &ex) {
        Logger::error("main() %s", ex.what());
    }

    Thread::exit();
}
