#include"cache.h"
#include"proxyexc.h"
#include"download.h"
#include"proxyexc.h"
#include<iostream>

const std::string Cache::NOT_IMPLEMENTED("HTTP/1.0 501 NOT IMPLEMENTES\r\n\r\nNOT IMPLEMENTED\r\n");
const std::string Cache::NOT_SUPPORTED("HTTP/1.0 505 HTTP VERSION NOT SUPPORTED\r\n\r\nHTTP VERSION NOT SUPPORTED");
const std::string Cache::NOT_FOUND("HTTP/1.0 404 NOT FOUND\r\n\r\nNOT FOUND\r\n");

Cache::Cache(SelectTasks& sel):select(sel)
{
	pthread_mutex_init(&cacheLock, NULL);
	memcache.insert(std::pair<PageId, Data>(PageId(NOT_IMPLEMENTED, 0), Data(NOT_IMPLEMENTED.c_str(), NOT_IMPLEMENTED.length(), true)));
	memcache.insert(std::pair<PageId, Data>(PageId(NOT_SUPPORTED, 0), Data(NOT_SUPPORTED.c_str(), NOT_SUPPORTED.length(), true)));
	memcache.insert(std::pair<PageId, Data>(PageId(NOT_FOUND, 0), Data(NOT_FOUND.c_str(), NOT_FOUND.length(), true)));
	cacheSize=0;
	downloadInf=0;
}

void Cache::getExistElement(SocketClient& sock, std::string request, const PageId& page)
{
	Data& data=memcache[page];
	GetFromCache* getter = new GetFromCache(sock, select/*, page*/);
	getter->addData(data.getData(), data.getLength(), data.isEnded());
	if(data.isEnded())
	{
		std::cout << "All page in cache " << page.getPage() << std::endl;
		return;
	}
	std::cout << "Page is incomplit " << page.getPage() << std::endl;

	listeners[page].add(getter);        
}

void Cache::setCached(PageId& page, bool value)
{
	pthread_mutex_lock(&cacheLock);
	std::map<PageId, Data>::iterator iter=memcache.find(page);
	if(memcache.end()!=iter)
	{
		iter->second.setCached(value);
	}
	pthread_mutex_unlock(&cacheLock);
	std::cout << page.getPage() << " - uncached" <<std::endl;
}

void Cache::getNewPage(SocketClient& sock, std::string request, std::string host, const PageId& page)
{
	try
	{
		SocketClient socket(80, host.c_str());
		pthread_mutex_lock(&cacheLock);
		DownloadRequest* downReq = new DownloadRequest(socket, request, select);
		DownloadAnswer* downAns = new DownloadAnswer(socket, select, page, this);

		memcache[page].setDownloadTask(downAns);
		listeners[page].add(new GetFromCache(sock, select/*, page*/));

		select.addWrite(downReq);
		select.addRead(downAns);
		pthread_mutex_unlock(&cacheLock);
		std::cout << "New cache page - " << page.getPage() << std::endl;
		//std::cout << request << std::endl;
	}
	catch(SockEx e)
	{
		std::cout << "Exception in add new record in cache - ";
		std::cout << request << std::endl;
		getExistElement(sock, "", PageId(NOT_FOUND, 0));
		return;
	}

}
void Cache::cancel(PageId& page)
{
	pthread_mutex_lock(&cacheLock);
	//std::cout << memcache.size() << "   aaaaaa" << std::endl;
	cacheSize-= memcache[page].getLength();
	std::cout << "Download is canceled" << std::endl;
	//std::cout << memcache.size() << "   aaaaaa" << std::endl;
	memcache.erase(page);
	//std::cout << memcache.size() << "   aaaaa"  << std::endl;
	Listeners& list = listeners[page];
	for(int i=0; i<list.getLength(); ++i)
	{
		list[i]->cancel();
		//list.remove(i);
	}
	pthread_mutex_unlock(&cacheLock);
}

void Cache::get(SocketClient& sock, ParseResult result)
{
	PageId id(result.page, 1);
	pthread_mutex_lock(&cacheLock);
	std::map<PageId, Data>::iterator iter=memcache.find(id);
	//std::cout << id.getPage() << std::endl;
	if(memcache.end()==iter)
	{
		pthread_mutex_unlock(&cacheLock);
		getNewPage(sock, result.query, result.host, id);
		return;
	}
	if(!(iter->second).isCached())
	{
		PageId newId(result.page, id.getCount()/* + 1*/);
		//id.addCount();
		memcache[newId]=Data();
		pthread_mutex_unlock(&cacheLock);
		getNewPage(sock, result.query, result.host, id);
		return;
	}
	//std::cout << iter->first.getPage() << std::endl << iter->first.getId() << std::endl;
	//std::cout << memcache.size() << std::endl;
	pthread_mutex_unlock(&cacheLock);
	getExistElement(sock, result.query, id);
}

void Cache::add(PageId page, void* data, int length, bool end)
{
	pthread_mutex_lock(&cacheLock);
	downloadInf+=length/(1024*1024);
	while(MAX_CACHE_SIZE<=cacheSize && !pages.empty())
	{
		std::cout << MAX_CACHE_SIZE <<" "<< cacheSize <<std::endl;
		PageId id = pages.front();
		std::map<PageId, Data>::iterator iter=memcache.find(id);
		pages.pop_front();
		if(memcache.end()==iter || !(iter->second).isCached())
		{
			continue;
		}
		cacheSize-=(iter->second).getLength();

		std::cout << "Cache record deleted" <<std::endl;

		memcache.erase(iter);
	}

	if(!memcache[page].addData(data, length, end))
	{
		int len = (memcache[page].isCached()) ? (memcache[page].getLength()) : 0;
		if(0!=len)
		{
			cacheSize-=len;
			std::cout<< "The record is very big" << std::endl;
		}
	}
	else
	{
		cacheSize+=length;
	}
	Listeners& list = listeners[page];
	for(int i=0; i<list.getLength(); ++i)
	{
		//std::cout << "LIST" << std::endl;
		if(list[i]->canceled())
		{
			list.remove(i);
			continue;
		}
		list[i]->addData(data, length, end);
		//std::cout << "ENDLIST" << std::endl;
	}
	check(page);
	if(end)
	{
		listeners.erase(page);
		pages.push_back(page);
	}
	pthread_mutex_unlock(&cacheLock);
}

void Cache::deleteDownloaders(PageId& page)
{
	//pthread_mutex_lock(&cacheLock);
	std::cout << "123" << std::endl;
	memcache[page].getDownloadTask()->cancel();
	memcache[page].setDownloadTask(NULL);
	//pthread_mutex_unlock(&cacheLock);
}

void Cache::check(PageId& page)
{
	Listeners& list = listeners[page];
	if(0==list.getLength())
	{
		std::cout << "12385" << std::endl;
		deleteDownloaders(page);
		//std::cout << memcache.size() << " bbbbbbbbbb" <<  std::endl;
		if(!memcache[page].isCached())
		{
			//std::cout << "cached not" << std::endl;

			if(NULL!=memcache[page].getDownloadTask())
			{
				memcache[page].getDownloadTask()->cancel();
			}
			memcache.erase(page);

		}
		else
		{
			if(memcache[page].isEnded())
			{
				//std::cout << "8" << std::endl;

				//memcache.erase(PageId(page.getPage(), 0));
				//memcache[PageId(page.getPage(), 0)]=memcache[page];
				//memcache.erase(page);
			}
			else if(!memcache[page].isEnded())
			{
				//memcache.erase(page);
				std::cout << "Delete refused pages" << std::endl;
			}
		}
	}
}

void Cache::remove(PageId page, tasks* task)
{
	pthread_mutex_lock(&cacheLock);
	Listeners& list = listeners[page];
	for(int i=0; i<list.getLength(); ++i)
	{
		if(task==list[i])
		{
			list.remove(i);
		}
	}
	check(page);
	pthread_mutex_unlock(&cacheLock);
}

Cache::~Cache()
{
	memcache.clear();
	std::map<PageId, Listeners>::iterator iter=listeners.begin();
	for(;iter!=listeners.end(); ++iter)
	{
		Listeners& list=iter->second;
		for(int j=0; j<list.getLength(); ++j)
		{
			delete(list[j]);
			list.remove(j);
		}
	}
	listeners.clear();
	pthread_mutex_destroy(&cacheLock);
}
