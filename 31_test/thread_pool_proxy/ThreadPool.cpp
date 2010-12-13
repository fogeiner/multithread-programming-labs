#include "ThreadPool.h"
#include "../../libs/Logger/Logger.h"

    ThreadPool::ThreadPool(int threads_count = 1){
        try{
            for(int i = 0; i < threads_count; ++i){
                _threads.push_back(Thread(thread_function, (void*)&state));
                _threads[i].run();
            }
        } catch(ThreadException &ex){
            Logger::error("ThreadPool::ThreadPool: %s", ex.what());
        }
    }

    ThreadPool::ThreadPool(const ThreadPool& orig){
    }

    ThreadPool::~ThreadPool(){

    }
    static void *ThreadPool::thread_function(void *ptr){
        ThreadPoolSharedState *state = static_cast<ThreadPoolSharedState*>(ptr);

        while(1){
            
        }
    }