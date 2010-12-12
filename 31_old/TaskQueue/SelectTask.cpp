#include "../../libs/Logger/Logger.h"
#include "SelectTask.h"
#include "TaskQueue.h"
#include "AcceptTask.h"
#include "CloseTask.h"
#include "ConnectTask.h"
#include "ReadTask.h"
#include "WriteTask.h"

void SelectTask::run() {
    Logger::debug("SelectTask run");
    std::list<Selectable*> rlist, wlist;

    AsyncDispatcher::_sockets_mutex.lock();
    std::list<AsyncDispatcher*> delete_list;
    for (std::list<AsyncDispatcher*>::iterator d = AsyncDispatcher::_sockets.begin();
            d != AsyncDispatcher::_sockets.end(); ++d) {
        if ((*d)->is_closed()) {
            delete_list.push_back(*d);
        }
    }

    for (std::list<AsyncDispatcher*>::iterator d = delete_list.begin();
            d != delete_list.end(); ++d) {
        AsyncDispatcher::_sockets.remove(*d);
        delete *d;
     }

    rlist.push_back(&AsyncDispatcher::_signal_pipe);

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

        if (*i == &AsyncDispatcher::_signal_pipe) {
            AsyncDispatcher::_signal_pipe.reset_signal();
            continue;
        }

        AsyncDispatcher *ad = NULL;
        for (std::list<AsyncDispatcher*>::iterator d = AsyncDispatcher::_sockets.begin();
                d != AsyncDispatcher::_sockets.end(); ++d) {
            if (*i == (*d)->_s) {
                ad = *d;
                break;
            }
        }

        if (ad->_s->get_state() == TCPSocket::LISTENING) {
            ad->deactivate();
            this->_tq->put(new AcceptTask(ad));
            //ad -> handle_accept();
        }
        assert(ad != NULL);
        if (ad->_s->get_state() == TCPSocket::CONNECTED) {
            if (ad->_s->peek() == 0) {
                ad->deactivate();
                this->_tq->put(new CloseTask(ad));
                //ad->handle_close();
            } else {
                ad->deactivate();
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
            ad->deactivate();
            this->_tq->put(new WriteTask(ad));
            //ad->handle_write();
        }

        if (ad->_s->get_state() == TCPSocket::CONNECTING) {
            ad->deactivate();
            this->_tq->put(new ConnectTask(ad));
            //ad->handle_connect();
        }
    }

    AsyncDispatcher::_sockets_mutex.unlock();

    this->_tq->put(new SelectTask(this->_tq));
}