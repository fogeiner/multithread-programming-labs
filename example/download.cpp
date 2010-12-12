#include"download.h"
#include"proxyexc.h"
#include<iostream>

DownloadRequest::DownloadRequest(SocketClient socket, std::string req, SelectTasks& sel)
    :sock(socket), request(req), select(sel)
{
    write=0;
    canceled=false;
}

int DownloadRequest::getSocket()
{
    return sock.getId();    
}

void DownloadRequest::cancel()
{
    //sock.close();
    canceled=true;
}

int DownloadRequest::execThis()
{
    if(canceled)
    {
	sock.close();
	return 0;
    }
    int left=request.length()-write;
    try
    {
	sock.writeForSoc(request.c_str() + write, left);
    }
    catch(SockEx& e)
    {
	sock.close();
	std::cerr << "Exception when write request to server" << std::endl;
	std::cerr << e.what() << std::endl;
	canceled=true;
	return 0;
    }
    write+=left;
    if(request.length()==write)
    {
	return 0;
    }
    else
    {
	select.addWrite(this);
	//select.addWrite(this);
	return 1;
    }
}

DownloadAnswer::DownloadAnswer(SocketClient socket, SelectTasks& sel, PageId pages, Cache* cach)
    :sock(socket), select(sel), page(pages), cache(cach)
{
    canceled=false;
    page.addCount();
}

int DownloadAnswer::getSocket()
{
    return sock.getId();    
}

void DownloadAnswer::cancel()
{
    canceled=true;
}

int DownloadAnswer::execThis()
{
    if(canceled)
    {
	sock.close();
	//cache->deleteDownloaders(page);
	cache->cancel(page);
	return 0;
    }
    const int SIZE_BUF=65536;
    //2^16 
    char data[SIZE_BUF];
    int length=SIZE_BUF-1;
    try
    {
	sock.readForSoc((void*) data, length);
    }
    catch(SockEx& e)
    {
	sock.close();
	cache->cancel(page);
	//cache->deleteDownloaders(page);
	std::cerr << "Exception when read answer from server" << std::endl;
	std::cerr << e.what() << std::endl;
	return 0;
    }
    if(0<length)
    {
	cache->add(page,data, length, false);
	select.addRead(this);
	//std::cout << "select.addRead(this);" <<std::endl;
	return 1;
    }
    cache->add(page, data, length, true);
    
    sock.close();
    //cache->deleteDownloaders(page);
    return 0;
}

DownloadAnswer::~DownloadAnswer()
{
    page.editCount();
}
