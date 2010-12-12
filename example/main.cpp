#include<iostream>
#include<cstdlib>
#define _REENTRANT true
#include"selecttask.h"
#include"workserver.h"
#include"pr_poll.h"
#include"proxyexc.h"
#include"cache.h"

using namespace std;

int main(int argc, char** argv)
{
    if(3!=argc)
    {
	cout << "Counts thread!! or Port!!" << endl;
	exit(1);
    }
    SelectTasks* select = new SelectTasks();
    Cache* cache = new Cache(*select);
    ParserHTTP* parser = new ParserHTTP(cache);
    
    try
    {
	unsigned short port = atoi(argv[2]);
	select->addRead(new WorkWithServer(*select, *parser, port));
    }
    catch(SockEx& e)
    {
	std::cerr << "Can't start server" << std::endl;
	std::cerr << e.what() <<std::endl;
	exit(0);
    }
    int count=atoi(argv[1]);
    if(0>=count)
    {
	std::cout << "Invalid parametr : count threads;" << std::endl;
	exit(1);
    }
    threadPoll* poll = new threadPoll(*select, count);
    //std::cout << "End main" <<std::endl;
    pthread_exit(NULL);
}
