#ifndef BUFFER_H
#define BUFFER_H

#include "Chunk/Chunk.h"
#include "../Mutex/Mutex.h"

#include <list>

class Buffer {
	private:
		mutable Mutex m;

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
			m.lock();
			for (std::list<Chunk*>::iterator i = _chunks.begin();
					i != _chunks.end();) {
				Chunk *chunk = *i;
				delete chunk;
				i = _chunks.erase(i);
			}
			m.unlock();
		}

		int size() const {
			m.lock();
			int size = _chunks.size();
			m.unlock();
			return size;
		}

		void push_back(Chunk *chunk) {
			m.lock();
			_chunks.push_back(chunk);
			m.unlock();
		}

		void push_front(Chunk *chunk) {
			m.lock();
			_chunks.push_front(chunk);
			m.unlock();
		}

		void push_back(const char *buf, int size) {
			m.lock();
			_chunks.push_back(_mk_chunk(buf, size));
			m.unlock();
		}

		void push_front(const char *buf, int size) {
			m.lock();
			_chunks.push_front(_mk_chunk(buf, size));
			m.unlock();
		}

		Chunk* pop_back() {
			m.lock();
			if(_chunks.size() == 0){
				m.unlock();
				return NULL;
			}

			Chunk *ret_chunk_ptr = _chunks.back();
			_chunks.pop_back();
			m.unlock();
			return ret_chunk_ptr;
		}

		Chunk* pop_front() {
			m.lock();
			if(_chunks.size() == 0){
				m.unlock();
				return NULL;
			}

			Chunk *ret_chunk_ptr = _chunks.front();
			_chunks.pop_front();
			m.unlock();
			return ret_chunk_ptr;
		}

		int put_back_front(Chunk *&chunk, int bytes_used){
			m.lock();
			if(bytes_used == 0){
				_chunks.push_front(chunk);
				int size = chunk->size();
				m.unlock();
				return size;
			}

			int chunk_size = chunk->size();
			const char *buf = chunk->buf();
			if(bytes_used < chunk_size){
				_chunks.push_front(_mk_chunk((buf + bytes_used), (chunk_size - bytes_used)));
			}
			delete chunk;
			chunk = NULL;
			m.unlock();
			return (chunk_size - bytes_used);
		}

		bool is_empty(){
			m.lock();
			int size =  _chunks.size();
			m.unlock();   
			return (size == 0) ? true : false;
		}
};

#endif
