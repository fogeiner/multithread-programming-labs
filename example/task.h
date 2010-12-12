#ifndef TASK_H
#define TASK_H
#include<string>
class tasks
{
    public:
    
    virtual void cancel()=0;
    virtual int getSocket()=0;    
    virtual int execThis()=0;
};
#endif
