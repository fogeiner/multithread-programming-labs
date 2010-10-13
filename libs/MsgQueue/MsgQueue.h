#pragma once
#include "../Semaphore/Semaphore.h"
#include <list>


class MsgQueue {
private:
    bool dropped;

	// mutex = i.e. binary semaphore
    Semaphore _mutex;
	// number of taken slots
    Semaphore _taken;
	// number of free slots
    Semaphore _free;

    std::list<std::string> _msgs;
public:

    MsgQueue(int queue_size = 10) :
    dropped(false),
    _mutex(1),
    _free(queue_size),
    _taken(0),
    _msgs() {
    }
    ~MsgQueue();

    int get(char *buf, size_t bufsize);
    int put(const char *msg);
    void drop();
};
