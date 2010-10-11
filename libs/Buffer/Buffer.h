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
			if(_chunks.size() == 0){
				return NULL;
			}

			Chunk *ret_chunk_ptr = _chunks.back();
			_chunks.pop_back();
			return ret_chunk_ptr;
		}

		Chunk* pop_front() {
			if(_chunks.size() == 0){
				return NULL;
			}

			Chunk *ret_chunk_ptr = _chunks.front();
			_chunks.pop_front();
			return ret_chunk_ptr;
		}

		int put_back(Chunk *&chunk, int bytes_used){
			if(bytes_used == 0){
				push_front(chunk);
				return chunk->size();
			}

			int chunk_size = chunk->size();
			const char *buf = chunk->buf();
			if(bytes_used < chunk_size){
				Chunk *new_chunk = new Chunk((buf + bytes_used), (chunk_size - bytes_used));
				push_front(new_chunk);
			}
			delete chunk;
			chunk = NULL;
			return (chunk_size - bytes_used);
		}
};

#endif
