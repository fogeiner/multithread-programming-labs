#include "Proxy/Proxy.h"
#include "../libs/Logger/Logger.h"
#include "config.h"
#include "TaskQueue/TaskQueue.h"
#include "TaskQueue/SelectTask.h"

void init_logger() {
    Logger::set_ident(ProxyConfig::ident);
    Logger::set_level(Logger::DEBUG);
}

int main(int argc, char *argv[]) {
//    try {
        init_logger();
        TaskQueue *task_queue = new TaskQueue();
        task_queue->put(new SelectTask(task_queue));

        // creating threads with TaskQueue::process

         new Proxy();
         TaskQueue::process(task_queue);
  //  } catch (std::exception &ex) {
 //      Logger::error(ex.what());
 //   }
}
