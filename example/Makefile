CC=/usr/bin/g++

proxy: main.o cache.o data.o download.o getfromcache.o listeners.o pageid.o parser.o pr_poll.o readerrequest.o selecttask.o socketclient.o socketserver.o workserver.o proxyexc.o
	g++ -lpthread -o proxy_sol main.cpp cache.cpp data.cpp download.cpp getfromcache.cpp listeners.cpp pageid.cpp parser.cpp pr_poll.cpp readerrequest.cpp selecttask.cpp socketclient.cpp socketserver.cpp workserver.cpp proxyexc.cpp

clean:
	rm -f *.o proxy
