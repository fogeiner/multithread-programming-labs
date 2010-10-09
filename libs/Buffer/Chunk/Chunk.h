#ifndef CHUNK_H
#define CHUNK_H

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
        for (int i = 0; i < size; ++i) {
            _buffer[i] = buf[i];
        }
    }

    ~Chunk() {
        delete[] _buffer;
    }
};

#endif
