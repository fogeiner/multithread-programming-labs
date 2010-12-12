#include"parser.h"
#include<iostream>

ParserHTTP::ParserHTTP(Cache* cach)
    :cache(cach)
{

}

std::string ParserHTTP::getWorld(std::string& query)
{
    std::string world = "";
    while(0<query.length())
    {
	if(' '==query[0] || '\n'==query[0] || '\r'==query[0])
	{
	    if(0!=world.length())
	    {
		return world;
	    }
	    query.erase(0,1);
	    continue;
	}
	world+=query[0];
	query.erase(0,1);
    }
    return world;
}

void ParserHTTP::parse(std::string query, SocketClient& sock)
{
    ParseResult result;
    result.query = query;
    result.method = getWorld(query);
    //std::cout << result.method << std::endl;
    if(0!=result.method.compare("GET") && 0!=result.method.compare("HEAD"))
    {
	result.implement = false;
	result.page=Cache::NOT_IMPLEMENTED;
	cache->get(sock, result);
	return;
    }
    result.page=getWorld(query);
    //std::cout << result.page << std::endl;
    result.protocol=getWorld(query);
    //std::cout << result.protocol << std::endl;
    if(0!=result.protocol.compare("HTTP/1.0"))
    {
	result.protocol = "HTTP/1.0";
	//!!!!!!!!!!!!!!
    }
    do
    {
	result.host = getWorld(query);
    }while(0!=result.host.compare("Host:"));
    
    result.host = getWorld(query);
    
    if(result.page.length()>0 && '/'==result.page[0])
    {
	result.page=result.host+result.page;
    }
    result.implement=true;
    
    if(result.page.length()<7 || 0!=result.page.substr(0,7).compare("http://"))
    {
	result.host= "http://" + result.host;    
    }
    
    //std::cout << result.host << std::endl;
    
    cache->get(sock, result);
    
    return;
}

ParserHTTP::~ParserHTTP()
{
    delete(cache);
}
