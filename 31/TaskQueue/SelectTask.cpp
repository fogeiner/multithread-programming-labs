#include "SelectTask.h"

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
            ad -> handle_accept();
        }
        assert(ad != NULL);
        if (ad->_s->get_state() == TCPSocket::CONNECTED) {
            if (ad->_s->peek() == 0) {
                ad->handle_close();
            } else {
                ad -> handle_read();
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
            ad->handle_write();
        }

        if (ad->_s->get_state() == TCPSocket::CONNECTING) {
            ad->_s->validate_connect();
            ad->handle_connect();
        }
    }

}