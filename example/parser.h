#ifndef PARSER_HTTP
#define PARSER_HTTP
#include<string>
#include"socketclient.h"
#include"cache.h"
#include"parseresult.h"

class ParserHTTP
{
    private:
    Cache* cache;
    std::string getWorld(std::string& query);
    
    public: 
    ParserHTTP(Cache* cach);
    void parse(std::string query, SocketClient& sock);
    ~ParserHTTP();
};

#endif
