#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <list>

using namespace std;

void die_with_error(const char *s) {
    perror(s);
    exit(EXIT_FAILURE);
}

void usage(const char *s) {
    fprintf(stdout, "Usage: %s port_to_listen "
            "remote_hostname|remote_ip remote_port\n", s);
}

void store_max_value(int *current_max, int new_value1, int new_value2) {
    int bigger_value = (new_value1 > new_value2) ? new_value1 : new_value2;
    if (*current_max < bigger_value) {
        *current_max = bigger_value;
    }
}

const int BUFSIZE = 16 * 1024;
const int default_tries_to_send = 5;

struct buf_chunk {
    char buf[BUFSIZE];
    int buf_count;
};

struct proxy_connection {
    int client_sock;
    int server_sock;
    list<struct buf_chunk *> ctos_buffer;
    list<struct buf_chunk *> stoc_buffer;
};

void close_client_connection(proxy_connection *p) {
    close(p->client_sock);

    for (list<buf_chunk*>::iterator i = p->stoc_buffer.begin();
            i != p->stoc_buffer.end(); ++i) {
        delete *i;
    }

    p->stoc_buffer.clear();
    p->client_sock = -1;
}

void close_server_connection(proxy_connection *p) {
    close(p->server_sock);

    for (list<buf_chunk*>::iterator i = p->ctos_buffer.begin();
            i != p->ctos_buffer.end(); ++i) {
        delete *i;
    }

    p->ctos_buffer.clear();
    p->server_sock = -1;
}

void fill_local_addr_structure(sockaddr_in *local_addr) {

}

void fill_remote_addr_structure(sockaddr_in *remote_addr) {

}

int main2(int argc, char *argv[]) {

    if (argc != 4) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    u_int16_t local_port = atoi(argv[1]);
    char *remote_addess = argv[2];
    u_int16_t remote_port = atoi(argv[3]);

    int ret_val;
    int list_sock;
    int backlog = 5;

    list_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (list_sock == -1) die_with_error("socket()");

    struct sockaddr_in local_addr;

    {
        local_addr.sin_family = AF_INET;
        local_addr.sin_port = htons(local_port);
        local_addr.sin_addr.s_addr = INADDR_ANY;
    }

    struct sockaddr_in remote_addr;

    {
        // in static memory, no need to call free
        struct hostent *remote_hostent = gethostbyname(remote_addess);

        if (remote_hostent == NULL) {
            herror("gethostbyname()");
            exit(EXIT_FAILURE);
        }

        if (remote_hostent->h_addr_list[0] == NULL) {
            fprintf(stderr, "%s\n", "Can't get location of the remote host");
        }

        remote_addr.sin_addr = *((in_addr *) remote_hostent->h_addr_list[0]);
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = htons(remote_port);
    }

    ret_val = bind(list_sock, (const sockaddr *) & local_addr, sizeof (local_addr));
    if (ret_val == -1) die_with_error("bind()");

    ret_val = listen(list_sock, backlog);
    if (ret_val == -1) die_with_error("listen()");

    fd_set readfds, writefds;
    list<struct proxy_connection*> proxy_connections;

    for (;;) {
        // adding sockets to a list:
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);

        FD_SET(list_sock, &readfds);

        int max_fd_num = list_sock;

        for (list<struct proxy_connection*>::iterator i = proxy_connections.begin();
                i != proxy_connections.end(); ++i) {

            struct proxy_connection *p = *i;


            if ((p-> client_sock == -1) && (p->server_sock == -1)) {
                delete p;
                i = proxy_connections.erase(i);
                continue;
            }

            if (p->client_sock != -1) {
                FD_SET(p->client_sock, &readfds);

                if (!p->stoc_buffer.empty())
                    FD_SET(p->client_sock, &writefds);
            }

            if (p->server_sock != -1) {
                FD_SET(p->server_sock, &readfds);

                if (!p->ctos_buffer.empty())
                    FD_SET(p->server_sock, &writefds);
            }

            store_max_value(&max_fd_num, p->client_sock, p->server_sock);
        }

        ret_val = select(max_fd_num + 1, &readfds, &writefds, NULL, NULL);
        if (ret_val == -1) die_with_error("select()");


        // new connection from a client
        if (FD_ISSET(list_sock, &readfds)) {
            int client_sock = accept(list_sock, NULL, NULL);
            if (client_sock == -1) die_with_error("accept()");

            int server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (server_sock == -1) die_with_error("socket()");

            ret_val = connect(server_sock, (const sockaddr *) & remote_addr, sizeof (remote_addr));
            if (ret_val == -1) die_with_error("connect()");

            struct proxy_connection *pc = new proxy_connection();

            pc->client_sock = client_sock;
            pc->server_sock = server_sock;
            proxy_connections.push_front(pc);
        }

        for (list<struct proxy_connection*>::iterator i = proxy_connections.begin();
                i != proxy_connections.end(); ++i) {

            struct proxy_connection *p = *i;

            if (FD_ISSET(p->client_sock, &readfds)) {
                buf_chunk *bc = new buf_chunk();

                ret_val = recv(p->client_sock, bc->buf, sizeof (bc->buf), 0);

                // client closed connection
                do {
                    if (ret_val == 0) {
                        close_client_connection(p);
                        delete bc;
                        break;
                    }

                    bc->buf_count = ret_val;
                    p->ctos_buffer.push_back(bc);

                } while (0);
            }

            if (FD_ISSET(p->server_sock, &readfds)) {
                buf_chunk *bc = new buf_chunk();

                ret_val = recv(p->server_sock, bc->buf, sizeof (bc->buf), 0);

                // server closed connection
                do {
                    if (ret_val == 0) {
                        close_server_connection(p);
                        delete bc;
                        break;
                    }

                    bc->buf_count = ret_val;
                    p->stoc_buffer.push_back(bc);
                } while (0);
            }

            if (FD_ISSET(p->client_sock, &writefds)) {

                buf_chunk *bc = p->stoc_buffer.front();
                p->stoc_buffer.pop_front();
                ret_val = send(p->client_sock, bc->buf, bc->buf_count, 0);

                if (ret_val != bc->buf_count) {
                    buf_chunk *new_bc = new buf_chunk();
                    new_bc->buf_count = bc->buf_count - ret_val;
                    for (int i = ret_val; i < bc->buf_count; ++i) {
                        new_bc->buf[i - ret_val] = bc->buf[i];
                    }
                    p->stoc_buffer.push_front(new_bc);
                }

                delete bc;

                if (p->server_sock == -1) {
                    close_client_connection(p);
                }

            }

            if (FD_ISSET(p->server_sock, &writefds)) {

                buf_chunk *bc = p->ctos_buffer.front();
                p->ctos_buffer.pop_front();

                ret_val = send(p->server_sock, bc->buf, bc->buf_count, 0);

                if (ret_val != bc->buf_count) {
                    buf_chunk *new_bc = new buf_chunk();
                    new_bc->buf_count = bc->buf_count - ret_val;
                    for (int i = ret_val; i < bc->buf_count; ++i) {
                        new_bc->buf[i - ret_val] = bc->buf[i];
                    }

                    p->ctos_buffer.push_front(new_bc);
                }

                delete bc;

                if (p->client_sock == -1) {
                    close_server_connection(p);
                }
            }
        }
    }
}
