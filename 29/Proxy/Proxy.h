#pragma once
#include "../../libs/AsyncDispatcher/AsyncDispatcher.h"
class Client;

class Proxy : public AsyncDispatcher {
public:
    Proxy() ;
    bool readable() const;
    bool writable() const ;
    void handle_accept();
};

