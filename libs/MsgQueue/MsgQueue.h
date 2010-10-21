#pragma once
#include "../Semaphore/Semaphore.h"
#include <list>


class MsgQueue {
protected:
    std::list<std::string> _msgs;
public:

    MsgQueue(){}
    virtual ~MsgQueue(){}

    virtual int get(char *buf, size_t bufsize) = 0;
    virtual int put(const char *msg) = 0;
    virtual void drop() = 0;
};
