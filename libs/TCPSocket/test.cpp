#define DEBUG
#include "../Buffer/VectorBuffer.h"
#include "TCPSocket.h"
#include <cstdio>

int main(int argc, char *argv[]){
	try{
		TCPSocket s;
		s.connect("dirty.ru", 80);
		Buffer *in = new VectorBuffer();
		Buffer *out = new VectorBuffer();
		char query[] = "GET http://dirty.ru/ HTTP/1.0\r\n\r\n";
		in->append(query, sizeof(query)-1);


		s.send(in);

		while(s.recv(out)){}

		fprintf(stdout, "%s", out->buf());
		/*
		TCPSocket s1;

		s1.bind(8080);
		s1.listen(5);
		TCPSocket s2 = s1.accept();
		
		s2.send(in);
		*/
		delete in;
		delete out;
	} catch(SocketException &ex){
		fprintf(stdout, "%s\n", ex.what());
	}
	return 0;
}
