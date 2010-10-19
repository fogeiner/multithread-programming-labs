#include <iostream>
#include <string>
#include <cerrno>

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

int GET_send_request(int &socket, std::string &url, std::string &path, std::string &host) {

	std::string request = "GET " + path + " HTTP/1.0\r\nHost: " + host + "\r\n\r\n";
#ifdef DEBUG
	std::clog << "Sending request:\n" << request << std::endl;
#endif

	int send_size = request.length();
	int sent;

	sent = ::send(socket, request.c_str(), send_size, 0);

	if (sent != send_size) {
		return -1;
	}
}

int GET_recv_answer(int &socket, Buffer &recv_buf) {
	const int BUFSIZE = 4*1024;
	char b[BUFSIZE];
	int read;
	read = ::recv(socket, b, sizeof (b), 0);

	if (read > 0) {
		recv_buf.push_back(b, read);
	}

	return read;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " url" << std::endl;
		return EXIT_FAILURE;
	}

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

	// saving current state of the terminal
	struct termios saved_tty, changed_tty;
	if(tcgetattr(STDIN_FILENO, &saved_tty) == -1){
		std::cerr << strerror(errno) << std::endl;
		return EXIT_FAILURE;
	}

	try{

		// making changes
		changed_tty = saved_tty;
		changed_tty.c_lflag &= ~(ICANON);
		changed_tty.c_cc[VMIN] = 1;
		changed_tty.c_cc[VTIME] = 0;

		// applying it to a device
		if(tcsetattr(STDIN_FILENO, TCSANOW, &changed_tty) == -1){
			std::cerr << strerror(errno) << std::endl;
			return EXIT_FAILURE;
		}


#ifdef DEBUG
		std::clog << "Terminal size: " << screen_rows_count << "x" << screen_cols_count << std::endl;
#endif

		int serv_socket = init_tcp_socket();

		if (serv_socket == -1) {
			std::cerr << strerror(errno) << std::endl;
			return EXIT_FAILURE;
		}

		Buffer recv_buf;
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

		if (GET_send_request(serv_socket, url, path, host) == -1) {
			std::cerr << strerror(errno) << std::endl;
			return EXIT_FAILURE;
		}

		bool screen_full = false;
		Fd_set readfds;

		for (;;) {

			if ((serv_socket == CLOSED_SOCKET) && (recv_buf.size() == 0)) {
				break;
			}

			readfds.zero();

			if (serv_socket != CLOSED_SOCKET) {
				readfds.set(serv_socket);
			}

			readfds.set(STDIN_FILENO);

			int availible_fds = select(readfds.max_fd() + 1, &readfds.fdset(), NULL, NULL, NULL);

			if (availible_fds == -1) {
				std::cerr << strerror(errno) << std::endl;
				return EXIT_FAILURE;
			}

			if (serv_socket != CLOSED_SOCKET && readfds.isset(serv_socket)) {
				int ret = GET_recv_answer(serv_socket, recv_buf);
				if (ret == -1) {
					std::cerr << strerror(errno) << std::endl;
					return EXIT_FAILURE;
				}

				if (ret == 0) {
					close(serv_socket);
					serv_socket = CLOSED_SOCKET;
				}
			}

			if (screen_full == true && readfds.isset(STDIN_FILENO)) {
				char b;
				::read(STDIN_FILENO, &b, sizeof (b));
				screen_full = false;
			}

			if (screen_full == false) {
				print_screen(recv_buf, screen_full, screen_rows_count, screen_cols_count);
			}
		}
	} catch(...){
		if(tcsetattr(STDIN_FILENO, TCSANOW, &saved_tty) == -1){
			std::cerr << strerror(errno) << std::endl;
			return EXIT_FAILURE;
		}
	}
}

