#include "Downloader.h"
#include "../libs/TCPSocket/TCPSocket.h"
#include "../libs/Buffer/VectorBuffer.h"

class CacheEntry;
class Downloader{
private:
    CacheEntry *ce;
    TCPSocket *_sock;
    Buffer *_in;
    Buffer *_out;
public:
    static void *run(void *downloader_ptr){
        bool again;

        // forming query to send

        // try to connect
        // if fail -- set Entry status to CON_ERROR
        // and quit

        // try to send request
        // if fail -- set Entry status to SEND_ERROR
        // and quit

        // reading request from server
        // as soon as there enough symbols to detect
        // HTTP code, parse header and in case
        // it's not 200, delete CacheEntry from Cache

        // if recv fails delete Entry from Cache (if it wasn't deleted earlier)
        // set Entry status to RECV_ERROR and quit

        // if recv = 0 then set Entry to Cached one (or just FINISHED)
        // and quit
    }
};