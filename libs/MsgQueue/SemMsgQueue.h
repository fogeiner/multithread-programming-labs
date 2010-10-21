#pragma once
#include "../Semaphore/Semaphore.h"
#include "MsgQueue.h"
#include <list>


class SemMsgQueue: public MsgQueue {
private:
    bool _dropped;

	// mutex = i.e. binary semaphore
    Semaphore _mutex;
	// number of taken slots
    Semaphore _taken;
	// number of free slots
    Semaphore _free;

public:

    SemMsgQueue(int queue_size = 10) :
    _dropped(false),
    _mutex(1),
    _free(queue_size),
    _taken(0){
    }
    ~SemMsgQueue();

    int get(char *buf, size_t bufsize);
    int put(const char *msg);
    void drop();
};
