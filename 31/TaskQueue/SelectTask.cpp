#include "SelectTask.h"
#include "TaskQueue.h"
#include "AcceptTask.h"
#include "CloseTask.h"
#include "ConnectTask.h"
#include "ReadTask.h"
#include "WriteTask.h"

void SelectTask::run() {
    std::list<Selectable*> rlist, wlist;

    for (std::list<AsyncDispatcher*>::iterator i = AsyncDispatcher::_sockets.begin();
            i != AsyncDispatcher::_sockets.end(); ++i) {
        AsyncDispatcher *s = *i;
        if (s->readable()) {
            rlist.push_back(s->_s);

        }
        if (s->writable()) {
            wlist.push_back(s->_s);
        }
    }

    Select(&rlist, &wlist, NULL, 0);

    for (std::list<Selectable*>::iterator i = rlist.begin();
            i != rlist.end(); ++i) {
        AsyncDispatcher *ad = NULL;
        for (std::list<AsyncDispatcher*>::iterator d = AsyncDispatcher::_sockets.begin();
                d != AsyncDispatcher::_sockets.end(); ++d) {
            if (*i == (*d)->_s) {
                ad = *d;
                break;
            }
        }

        if (ad->_s->get_state() == TCPSocket::LISTENING) {
            this->_tq->put(new AcceptTask(ad));
            //ad -> handle_accept();
        }
        assert(ad != NULL);
        if (ad->_s->get_state() == TCPSocket::CONNECTED) {
            if (ad->_s->peek() == 0) {
                this->_tq->put(new CloseTask(ad));
                //ad->handle_close();
            } else {
                this->_tq->put(new ReadTask(ad));
                //ad -> handle_read();
            }
        }
    }

    for (std::list<Selectable*>::iterator i = wlist.begin();
            i != wlist.end(); ++i) {
        AsyncDispatcher *ad = NULL;
        for (std::list<AsyncDispatcher*>::iterator d = AsyncDispatcher::_sockets.begin();
                d != AsyncDispatcher::_sockets.end(); ++d) {
            if (*i == (*d)->_s) {
                ad = *d;
                break;
            }
        }
        assert(ad != NULL);
        if (ad->_s->get_state() == TCPSocket::CONNECTED) {
            this->_tq->put(new WriteTask(ad));
            //ad->handle_write();
        }

        if (ad->_s->get_state() == TCPSocket::CONNECTING) {
 
            this->_tq->put(new ConnectTask(ad));
            //ad->handle_connect();
        }
    }

    this->_tq->put(new SelectTask(this->_tq));
}