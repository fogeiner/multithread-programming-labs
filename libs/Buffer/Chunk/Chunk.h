#ifndef CHUNK_H
#define CHUNK_H
#include <cstring>

class Chunk {
private:
    char *_buffer;
    int _size;

public:

    int size() const {
        return _size;
    }

    const char *buf() const {
        return _buffer;
    }

    Chunk(const char *buf, int size) : _size(size) {
        _buffer = new char[size];
        memcpy(_buffer, buf, size);
    }

    ~Chunk() {
        delete[] _buffer;
    }
};

#endif

