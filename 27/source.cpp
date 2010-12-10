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

#include "../libs/ChunkBuffer/Buffer.h"
#include "../libs/Fd_set/Fd_set.h"
#include "../libs/Terminal/terminal.h"

#define DEBUG
#undef DEBUG

class RuntimeException: public std::exception{
	private:
		std::string _err;
	public:
		RuntimeException(const char *m): _err(m){}
		RuntimeException(int error){
				char buf[256];
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE
				::strerror_r(error, buf, sizeof(buf));
				_err.assign(buf);
#else
				char *msg_ptr;
				msg_ptr = ::strerror_r(error, buf, sizeof (buf));
				_err.assign(msg_ptr);
#endif
		}

		const char *what() const throw() {
			return _err.c_str();
		}

		~RuntimeException() throw (){}
};


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
	const int STDIN_BUFSIZE = 128;
    const int HTTP_DEFAULT_PORT = 80;
    const int CLOSED_SOCKET = -1;
    const char msg_to_press_key[] = "\nPress space to scroll...";
    const int DEFAULT_TAB_WIDTH = 8;


    int rows, cols;
    if (get_terminal_width_height(STDOUT_FILENO, &cols, &rows) == -1) {
        perror("Terminal dimensions");
        return EXIT_FAILURE;
    }

    
	if (!isatty(STDIN_FILENO)) {
        perror("isatty");
        return EXIT_FAILURE;
    }

    if (term_save_state() == -1) {
        perror("Saving term state");
        return EXIT_FAILURE;
    }

    char *print_buf = new char[cols * rows];

    try {
        if (term_canon_off() == -1) {
			throw RuntimeException("Term canon off");
        }
#ifdef DEBUG
        std::clog << "Terminal size: " << rows << "x" << cols << std::endl;
#endif

        int serv_socket = init_tcp_socket();

        if (serv_socket == -1) {
			throw RuntimeException(errno);
        }

        std::string host, path, url(argv[1]);

        if (parse_arguments(url, host, path) == -1) {
			throw RuntimeException("Invalid input");
        }

        sockaddr_in remote_addr;

        if (init_remote_host_sockaddr(remote_addr, host.c_str(), htons(HTTP_DEFAULT_PORT)) == -1) {
			throw RuntimeException(hstrerror(h_errno));
        }

        if (connect(serv_socket, (const sockaddr*) & remote_addr, sizeof (remote_addr)) == -1) {
			throw RuntimeException(errno);
        }


        Buffer recv_Buf;
        std::string request = "GET " + path + " HTTP/1.0\r\nHost: " + host + "\r\n\r\n";
		int need_to_send = request.size();
		int sent = 0, sent_total = 0;
		while (need_to_send != 0){
			// sending request
			if (-1 == (sent = write(serv_socket, 
							request.c_str() + sent_total, need_to_send - sent_total))) {
				throw RuntimeException("write");
			} else {
				need_to_send -= sent;
				sent_total += sent;
			}
		}
		

        int next_tab_position;
        int cur_row = 0, cur_col = 0;
        char recv_buf[BUFSIZE];
        char stdin_read_buf[STDIN_BUFSIZE];
        int screens_to_print_count = 1;
        bool can_print = true;
        int ret;

        struct aiocb socket_readrq;
        struct aiocb stdin_readrq;
        struct aiocb stdout_writerq;

        bool socket_readrq_done = true;
        bool stdin_readrq_done = true;
        bool stdout_writerq_done = true;


        memset(&socket_readrq, 0, sizeof (socket_readrq));
        memset(&stdin_readrq, 0, sizeof (stdin_readrq));
        memset(&stdout_writerq, 0, sizeof (stdout_writerq));

        socket_readrq.aio_fildes = serv_socket;
        socket_readrq.aio_buf = recv_buf;
        socket_readrq.aio_nbytes = sizeof (recv_buf);

        stdout_writerq.aio_fildes = STDOUT_FILENO;
        stdout_writerq.aio_buf = print_buf;

        stdin_readrq.aio_fildes = STDIN_FILENO;
        stdin_readrq.aio_buf = stdin_read_buf;
        stdin_readrq.aio_nbytes = sizeof (stdin_read_buf);


        // socket_readrq -> 0; stdin_readrq -> 1; stdout_writerq -> 2
        struct aiocb * rq_list[3] = {&socket_readrq, &stdin_readrq, NULL};

        Chunk *chunk;
        int count_to_flush;
        bool stdout_write_requested = false;

        while(!((serv_socket == CLOSED_SOCKET) && 
				(recv_Buf.is_empty()))) {

            if (stdin_readrq_done) {
                stdin_readrq_done = false;
                if(aio_read(&stdin_readrq) == -1){
					throw RuntimeException("aio_read");
                }
            }

            if (socket_readrq_done && serv_socket != CLOSED_SOCKET) {
                socket_readrq_done = false;
                if(aio_read(&socket_readrq) == -1){
					throw RuntimeException("aio_read");
                }
            }

            if (stdout_writerq_done && !recv_Buf.is_empty() && can_print) {
                stdout_writerq_done = false;
                stdout_write_requested = true;
                chunk = recv_Buf.front();
                int chunk_size = chunk->size();
                const char *buf = chunk->buf();

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
                    
					count_to_flush = i + 1;

                    if (cur_row == rows) {
                        screens_to_print_count--;

                        if (screens_to_print_count == 0) {
                            can_print = false;
                        }


                        for (int j = 0; j < sizeof (msg_to_press_key) - 1; j++, i++) {
                            print_buf[i] = msg_to_press_key[j];
                        }

                        cur_col = cur_row = 0;
                        break;
                    }
                }

                stdout_writerq.aio_nbytes = i;
                if(aio_write(&stdout_writerq) == -1){
					throw RuntimeException("aio_write");
                }
                rq_list[2] = &stdout_writerq;
            }

#ifdef DEBUG
//			std::clog << "socket requested=" << socket_readrq_done << "\n"
//				"stdin requested=" << stdin_readrq_done << "\n" <<
//				"stdout requested=" << stdout_write_requested << std::endl;
#endif
            aio_suspend(rq_list, 3, NULL);

            if (aio_error(&stdin_readrq) == 0) {
                stdin_readrq_done = true;
                ret = aio_return(&stdin_readrq);
#ifdef DEBUG
				std::clog << "Read " << ret << " symbols from stdin" << std::endl;
#endif
                screens_to_print_count += ret == 0 ? 1 : ret;
                can_print = true;
            }

            if (stdout_write_requested && aio_error(&stdout_writerq) == 0) {
                stdout_writerq_done = true;
                stdout_write_requested = false;
                rq_list[2] = NULL;
                aio_return(&stdout_writerq);
                recv_Buf.pop_front();
#ifdef DEBUG 
				std::clog << "Returning " << count_to_flush << " symbols to buffer" << std::endl;
#endif
                recv_Buf.put_back_front(chunk, count_to_flush);
            }

            if (serv_socket != CLOSED_SOCKET && aio_error(&socket_readrq) == 0) {
                socket_readrq_done = true;
                ret = aio_return(&socket_readrq);
#ifdef DEBUG
				std::clog << "Read " << ret << " symbols from socket" << std::endl;
#endif
                if (ret != 0) {
                    recv_Buf.push_back(recv_buf, ret);
                } else {
                    close(serv_socket);
                    serv_socket = CLOSED_SOCKET;
                    rq_list[0] = NULL;
                }
            }

        }
    } catch (std::exception &ex) {
		fprintf(stderr, "%s", ex.what());
        if (term_restore_state() == -1) {
            perror("Term restore state");
        }
		delete[] print_buf;
        return EXIT_FAILURE;
    }

    if (term_restore_state() == -1) {
        perror("Term restore state");
    }

	delete[] print_buf;

    return EXIT_SUCCESS;
}
