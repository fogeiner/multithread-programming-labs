#include "../libs/Thread/Thread.h"
#include "../libs/Logger/Logger.h"

#include "TaskQueue/TaskQueue.h"
#include "TaskQueue/SelectTask.h"
#include "Proxy/Proxy.h"

#include "config.h"

#include <cstdlib>
#include <vector>

void init_logger() {
    Logger::set_ident(ProxyConfig::ident);
    Logger::set_level(Logger::DEBUG);
}

int main(int argc, char *argv[]) {
    int threads_count;
    if(argc < 2){
        threads_count = 1;
    } else {
        threads_count = atoi(argv[1]);
    }
    try {
        init_logger();
        TaskQueue *task_queue = new TaskQueue();
        task_queue->put(new SelectTask(task_queue));


        new Proxy();

        // creating threads with TaskQueue::process
        std::vector<Thread> threads;
        for(int i = 0; i < threads_count - 1; ++i){
            threads.push_back(Thread(TaskQueue::process, task_queue));
        }

        for(unsigned int i = 0; i < threads.size(); ++i){
            threads[i].run();
        }

        TaskQueue::process(task_queue);
    } catch (std::exception &ex) {
        Logger::error(ex.what());
        return EXIT_FAILURE;
    }
}
