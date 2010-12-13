#pragma once
#include <vector>
#include <queue>

#include "../../libs/Thread/Thread.h"
#include "../../libs/CondVar/CondVar.h"

class ThreadPool {
    std::vector<Thread> _threads;
    std::vector<SocketTask*> _tasks;
    Mutex _tasks_mutex;
    CondVar _tasks_cv;

    struct ThreadPoolSharedState {
        std::vector<Thread> &_threads;
        std::vector<SocketTask*> &_tasks;
        Mutex &_tasks_mutex;
        CondVar &_tasks_cv;

        ThreadPoolSharedState(std::vector<Thread> &_threads,
                std::vector<SocketTask*> &_tasks,
                Mutex &_tasks_mutex,
                CondVar & _tasks_cv) :
        _threads(_threads), _tasks(_tasks), _tasks_mutex(_tasks_mutex), _tasks_cv(_tasks_cv) {
        }
    } state;
public:
    ThreadPool(int threads_count = 1);
    ThreadPool(const ThreadPool& orig);
    ~ThreadPool();
    static void *thread_function(void *ptr);
};
