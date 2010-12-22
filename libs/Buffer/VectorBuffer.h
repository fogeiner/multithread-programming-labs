#pragma once
#include "Buffer.h"
#include <vector>
#include <cassert>

class VectorBuffer : public Buffer {
private:
    std::vector<char> _v;

public:

    VectorBuffer() {
    }

    VectorBuffer(const char *buf) {
        this->append(buf);
    }

    VectorBuffer(const VectorBuffer &orig) :
    _v(orig._v) {
    }

    VectorBuffer & operator=(const VectorBuffer &orig) {
        if (this == &orig) {
            return *this;
        }

        _v = orig._v;
        return *this;
    }

    VectorBuffer(const char *buf, size_t length) : _v(buf, buf + length) {
    }

    virtual void append(const Buffer *buffer) {
        this->append(buffer->buf(), buffer->size());
    }

    virtual void append(const Buffer &buffer) {
        this->append(&buffer);
    }

    virtual void append(const char *buf, size_t length) {
        this->_v.insert(this->_v.end(), buf, buf + length);
    }

    virtual void append(const char *buf) {
        size_t i;
        for (i = 0; *(buf + i) != '\0'; ++i)
            ;
        this->_v.insert(this->_v.end(), buf, buf + i);
    }

    virtual const char* buf() const {
        return &this->_v[0];
    }

    virtual size_t size() const {
        return this->_v.size();
    }

    virtual Buffer *subbuf(size_t start, size_t end) const {
        assert(start >= 0);
        assert(end >= 0);
        assert(end >= start);
        assert(end <= this->size());

        return new VectorBuffer(&_v[start], end - start);
    }

    virtual Buffer *first(size_t count) const {
        return this->subbuf(0, count);
    }

    virtual Buffer *last(size_t count) const {
        size_t size = this->size();
        return this->subbuf(size - count, size);
    }

    virtual void drop_first(size_t count) {
        assert(count <= this->size());
        this->_v.erase(this->_v.begin(), this->_v.begin() + count);
    }

    virtual void drop_last(size_t count) {
        assert(count <= this->size());
        this->_v.erase(this->_v.begin() + this->size() - count, this->_v.end());
    }

    virtual char at(size_t index) const {
        return (*this)[index];
    }

    virtual char operator[](size_t index) const {
        assert(index >= 0);
        assert(index <= this->size());
        return this->_v[index];
    }

    virtual Buffer & operator+=(const Buffer &another) {
        return (*this) += &another;
    }

    virtual Buffer & operator+=(const Buffer *another) {
        this->append(another);
        return *this;
    }

    virtual bool is_empty() const {
        return this->size() == 0 ? true : false;
    }

    virtual void clear() {
        this->drop_first(this->size());
    }

    virtual operator std::string() const {
        return std::string(_v.begin(), _v.end());
    }

    virtual ~VectorBuffer() {
    }
};

