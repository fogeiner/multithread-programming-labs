#include<cstdlib>
#include<cstring>
#include"getfromcache.h"
#include"proxyexc.h"
#include<iostream>

GetFromCache::GetFromCache(SocketClient socketId, SelectTasks& sel/*, PageId pag*/)
    :select(sel), sock(socketId)//, page(pag)
{
    length=0;
    data=NULL;
    pthread_mutex_init(&forData, NULL);
    ended=false;
    inQueue=false;
    isCancelled=false;
}

void GetFromCache::addData(void* data, int length, bool end)
{
    //std::cout << this->length << "    " << length << std::endl;
    pthread_mutex_lock(&forData);
    if(isCancelled)
    {
	pthread_mutex_unlock(&forData);
	return;
    }
    void* newData=calloc((this->length + length), sizeof(char));
    if(NULL==newData)
    {
	std::cerr << "No free memory!" << std::endl;
	exit(0);
    }
    memcpy(newData, (this->data), (this->length));
    memcpy(((char*)newData + (this->length)), data, length);
    this->length = length + (this->length);
    if(NULL!=this->data)
    {
	free(this->data);
    }
    this->data=newData;
    ended=end;
    if(!inQueue)
    {
	inQueue=true;
	select.addWrite(this);
    }
    pthread_mutex_unlock(&forData);
}

int GetFromCache::getSocket()
{
    return sock.getId();
}

void GetFromCache::cancel()
{
    pthread_mutex_lock(&forData);
    std::cerr << "Writing client file was canceled!" << std::endl;
    isCancelled=true;
    pthread_mutex_unlock(&forData);
}

int GetFromCache::execThis()
{
    pthread_mutex_lock(&forData);
    if(isCancelled)
    {
	free(data);
	sock.close();
	pthread_mutex_unlock(&forData);
	return 0;
    }
    int len=length;
    try
    {
	sock.writeForSoc(data, len);
	//cout << "sock" << std::endl; 
    }
    catch(SockEx& e)
    {
	sock.close();
	std::cerr << "Exception in write data for client" << std::endl;
	free(data);
	data=NULL;
	isCancelled=true;
	inQueue=false;
	pthread_mutex_unlock(&forData);
	return 0;
    }
    length-=len;
    if(0==length && ended)
    {
	free(data);
	data=NULL;
	sock.close();
	isCancelled=true;
	inQueue=false;
	pthread_mutex_unlock(&forData);
	return 0;
    }
    if(0==length)
    {
	inQueue=false;
	free(data);
	data=NULL;
	pthread_mutex_unlock(&forData);
	//select.addWrite(this);
    	return 1;
    }
    void* newData=calloc(length, sizeof(char));
    if(NULL==newData)
    {
	std::cerr << "Not free memory!" << std::endl;
	exit(0);
    }
    memcpy(newData, (char*) data + len, length);
    free(data);
    data=newData;
    //select.addWrite(this);
    pthread_mutex_unlock(&forData);
    return 1;
}

bool GetFromCache::canceled()
{
    return isCancelled;
}
