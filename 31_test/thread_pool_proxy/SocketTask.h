#pragma once

#include "../../libs/TCPSocket/Selectable.h"
#include "../../libs/TCPSocket/TCPSocket.h"

class SocketTask: public Selectable {
public:
    SocketTask();
    virtual bool run() = 0;
    virtual ~SocketTask();
private:
protected:
    TCPSocket *_socket;
};
