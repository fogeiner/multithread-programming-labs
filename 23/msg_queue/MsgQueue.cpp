#include <list>

#include "MsgQueue.h"

MsgQueue::~MsgQueue() {
}

int MsgQueue::get(char *buf, size_t bufsize) {

    _taken--;
    _mutex--;

    if (dropped) {
        _taken++;
        _mutex++;
        return 0;
    }

    std::string str = _msgs.back();
    _msgs.pop_back();

    const char *s = str.c_str();
    int str_length = str.length();

    int i;
    for (i = 0; (i < bufsize) && (i < str_length); ++i) {
        buf[i] = s[i];
    }

    _mutex++;
    _free++;
    return i;
}

int MsgQueue::put(const char *msg) {
    _free--;
    _mutex--;

    if (dropped) {
        _free++;
        _mutex++;
        return 0;
    }

    std::string str(msg);
    _msgs.push_front(str);

    _mutex++;
    _taken++;

    return str.length();
}

void MsgQueue::drop() {
    dropped = true;

    _mutex--;
    _free++;
    _taken++;

    _msgs.erase(_msgs.begin(), _msgs.end());

    _mutex++;
}

