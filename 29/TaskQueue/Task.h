#pragma once

class Task {
public:
    virtual void run() = 0;
    virtual void cancel(){}
    virtual ~Task() {
    }
};
