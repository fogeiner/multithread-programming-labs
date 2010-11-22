#define DEBUG
#include "TCPSocket.h"
#include <cstdio>
#include <list>

int main(){
	try{
		TCPSocket *l_s = new TCPSocket();
		l_s->bind(8080);
		l_s->listen(5);

		std::list<Selectable*> rlist;
		rlist.push_back(l_s);

		Select(&rlist, NULL, NULL);

		fprintf(stdout, "RETURNED");

		delete l_s;
	} catch(TCPSocketException &ex){
		fprintf(stderr, "%s\n", ex.what());
	}
}
	
