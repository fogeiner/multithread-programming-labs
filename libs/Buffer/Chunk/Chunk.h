#ifndef CHUNK_H
#define CHUNK_H
#include <cstring>
#include <vector>

class Chunk {
private:
	std::vector<char> _buffer;

public:

    int size() const {
		_buffer.size();
    }

    const char *buf() const {
        return &_buffer[0];
    }

    Chunk(const char *buf, int size): _buffer(buf, buf + size)
	{}

    ~Chunk(){}
};

#endif

