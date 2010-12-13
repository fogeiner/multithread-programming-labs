#pragma once

#include <queue>
#include "SocketTask.h"
#include "../../libs/Mutex/Mutex.h"

class Scheduler {
    static std::queue<SocketTask*> _read_tasks;
    static std::queue<SocketTask*> _write_tasks;
    static Mutex _read_mutex;
    static Mutex _write_mutex;

    static Scheduler *instance();
    Scheduler();

public:
    static void enqueue_read_task(SocketTask *t);
    static void enqueue_write_task(SocketTask *t);

};
