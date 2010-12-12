#include"data.h"
#include"proxyexc.h"
#include<stdlib.h>
#include<iostream>
#include<cstring>

Data::Data()
{
    data=NULL;
    ended=false;
    cached=true;
    task=NULL;
    length=0;
    count=0;    
}

Data::Data(const Data& old)
{
    ended=old.isEnded();
    cached=old.isCached();
    task=old.getDownloadTask();
    length=old.getLength();
    count=old.getCount();
        
    if(length>0)
    {
	data=calloc(length, sizeof(char));
	if(NULL==data)
	{
	    CacheEx e;
	    throw(e);
	}
	memcpy(data, old.getData(), length);
    } else {
	data = NULL;
    }
}

Data::Data(const void* data, int length, bool ended)
{
    this->data=calloc(length, sizeof(char));
    this->ended=ended;
    cached=true;
    task=NULL;
    this->length=length;
    count=0;
    if(NULL==data)
    {
	CacheEx e;
	throw(e);
    }    
    memcpy(this->data, data, length);
}

const Data& Data::operator =(const Data& other)
{
    length=other.getLength();
    data = calloc(length, sizeof(char));
    ended = other.isEnded();
    cached=other.isCached();
    task = other.getDownloadTask();
    count=other.getCount();
    if(NULL==data) 
    {
	CacheEx e;
	throw(e);
    }
    memcpy(data, other.getData(), length);
    return other;
}

bool Data::addData(void* data, int len, bool ended)
{
    this->ended=ended;
    if(!cached)
    {
	return false;
    }
    if(length + len > limit)
    {
	cached=false;
	//free(data);
	//data=NULL;
	return false;
    }
    void* newData = calloc(length+len, sizeof(char));
    if(NULL==newData)
    {
	CacheEx e;
	throw(e);
    }
    memcpy(newData, this->data, length);
    memcpy((char *)newData + length, data, len);
    free(this->data);
    length+=len;
    this->data=newData;
    return true;
}

tasks* Data::getDownloadTask() const
{
    return task;
}	

void Data::setDownloadTask(tasks* newTask)
{
    task=newTask;
}

unsigned int Data::getCount() const
{
    return count;
}

void Data::addCount()
{
    count++;
}

int Data::getLength() const
{
    return length;
}

bool Data::isEnded() const
{
    return ended;
}

bool Data::isCached() const
{
    return cached;
}	

void* Data::getData() const
{
    return data;
}

void Data::setCached(bool value)
{	
    cached=value;
}

Data::~Data()
{
    free(data);
};
