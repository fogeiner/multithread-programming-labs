#pragma once

class ClientRetranslator {
public:
    virtual void client_finished() = 0;
    virtual ~ClientRetranslator(){}
};