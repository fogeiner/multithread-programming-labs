#pragma once

class Task {
public:

    virtual void run() {
    };

    virtual void cancel() {
    }

    virtual ~Task() {
    }
};
