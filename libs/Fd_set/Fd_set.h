#ifndef FD_SET_H
#define FD_SET_H
#include <sys/select.h>

class Fd_set {
private:
    fd_set _fdset;
    int _max_fd;
public:

    Fd_set() {
        this->zero();
        _max_fd = -1;
    }

    ~Fd_set() {
    }

    fd_set& fdset() {
        return _fdset;
    }

    void zero() {
        FD_ZERO(&_fdset);
    }

    void set(int fd) {
        FD_SET(fd, &_fdset);
        if (fd > _max_fd) {
            _max_fd = fd;
        }
    }

    void clr(int fd) {
        FD_CLR(fd, &_fdset);
        // actually have to update _max_fd
    }

    int isset(int fd) {
        return FD_ISSET(fd, &_fdset);
    }

    int max_fd() const {
        return _max_fd;
    }
};
#endif