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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " url" << std::endl;
        return EXIT_FAILURE;
    }

    const int BUFSIZE = 1024;
    const int HTTP_DEFAULT_PORT = 80;
    const int CLOSED_SOCKET = -1;
    const char msg_to_press_key[] = "Press enter to scroll...";
    const int DEFAULT_TAB_WIDTH = 8;


    int rows, cols;
    if (get_terminal_width_height(STDOUT_FILENO, &cols, &rows) == -1) {
        std::cerr << strerror(errno) << std::endl;
    }

    if (!isatty(STDIN_FILENO)) {
        std::cerr << "Standard input device is not a terminal" << std::endl;
        return EXIT_FAILURE;
    }

    if (term_canon_off() == -1) {
        std::cerr << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }
    try {
#ifdef DEBUG
        std::clog << "Terminal size: " << rows << "x" << cols << std::endl;
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


        Buffer recv_Buf;
        std::string request = "GET " + path + " HTTP/1.0\r\nHost: " + host + "\r\n\r\n";

        // sending request
        if (-1 == write(serv_socket, request.c_str(), request.length())) {
            std::cerr << strerror(errno) << std::endl;
            return EXIT_FAILURE;
        }

        int next_tab_position;
        int cur_row = 0, cur_col = 0;

        struct aiocb socket_readrq;
        bool socket_readrq_inprogress = false;

        struct aiocb stdin_readrq;
        bool stdin_readrq_inprogress = false;

        struct aiocb stdout_writerq;
        bool stdout_writerq_inprogress = false;

        bzero(&stdout_writerq, sizeof (stdout_writerq));
        bzero(&socket_readrq, sizeof (socket_readrq));
        bzero(&stdin_readrq, sizeof (stdin_readrq));

        struct aiocb * rq_list[3] = {NULL, NULL, NULL};

        int ret;

        char symbol;
        char recv_buf[BUFSIZE];
        char print_buf[BUFSIZE];
        bool screen_full = false;

        for (;;) {
            // all is read, all is shown
            if ((serv_socket == CLOSED_SOCKET) && (recv_Buf.is_empty())) {
                break;
            }

            int rq_list_index = 0;
            Chunk *chunk;
            bzero(rq_list, sizeof (rq_list));

            // initing requests
            if (!stdin_readrq_inprogress) {
                stdin_readrq.aio_fildes = STDIN_FILENO;
                stdin_readrq.aio_buf = &symbol;
                stdin_readrq.aio_nbytes = sizeof (symbol);
                //	stdin_readrq.aio_lio_opcode = LIO_READ;

                if (aio_read(&stdin_readrq) == -1) {
                    std::cerr << strerror(errno) << std::endl;
                    return EXIT_FAILURE;
                }
            }
            rq_list[rq_list_index++] = &stdin_readrq;

            if (!screen_full && !recv_Buf.is_empty() && !stdout_writerq_inprogress) {
                // forming new print_buf
                int cur_col_v = cur_col;
                int cur_row_v = cur_row;

                chunk = recv_Buf.pop_front();
                const char *buf = chunk->buf();
                const int chunk_size = chunk->size();

                int i;
                for (i = 0; i < chunk_size; ++i) {
                    switch (buf[i]) {
                        case '\t':
                            next_tab_position = DEFAULT_TAB_WIDTH *
                                    ((cur_col_v + DEFAULT_TAB_WIDTH) / DEFAULT_TAB_WIDTH);
                            if (next_tab_position <= cols) {
                                cur_col_v += next_tab_position;
                                break;
                            }
                        case '\n':
                            cur_row_v++;
                            cur_col_v = 0;
                            break;
                        default:
                            cur_col_v++;
                    }

                    print_buf[i] = buf[i];

                    if (cur_col_v == cols) {
                        cur_col_v = 0;
                        cur_row_v++;
                    }

                    if (cur_row_v == rows) {
                        break;
                    }
                }

                stdout_writerq.aio_fildes = STDOUT_FILENO;
                stdout_writerq.aio_buf = print_buf;
                stdout_writerq.aio_nbytes = i;
                //stdout_writerq.aio_lio_opcode = LIO_WRITE;

                rq_list[rq_list_index++] = &stdout_writerq;
                if (aio_write(&stdout_writerq) == -1) {
                    std::cerr << strerror(errno) << std::endl;
                    return EXIT_FAILURE;
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
                    std::cerr << strerror(errno) << std::endl;
                    return EXIT_FAILURE;
                }
            } else if (socket_readrq_inprogress) {
                rq_list[rq_list_index++] = &socket_readrq;
            }

            aio_suspend(rq_list, rq_list_index, NULL);

            // testing results

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
                    std::cerr << strerror(ret) << std::endl;
                    break;
                }
            }

            if (!screen_full && !recv_Buf.is_empty()) {
                ret = aio_error(&stdout_writerq);
                if (ret == EINPROGRESS) {
                    stdout_writerq_inprogress = true;
                } else if (ret == 0) {
                    stdout_writerq_inprogress = false;
                    int wrote;
                    wrote = aio_return(&stdout_writerq);

                    for (int i = 0; i < wrote; ++i) {
                        switch (print_buf[i]) {
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

                        std::cout << print_buf[i];

                        if (cur_col == cols) {
                            cur_col = 0;
                            cur_row++;
                        }

                        if (cur_row == rows - 1) {
                            cur_row = cur_col = 0;
                            std::cout << msg_to_press_key;
                            std::cout.flush();
                            screen_full = true;
                            recv_Buf.put_back_front(chunk, wrote);
                        }
                    }

                    // stdin reading; looks pretty stupid
                    ret = aio_error(&stdin_readrq);
                    if (ret == EINPROGRESS) {
                        stdin_readrq_inprogress = true;
                    } else if (ret == 0) {
                        stdin_readrq_inprogress = false;
                        aio_return(&stdin_readrq);
                        screen_full = false;
                    } else {
                        std::cerr << strerror(ret) << std::endl;
                        break;
                    }

                }
            }
        }
    } catch (...) {
        if (term_canon_on() == -1) {
            std::cerr << strerror(errno) << std::endl;
        }
        return EXIT_FAILURE;
    }
}
