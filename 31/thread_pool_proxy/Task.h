#pragma once
#include "../../libs/TCPSocket/Selectable.h"

class Task: public Selectable {
public:
    Task();
    Task(const Task& orig);
    virtual bool run() = 0;
    virtual ~Task();
private:
protected:
};
