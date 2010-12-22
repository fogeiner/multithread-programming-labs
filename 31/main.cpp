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

#include <execinfo.h>

void term_handler() {
    void *trace_elems[50];
    int trace_elem_count(backtrace(trace_elems, 50));
    char **stack_syms(backtrace_symbols(trace_elems, trace_elem_count));
    Logger::emergent("Trace start:");
    for (int i = 0; i < trace_elem_count; ++i) {
        Logger::emergent(stack_syms[i]);
    }
    Logger::emergent("Trace end.");
    free(stack_syms);

    exit(1);
}

void sig_handler(int sig) {
    Logger::emergent("Signal %d", sig);
    term_handler();
}

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);
    std::set_terminate(term_handler);
    signal(SIGSEGV, sig_handler);
    signal(SIGABRT, sig_handler);

    int threads_count;
    std::vector<Thread> threads;

    if (argc < 2) {
        threads_count = 1;
    } else {
        threads_count = atoi(argv[1]);
    }

    Logger::set_level(Logger::INFO);

    TaskQueue *task_queue = new TaskQueue();
    task_queue->put(new SelectTask(task_queue));

    try {
        Cache::init();
        new Proxy();

        for (int i = 0; i < threads_count; ++i) {
            threads.push_back(Thread(TaskQueue::process, task_queue));
            threads[i].run();
        }
        //TaskQueue::process(task_queue);
    } catch (ThreadException &ex) {
        Logger::error("ThreadException: %s", ex.what());
    } catch (BindException &ex) {
        Logger::error("BindException: %s", ex.what());
        return EXIT_FAILURE;
    }

    Thread::exit();
}
