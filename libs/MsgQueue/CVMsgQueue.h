#pragma once
#include "../CondVar/CondVar.h"
#include "../Mutex/Mutex.h"
#include "MsgQueue.h"
#include <list>


class CVMsgQueue: public MsgQueue {
private:
    bool _dropped;
	int _queue_size;
	CondVar _cv;
	Mutex _m;
public:

    CVMsgQueue(int queue_size = 10) :
    _dropped(false),
    _queue_size(queue_size){
    }
    ~CVMsgQueue();

    int get(char *buf, size_t bufsize);
    int put(const char *msg);
    void drop();
};
