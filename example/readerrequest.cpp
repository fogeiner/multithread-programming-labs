#include<iostream>
#include"readerrequest.h"
#include"proxyexc.h"

ReaderRequest::ReaderRequest(SocketClient soc, SelectTasks& sel, ParserHTTP& pars):
    select(sel), parse(pars)
{
    sock=soc;
    mes="";
}

int ReaderRequest::execThis()
{
    const int SIZE_BUF = 65536;
    char message[SIZE_BUF];
    int length = SIZE_BUF - 1;
    try
    {
	sock.readForSoc((void*)message, length);
    }
    catch(SockEx e)
    {
	sock.close();
	std::cerr << "Exeption when reading request from client" << std::endl;
	std::cerr << e.what() << std::endl;
	return 0;
    }
    if(0<length)
    {
	message[length]='\0';
	mes+=std::string(message);
	int i=mes.length();
	if(std::string::npos == mes.find("\r\n\r\n"))
	{
	    select.addRead(this);
	    //std::cout << "int ReaderRequest::execThis()" << std::endl;
	    return 1;
	}
    }
    if(2>mes.length())
    {
	return 0;
    }
    mes.erase(mes.length()-2, 2);
    mes+="Connection: close \r\n\r\n";
    parse.parse(mes, sock);
    return 0;
}

int ReaderRequest::getSocket()
{
    return sock.getId();
}

void ReaderRequest::cancel()
{

}
