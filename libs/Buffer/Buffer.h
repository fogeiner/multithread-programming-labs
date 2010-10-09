#ifndef BUFFER_H
#define BUFFER_H

#include "Chunk/Chunk.h"
#include <list>

class Buffer {
private:
    std::list<Chunk*>_chunks;

    Chunk *_mk_chunk(const char *buf, int size) const {
        return new Chunk(buf, size);
    }

    Buffer(const Buffer&){}
    Buffer& operator=(const Buffer&){}
public:

    Buffer() {

    }

    ~Buffer() {
        for (std::list<Chunk*>::iterator i = _chunks.begin();
                i != _chunks.end();) {
            Chunk *chunk = *i;
            delete chunk;
            i = _chunks.erase(i);
        }
    }

    int size() const {
        return _chunks.size();
    }

    void push_back(Chunk *chunk) {
        _chunks.push_back(chunk);
    }

    void push_front(Chunk *chunk) {
        _chunks.push_back(chunk);
    }

    void push_back(const char *buf, int size) {
        _chunks.push_back(_mk_chunk(buf, size));
    }

    void push_front(const char *buf, int size) {
        _chunks.push_front(_mk_chunk(buf, size));
    }

    Chunk* pop_back() {
        Chunk *ret_chunk_ptr = _chunks.back();
        _chunks.pop_back();
        return ret_chunk_ptr;
    }

    Chunk* pop_front() {
        Chunk *ret_chunk_ptr = _chunks.front();
        _chunks.pop_front();
        return ret_chunk_ptr;
    }
};

#endif