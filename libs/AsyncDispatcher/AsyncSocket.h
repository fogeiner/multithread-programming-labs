#pragma once

class AsyncSocket: public Selectable {
	public:
		virtual bool readable() = 0;
		virtual bool writable() = 0;
		virtual void handle_read() = 0;
		virtual void handle_write() = 0;
		virtual void handle_close() = 0;
		virtual void handle_accept() = 0;
		virtual ~AsyncSocket();
};
