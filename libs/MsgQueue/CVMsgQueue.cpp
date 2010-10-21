#include <list>

#include "CVMsgQueue.h"

CVMsgQueue::~CVMsgQueue() {
}

int CVMsgQueue::get(char *buf, size_t bufsize) {


	_m.lock();
	while(_msgs.size() == 0 && !_dropped){
		_cv.wait(_m);
	}

	if(_dropped){
		_m.unlock();
		return 0;
	}

    std::string str = _msgs.back();
    _msgs.pop_back();

	_m.unlock();

    const char *s = str.c_str();
    int str_length = str.length();

    int i;
    for (i = 0; (i < bufsize) && (i < str_length + 1); ++i) {
        buf[i] = s[i];
    }
	buf[bufsize - 1] = '\0';

    return i;
}

int CVMsgQueue::put(const char *msg) {
	_m.lock();
	while(_msgs.size() >= _queue_size && !_dropped){
		_cv.wait(_m);
	}
	if(_dropped){
		_m.unlock();
		return 0;
	}
    std::string str(msg);
    _msgs.push_front(str);

	_cv.broadcast();
	_m.unlock();

    return str.length() + 1;
}

void CVMsgQueue::drop() {
    _dropped = true;

	_cv.broadcast();

	_m.lock();
    _msgs.erase(_msgs.begin(), _msgs.end());
	_m.unlock();
}

