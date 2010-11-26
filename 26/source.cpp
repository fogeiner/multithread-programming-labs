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

#include "../libs/HTTPURIParser/HTTPURIParser.h"
#include "../libs/TCPSocket/TCPSocket.h"
#include "../libs/Buffer/VectorBuffer.h"
#include "../libs/Fd_set/Fd_set.h"
#include "../libs/Terminal/Terminal.h"

#define DEBUG


void print_screen(Buffer *buf, bool &screen_full, int &print_screen_counter, int rows, int cols) {

	static const char msg_to_press_key[] = "Press enter to scroll...";
	static int cur_row = 0, cur_col = 0;
	int next_tab_position;
	const int DEFAULT_TAB_WIDTH = 8;

	while(!screen_full && !buf->is_empty()) {
		int size = buf->size();
		const char *b = buf->buf();

		int i;
		for (i = 0; i < size; ++i) {
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
				print_screen_counter--;
				if(print_screen_counter == 0){
					screen_full = true;
				}
				break;
			}
		}
		buf->drop_first(i);
	}
}

void get_terminal_props_and_save_state(int &screen_rows_count, int &screen_cols_count){
	if(!isatty(STDIN_FILENO)){
		std::cerr <<  "Standard input device is not a terminal" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (get_terminal_width_height(STDOUT_FILENO, &screen_cols_count, &screen_rows_count) == -1) {
		std::cerr << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	if(term_save_state() == -1){
		std::cerr << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " url" << std::endl;
		return EXIT_FAILURE;
	}

	int screen_rows_count, screen_cols_count;
	get_terminal_props_and_save_state(screen_rows_count, screen_cols_count);

	TCPSocket *serv_socket = NULL;
	Buffer *recv_buf = NULL;
	Buffer *send_buf = NULL;
	ParsedURI *pu = NULL;

	try{
		if(term_canon_off() == -1){
			std::cerr << strerror(errno) << std::endl;
			return EXIT_FAILURE;
		}

#ifdef DEBUG
		std::clog << "Terminal size: " << screen_rows_count << "x" << screen_cols_count << std::endl;
#endif

		serv_socket = new TCPSocket();
		recv_buf = new VectorBuffer();
		send_buf = new VectorBuffer();
		std::string url(argv[1]);
		ParsedURI *pu = HTTPURIParser::parse(url);

		if(pu == NULL){
			std::cerr << "Invalid URI provided" << std::endl;
			exit(EXIT_FAILURE);
		}

#ifdef DEBUG
		std::clog << "connecting to " << pu->netloc << " port " << ((pu->port_n != 0) ? pu->port_n : 80) << std::endl;
#endif
		serv_socket->connect(pu->netloc, pu->port_n != 0 ? pu->port_n : 80);

		send_buf->append("GET ");
		// HTTP/1.1 servers can do without Host; usually send Location with full URI
		send_buf->append(url.c_str());
		send_buf->append(" HTTP/1.0\r\n\r\n");

#ifdef DEBUG
		std::clog << "sending request (" << send_buf->size() << " bytes): " << "GET " << url << " HTTP/1.0" << std::endl;
#endif
		serv_socket->send(send_buf, send_buf->size(), true);

		const int STDIN_BUFSIZE = 128;
		bool screen_full = false;
		char stdin_buf[STDIN_BUFSIZE];

		Fd_set readfds;

		int print_screen_counter = 1;

		for (;;) {

			if ((serv_socket->is_closed()) && (recv_buf->size() == 0)) {
				break;
			}

			readfds.zero();

			if (!serv_socket->is_closed()) {
				readfds.set(serv_socket->fileno());
			}

			readfds.set(STDIN_FILENO);

			// in case no fds are set select will block
			int availible_fds = select(readfds.max_fd() + 1, &readfds.fdset(), NULL, NULL, NULL);

			if (availible_fds == -1) {
				std::cerr << strerror(errno) << std::endl;
				return EXIT_FAILURE;
			}

			if (!serv_socket->is_closed() && readfds.isset(serv_socket->fileno())) {
				serv_socket->recv(recv_buf);
			}

			if (readfds.isset(STDIN_FILENO)) {
				int read;
				read = ::read(STDIN_FILENO, stdin_buf, sizeof (stdin_buf));
				print_screen_counter += (read == 0) ? 1 : read;
				screen_full = false;
			}

			if (screen_full == false) {
				print_screen(recv_buf, screen_full, print_screen_counter, screen_rows_count, screen_cols_count);
			}
		}
	} catch(std::exception &ex){
		delete serv_socket;
		delete recv_buf;
		delete send_buf;
		delete pu;

		std::cerr << "Error: " << ex.what() << std::endl;

		if(term_restore_state() == -1){
			std::cerr << strerror(errno) << std::endl;
		}
		return EXIT_FAILURE;
	}
	delete serv_socket;
	delete recv_buf;
	delete send_buf;
	delete pu;
	if(term_restore_state() == -1){
		std::cerr << strerror(errno) << std::endl;
	}
}

