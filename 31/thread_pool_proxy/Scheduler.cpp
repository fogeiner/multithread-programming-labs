#include "Scheduler.h"

std::queue<SocketTask*> Scheduler::_read_tasks;
std::queue<SocketTask*> Scheduler::_write_tasks;
Mutex Scheduler::_read_mutex;
Mutex Scheduler::_write_mutex;

void Scheduler::enqueue_read_task(SocketTask *t){
    _read_mutex.lock();
    _read_tasks.push(t);
    _read_mutex.unlock();
}
void Scheduler::enqueue_write_task(SocketTask *t){
    _write_mutex.lock();
    _write_tasks.push(t);
    _write_mutex.unlock();
}