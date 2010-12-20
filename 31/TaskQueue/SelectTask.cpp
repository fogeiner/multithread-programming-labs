#include "../../libs/Logger/Logger.h"
#include <cassert>

#include "SelectTask.h"
#include "TaskQueue.h"
#include "AcceptTask.h"
#include "CloseTask.h"
#include "ConnectTask.h"
#include "ReadTask.h"
#include "WriteTask.h"

void SelectTask::run() {
    Logger::debug("SelectTask::run() run");
    std::list<Selectable*> rlist, wlist;

    AsyncDispatcher::_sockets_mutex.lock();

    //  Logger::debug("SelectTask: deleting closed AsyncDispatchers");
    std::list<AsyncDispatcher*> delete_list;
    for (std::list<AsyncDispatcher*>::iterator d = AsyncDispatcher::_sockets.begin();
            d != AsyncDispatcher::_sockets.end(); ++d) {
        if ((*d)->is_closed()) {
            delete_list.push_back(*d);
        }
    }

    for (std::list<AsyncDispatcher*>::iterator d = delete_list.begin();
            d != delete_list.end();) {
        AsyncDispatcher::_sockets.remove(*d);
        delete *d;
        d = delete_list.erase(d);
    }



    //  Logger::debug("SelectTask: adding sockets");

    for (std::list<AsyncDispatcher*>::iterator i = AsyncDispatcher::_sockets.begin();
            i != AsyncDispatcher::_sockets.end(); ++i) {
        AsyncDispatcher *s = *i;
        if (!s->is_closed() && s->is_active() && s->readable()) {
            rlist.push_back(s->_s);
        }
        if (!s->is_closed() && s->is_active() && s->writable()) {
            wlist.push_back(s->_s);
        }
    }


    rlist.push_back(&AsyncDispatcher::_signal_pipe);

    AsyncDispatcher::_sockets_mutex.unlock();

    Logger::debug("SelectTask: Select()");
    Select(&rlist, &wlist, NULL, 0);

    AsyncDispatcher::_sockets_mutex.lock();

    //Logger::debug("SelectTask: rlist analysis");
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

        assert(ad != NULL);

        ad->deactivate();

        switch (ad->_s->get_state()) {
            case TCPSocket::LISTENING:
            {
                this->_tq->put(new AcceptTask(ad));
                break;
            }
            case TCPSocket::CONNECTED:
            {
                int peeked = ad->_s->peek();
                if (peeked == 0) {
                    this->_tq->put(new CloseTask(ad));
                } else if (peeked > 0) {
                    this->_tq->put(new ReadTask(ad));
                }
                break;
            }
            default:
                continue;
                assert(false);
        }
    }

    //Logger::debug("SelectTask: wlist analysis");
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

        if (!ad->is_active())
            continue;

        ad->deactivate();

        switch (ad->_s->get_state()) {
            case TCPSocket::CONNECTED:
                this->_tq->put(new WriteTask(ad));
                break;
            case TCPSocket::CONNECTING:
                this->_tq->put(new ConnectTask(ad));
                break;
            default:
                continue;
                assert(false);
        }
    }

    AsyncDispatcher::_sockets_mutex.unlock();
    this->_tq->put(new SelectTask(this->_tq));
}
