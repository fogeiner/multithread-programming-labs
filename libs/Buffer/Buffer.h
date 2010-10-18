#ifndef BUFFER_H
#define BUFFER_H

#include "Chunk/Chunk.h"

#ifdef MT_BUFFER
	#include "../Mutex/Mutex.h"
#endif

#include <list>

class Buffer {
	private:
#ifdef MT_BUFFER
		mutable Mutex m;
#endif
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
#ifdef MT_BUFFER
			m.lock();
#endif
			for (std::list<Chunk*>::iterator i = _chunks.begin();
					i != _chunks.end();) {
				Chunk *chunk = *i;
				delete chunk;
				i = _chunks.erase(i);
			}
#ifdef MT_BUFFER
			m.unlock();
#endif
		}

		int size() const {
#ifdef MT_BUFFER
			m.lock();
#endif
			int size = _chunks.size();
#ifdef MT_BUFFER
			m.unlock();
#endif
			return size;
		}

		void push_back(Chunk *chunk) {
#ifdef MT_BUFFER
			m.lock();
#endif
			_chunks.push_back(chunk);
#ifdef MT_BUFFER
			m.unlock();
#endif
		}

		void push_front(Chunk *chunk) {
#ifdef MT_BUFFER
			m.lock();
#endif
			_chunks.push_front(chunk);
#ifdef MT_BUFFER
			m.unlock();
#endif
		}

		void push_back(const char *buf, int size) {
#ifdef MT_BUFFER
			m.lock();
#endif
			_chunks.push_back(_mk_chunk(buf, size));
#ifdef MT_BUFFER
			m.unlock();
#endif
		}

		void push_front(const char *buf, int size) {
#ifdef MT_BUFFER
			m.lock();
#endif
			_chunks.push_front(_mk_chunk(buf, size));
#ifdef MT_BUFFER
			m.unlock();
#endif
		}

		Chunk* pop_back() {
#ifdef MT_BUFFER
			m.lock();
#endif
			if(_chunks.size() == 0){
#ifdef MT_BUFFER
				m.unlock();
#endif
				return NULL;
			}

			Chunk *ret_chunk_ptr = _chunks.back();
			_chunks.pop_back();
#ifdef MT_BUFFER
			m.unlock();
#endif
			return ret_chunk_ptr;
		}

		Chunk* pop_front() {
#ifdef MT_BUFFER
			m.lock();
#endif
			if(_chunks.size() == 0){
#ifdef MT_BUFFER
				m.unlock();
#endif
				return NULL;
			}

			Chunk *ret_chunk_ptr = _chunks.front();
			_chunks.pop_front();
#ifdef MT_BUFFER
			m.unlock();
#endif
			return ret_chunk_ptr;
		}

		int put_back_front(Chunk *&chunk, int bytes_used){
#ifdef MT_BUFFER
			m.lock();
#endif
			if(bytes_used == 0){
				_chunks.push_front(chunk);
				int size = chunk->size();
#ifdef MT_BUFFER
				m.unlock();
#endif
				return size;
			}

			int chunk_size = chunk->size();
			const char *buf = chunk->buf();
			if(bytes_used < chunk_size){
				_chunks.push_front(_mk_chunk((buf + bytes_used), (chunk_size - bytes_used)));
			}
			delete chunk;
			chunk = NULL;
#ifdef MT_BUFFER
			m.unlock();
#endif
			return (chunk_size - bytes_used);
		}

		bool is_empty(){
#ifdef MT_BUFFER
			m.lock();
#endif
			int size =  _chunks.size();
#ifdef MT_BUFFER
			m.unlock();   
#endif
			return (size == 0) ? true : false;
		}
};

#endif

