#include "Client.h"
#include "../libs/Buffer/VectorBuffer.h"

class Client {
private:
    TCPSocket *_sock;
    Buffer *_in;
    Buffer *_out;
    int _bytes_sent;

public:

    static void *run(void *client_ptr) {
        bool again;
        /*
                do {
                    again = false;
                    try {
                        f();
                    } catch (A &a) {
                        printf("A");
                    } catch (AGAIN &a) {
                        printf("\nAGAIN\n");
                        again = true;
                    }
                } while (again);*/

        // receiving data until \r\n\r\n
        // if client closes connection -- quit

        // parsing request
        // if it's uncorrect -- get cached error msg page
        // and send it all
        // if error -- quit

        // if request is OK then we should
        // check Cache for such entry

        // in case there's one and CACHED then we just
        // copy contents to the _out and send_it_all

        // in case there's one and it's CACHING then we just
        // add this client to than Entry

        // in case there's no such Entry, we create one;
        // Entry is meant to download data by herself

        // come to producer-consumer loop with CV and mutex synchronization
        // until Entry is Cached or damaged
        // checking flag of Entry {CACHING, CACHED, CONN_ERROR, SEND_ERROR, RECV_ERROR}
        //

    }
};

extern "C"
void *client(void *arg_ptr) {

}