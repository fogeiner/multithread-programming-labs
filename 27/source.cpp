#include <iostream>
#include <string>
#include <cerrno>

#include <aio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>

#include <unistd.h>

#include "../libs/Buffer/Buffer.h"
#include "../libs/Fd_set/Fd_set.h"
#include "../libs/Terminal/terminal.h"

#define DEBUG

int init_tcp_socket() {
	int socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return socket;
}

int init_remote_host_sockaddr(sockaddr_in &remote_addr, const char *remote_host, unsigned short remote_port) {
	// in static memory, no need to call free

	bzero(&remote_addr, sizeof (remote_addr));

	struct hostent *remote_hostent = gethostbyname(remote_host);

	if (remote_hostent == NULL) {
		return -1;
	}

	remote_addr.sin_addr = *((in_addr *) remote_hostent->h_addr_list[0]);
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = remote_port;
	return 0;
}

int parse_arguments(std::string url, std::string &host, std::string &path) {
	// starts with http://

	if (url.substr(0, 7) != "http://") {
		std::cerr << "URL required" << std::endl;
		return -1;
	}

	url = url.substr(7, url.length());
	int slash_index = url.find_first_of('/', 0);

	if (slash_index != -1) {
		host = url.substr(0, slash_index);
		path = url.substr(slash_index, url.length());
	} else {
		host = url;
		path = '/';
	}

#ifdef DEBUG
	std::clog << "Host: " << host << "\tPath: " << path << std::endl;
#endif
}

void print_screen(Buffer &buf, bool &screen_full, int rows, int cols) {

	static const char msg_to_press_key[] = "Press enter to scroll...";
	static int cur_row = 0, cur_col = 0;
	int next_tab_position;
	const int DEFAULT_TAB_WIDTH = 8;

	for (;;) {
		if(buf.is_empty()){
			return;
		}

		Chunk *chunk = buf.pop_front();

		int chunk_size = chunk->size();
		const char *b = chunk->buf();

		for (int i = 0; i < chunk_size; ++i) {
			switch (b[i]) {
				case '\t':
					next_tab_position = DEFAULT_TAB_WIDTH * 
						((cur_col + DEFAULT_TAB_WIDTH)/DEFAULT_TAB_WIDTH);
					if (next_tab_position <= cols){
						cur_col += next_tab_position;
						break;
					}
				case '\n':
					cur_row++;
					cur_col = 0;
					break;
				default:
					cur_col++;
			}

			std::cout << b[i];

			if (cur_col == cols) {
				cur_col = 0;
				cur_row++;
			}

			if (cur_row == rows) {
				cur_row = cur_col = 0;
				std::cout << msg_to_press_key;
				std::cout.flush();
				screen_full = true;
				buf.put_back_front(chunk, i);
				return;
			}
		}

		delete chunk;
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " url" << std::endl;
		return EXIT_FAILURE;
	}

	const int BUFSIZE = 1024;
	const int HTTP_DEFAULT_PORT = 80;
	const int CLOSED_SOCKET = -1;
	int screen_rows_count, screen_cols_count;

	if (get_terminal_width_height(STDOUT_FILENO, &screen_cols_count, &screen_rows_count) == -1) {
		std::cerr << strerror(errno) << std::endl;
	}

	if(!isatty(STDIN_FILENO)){
		std::cerr <<  "Standard input device is not a terminal" << std::endl;
		exit(EXIT_FAILURE);
	}

	if(term_canon_off() == -1){
		std::cerr << strerror(errno) << std::endl;
		return NULL;
	}
	try{
#ifdef DEBUG
		std::clog << "Terminal size: " << screen_rows_count << "x" << screen_cols_count << std::endl;
#endif

		int serv_socket = init_tcp_socket();

		if (serv_socket == -1) {
			std::cerr << strerror(errno) << std::endl;
			return EXIT_FAILURE;
		}

		std::string host, path, url(argv[1]);

		if (parse_arguments(url, host, path) == -1) {
			return EXIT_FAILURE;
		}

		sockaddr_in remote_addr;

		if (init_remote_host_sockaddr(remote_addr, host.c_str(), htons(HTTP_DEFAULT_PORT)) == -1) {
			std::cerr << "Getting remote_host info: " << hstrerror(h_errno) << std::endl;
			return EXIT_FAILURE;
		}

		if (connect(serv_socket, (const sockaddr*) & remote_addr, sizeof (remote_addr)) == -1) {
			std::cerr << strerror(errno) << std::endl;
			return EXIT_FAILURE;
		}


		Buffer recv_buf;
		std::string request = "GET " + path + " HTTP/1.0\r\nHost: " + host + "\r\n\r\n";
		
		int request_length = request.length() + 1;

		char request_str[request_length - 1];
		request_str[request_length] = '\0';
		for(int i = 0; i < request_length - 1; ++i){
			request_str[i] = request[i];
		}
		
		int request_offset = 0;

		static struct aiocb socket_writerq;
		bool socket_writerq_inprogress = false;

		static struct aiocb socket_readrq;
		bool socket_readrq_inprogress = false;
		
		static struct aiocb stdin_readrq;
		bool stdin_readrq_inprogress = false;

		static struct aiocb *const rq_list[3] = {&socket_writerq, &socket_readrq, &stdin_readrq};

		int ret;

		char symbol;
		char buf[BUFSIZE];
		bool screen_full = false;
		bool GET_sent = false;

		for (;;) {
			// all is read, all is shown
			if ((serv_socket == CLOSED_SOCKET) && (recv_buf.size() == 0)) {
				break;
			}


			bzero(&socket_writerq, sizeof(socket_writerq));
			bzero(&socket_readrq, sizeof(socket_readrq));
			bzero(&stdin_readrq, sizeof(stdin_readrq));

	
			// initing requests
			if(!stdin_readrq_inprogress){
				stdin_readrq.aio_fildes = STDIN_FILENO;
				stdin_readrq.aio_buf = &symbol;
				stdin_readrq.aio_nbytes = 1;
			}

			if ((GET_sent == false) && (!socket_writerq_inprogress)){
				socket_writerq.aio_fildes = serv_socket;
				socket_writerq.aio_buf = request_str + request_offset;
				socket_writerq.aio_nbytes = request_length - request_offset;
				socket_writerq.aio_lio_opcode = LIO_WRITE;
			}

			if ((serv_socket != CLOSED_SOCKET) && (!socket_readrq_inprogress)){
				socket_readrq.aio_fildes = serv_socket;
				socket_readrq.aio_buf = buf;
				socket_readrq.aio_nbytes = sizeof(buf);
				socket_readrq.aio_lio_opcode = LIO_READ;
			}

			lio_listio(LIO_NOWAIT, rq_list, sizeof(rq_list)/sizeof(rq_list[0]), NULL);
			
			// testing results
			// GET sending
			if(GET_sent == false){
				ret = aio_error(&socket_writerq);
				if(ret == EINPROGRESS){
					socket_writerq_inprogress = true;
				} else if(ret == 0) {
					socket_writerq_inprogress = false;
				} else {
					std::cerr << strerror(ret) << std::endl;
					break;
				}
			}


			// reply receiving
			if(serv_socket != CLOSED_SOCKET){
				ret = aio_error(&socket_readrq);
				if(ret == EINPROGRESS){
					socket_readrq_inprogress = true;
				} else if(ret == 0) {
					socket_readrq_inprogress = false;
				} else {
					std::cerr << strerror(ret) << std::endl;
					break;
				}
			}

			// stdin reading; looks pretty stupid
			ret = aio_error(&stdin_readrq);
			if(ret == EINPROGRESS){
				stdin_readrq_inprogress = true;
			} else if(ret == 0) {
				stdin_readrq_inprogress = false;
			} else {
				std::cerr << strerror(ret) << std::endl;
				break;
			}

			if(serv_socket != CLOSED_SOCKET && socket_readrq_inprogress == false){
				int read;
				read = aio_return(&socket_readrq);
				if(read == 0){
					// connection is closed
					close(serv_socket);
					serv_socket = CLOSED_SOCKET;
				} else {
					recv_buf.push_back(buf, read);
				}
			}

			if(!stdin_readrq_inprogress){
				aio_return(&stdin_readrq);
				screen_full = false;
			}

			if(GET_sent == false && !socket_writerq_inprogress){
				int wrote = aio_return(&socket_writerq);
				request_offset += wrote;
				if(request_offset == request.length() - 1){
					GET_sent = true;
				}
			}


			if (screen_full == false && !recv_buf.is_empty()) {
				print_screen(recv_buf, screen_full, screen_rows_count, screen_cols_count);
			}

		}
	} catch(...){
		if(term_canon_on() == -1){
			std::cerr << strerror(errno) << std::endl;
		}
		return EXIT_FAILURE;
	}
}


