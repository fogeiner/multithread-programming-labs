
#pragma once

class Buffer {
	public:
		virtual void append(const Buffer &buffer) = 0;
		virtual void append(const Buffer *buffer) = 0;
		virtual void append(const char *buf, int length) = 0;
		virtual void append(const char *buf) = 0;
		virtual const char* buf() const = 0 ;
		virtual int size() const = 0;
		virtual bool is_empty() const = 0;
		virtual Buffer *subbuf(int start, int end) const = 0;
		virtual Buffer *first(int count) const = 0;
		virtual Buffer *last(int count) const = 0;
		virtual void drop_first(int count) = 0;
		virtual void drop_last(int count) = 0;
		virtual char at(int index) const = 0;
		virtual Buffer &operator+=(const Buffer &another) = 0;
		virtual Buffer &operator+=(const Buffer *another) = 0;
		virtual char operator[](int index) const = 0;
                virtual void clear() = 0;
		virtual ~Buffer() {}
};
