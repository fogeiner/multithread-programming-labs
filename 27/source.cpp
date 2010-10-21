#include <string>
#include <cerrno>
#include <cstdio>
#include <iostream>

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
		fprintf(stderr, "URL required\n");
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

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s url\n", argv[0]);
		return EXIT_FAILURE;
	}

	const int BUFSIZE = 1024;
	const int HTTP_DEFAULT_PORT = 80;
	const int CLOSED_SOCKET = -1;
	const char msg_to_press_key[] = "Press space to scroll...\n";
	const int DEFAULT_TAB_WIDTH = 8;


	int rows, cols;
	if (get_terminal_width_height(STDOUT_FILENO, &cols, &rows) == -1) {
		perror("Terminal dimensions");
	}

	if (!isatty(STDIN_FILENO)) {
		perror("isatty");
		return EXIT_FAILURE;
	}

	if (term_save_state() == -1) {
		perror("Saving term state");
		return EXIT_FAILURE;
	}
	try {
		if(term_canon_off() == -1){
			perror("Term canon off");
			return EXIT_FAILURE;
		}
#ifdef DEBUG
		std::clog << "Terminal size: " << rows << "x" << cols << std::endl;
#endif

		int serv_socket = init_tcp_socket();

		if (serv_socket == -1) {
			perror("socket: ");
			return EXIT_FAILURE;
		}

		std::string host, path, url(argv[1]);

		if (parse_arguments(url, host, path) == -1) {
			return EXIT_FAILURE;
		}

		sockaddr_in remote_addr;

		if (init_remote_host_sockaddr(remote_addr, host.c_str(), htons(HTTP_DEFAULT_PORT)) == -1) {
			fprintf(stderr, "Getting remote_host info: %s\n", hstrerror(h_errno));
			return EXIT_FAILURE;
		}

		if (connect(serv_socket, (const sockaddr*) & remote_addr, sizeof (remote_addr)) == -1) {
			perror("connect");
			return EXIT_FAILURE;
		}


		Buffer recv_Buf;
		std::string request = "GET " + path + " HTTP/1.0\r\nHost: " + host + "\r\n\r\n";

		// sending request
		if (-1 == write(serv_socket, request.c_str(), request.length())) {
			perror("write");
			return EXIT_FAILURE;
		}

		int next_tab_position;
		int cur_row = 0, cur_col = 0;

		struct aiocb socket_readrq;
		struct aiocb stdin_readrq;
		struct aiocb stdout_writerq;
		
//		bool socket_readrq_inprogress = false;
//		bool stdin_readrq_inprogress = false;
//		bool stdout_writerq_inprogress = false;

		bool socket_readrq_done = true;
		bool stdin_readrq_done = true;
		bool stdout_writerq_done = true;

		memset(&stdout_writerq, 0, sizeof(stdout_writerq));
		memset(&stdin_readrq, 0, sizeof(stdin_readrq));
		memset(&socket_readrq, 0, sizeof(socket_readrq));

		struct aiocb * rq_list[3] = {NULL, NULL, NULL};

		int ret;

		char symbol;
		char recv_buf[BUFSIZE];
		char print_buf[BUFSIZE];
		bool screen_full = false;
		bool stdout_writerq_active = false;
		Chunk *chunk = NULL;

		for (;;) {
			// all is read, all is shown
			if ((serv_socket == CLOSED_SOCKET) && (recv_Buf.is_empty())) {
				break;
			}

			int rq_list_index = 0;
			bzero(rq_list, sizeof (rq_list));

			// initing requests
			if (!stdin_readrq_inprogress) {
				stdin_readrq.aio_fildes = STDIN_FILENO;
				stdin_readrq.aio_buf = &symbol;
				stdin_readrq.aio_nbytes = sizeof (symbol);
				//	stdin_readrq.aio_lio_opcode = LIO_READ;

				if (aio_read(&stdin_readrq) == -1) {
					perror("aio_read");
					break;
				}
			}
			rq_list[rq_list_index++] = &stdin_readrq;

			if (!screen_full && !recv_Buf.is_empty() && !stdout_writerq_inprogress) {
				// forming new print_buf

				chunk = recv_Buf.pop_front();

				const char *buf = chunk->buf();
				const int chunk_size = chunk->size();
				stdout_writerq_active = true;
				int i;
				for (i = 0; i < chunk_size; ++i) {
					switch (buf[i]) {
						case '\t':
							next_tab_position = DEFAULT_TAB_WIDTH *
								((cur_col + DEFAULT_TAB_WIDTH) / DEFAULT_TAB_WIDTH);
							if (next_tab_position <= cols) {
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

					print_buf[i] = buf[i];

					if (cur_col == cols) {
						cur_col = 0;
						cur_row++;
					}

					if (cur_row == rows - 2) {
						screen_full = true;
						cur_col = cur_row = 0;
						break;
					}
				}

				stdout_writerq.aio_fildes = STDOUT_FILENO;
				stdout_writerq.aio_buf = print_buf;
				stdout_writerq.aio_nbytes = i == chunk_size ? chunk_size : i + 1;
				//stdout_writerq.aio_lio_opcode = LIO_WRITE;

				rq_list[rq_list_index++] = &stdout_writerq;
				if (aio_write(&stdout_writerq) == -1) {
					perror("aio_write");
					break;
				}
			} else if (stdout_writerq_inprogress) {
				rq_list[rq_list_index++] = &stdout_writerq;
			}

			if ((serv_socket != CLOSED_SOCKET) && (!socket_readrq_inprogress)) {
				socket_readrq.aio_fildes = serv_socket;
				socket_readrq.aio_buf = recv_buf;
				socket_readrq.aio_nbytes = sizeof (recv_buf);
				//socket_readrq.aio_lio_opcode = LIO_READ;
				rq_list[rq_list_index++] = &socket_readrq;

				if (aio_read(&socket_readrq) == -1) {
					perror("aio_read");
					break;
				}
			} else if (socket_readrq_inprogress) {
				rq_list[rq_list_index++] = &socket_readrq;
			}

			aio_suspend(rq_list, rq_list_index, NULL);

			// stdin reading
			ret = aio_error(&stdin_readrq);
			if (ret == EINPROGRESS) {
				stdin_readrq_inprogress = true;
			} else if (ret == 0) {
				stdin_readrq_inprogress = false;
				aio_return(&stdin_readrq);
				screen_full = false;
			} else {
				perror("aio_error");
				break;
			}

			// reply receiving
			if (serv_socket != CLOSED_SOCKET) {
				ret = aio_error(&socket_readrq);
				if (ret == EINPROGRESS) {
					socket_readrq_inprogress = true;
				} else if (ret == 0) {
					socket_readrq_inprogress = false;
					int read;
					read = aio_return(&socket_readrq);
					if (read == 0) {
						// transmission end
						close(serv_socket);
						serv_socket = CLOSED_SOCKET;
					} else {
						recv_Buf.push_back(recv_buf, read);
					}
				} else {
					perror("aio_error");
					break;
				}
			}


			if (stdout_writerq_active) {
				ret = aio_error(&stdout_writerq);
				if (ret == EINPROGRESS) {
					stdout_writerq_inprogress = true;
				} else if (ret == 0) {
					stdout_writerq_active = false;
					stdout_writerq_inprogress = false;
					int wrote;
					wrote = aio_return(&stdout_writerq);
					if(screen_full){
						std::cout << msg_to_press_key;
						std::cout.flush();
					}
					recv_Buf.put_back_front(chunk, wrote);
				} else {
					perror("aio_error");
					break;
				}
			}
		}
	} catch (...) {
		if (term_restore_state() == -1) {
			perror("Term restore state");
		}
		return EXIT_FAILURE;
	}

	if (term_restore_state() == -1) {
		perror("Term restore state");
	}

	return EXIT_SUCCESS;
}
