#include <iostream>
#include <exception>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sstream>
#include <sys/ioctl.h>
#include <termios.h>
#include <cstdlib>

int get_terminal_width_height(int fd, int *width, int *height);

class host_not_found_exception : public std::exception {
private:
    std::string cause;
public:

    host_not_found_exception(char *s) throw () : cause(s) {

    }

    const char *what() const throw () {
        return cause.c_str();
    }

    ~host_not_found_exception() throw () {
    }
};

class malformed_http_url_exception : public std::exception {
private:
    std::string cause;
public:

    malformed_http_url_exception(char *s) throw () : std::exception(), cause(s) {

    }

    const char *what() const throw () {
        return cause.c_str();
    }

    ~malformed_http_url_exception() throw () {
    }
};

class netconnection_exception : public std::exception {
private:
    std::string cause;
public:

    netconnection_exception(char *s) throw () : std::exception(), cause(s) {

    }

    const char *what() const throw () {
        return cause.c_str();
    }

    ~netconnection_exception() throw () {
    }
};

class http_connection {
private:
    std::string host;
    std::string path;
    int sock;
    u_int16_t port;
    const static int BUFSIZE = 1024;
public:

    http_connection(char *c_url, int port = 80) {
        this->port = port;

        std::string url(c_url);
        // starts with http://

        if (url.substr(0, 7) != "http://") {
            throw malformed_http_url_exception("No http resource identifier");
        } else {
            url = url.substr(7, url.length());
        }

        int slash_index = url.find_first_of('/', 0);

        if (slash_index != -1) {
            this->host = url.substr(0, slash_index);
            this->path = url.substr(slash_index, url.length());
        } else {
            this->host = url;
            path = '/';
        }

     //   std::cout << this->host << "\n" << this->path << std::endl;
    }

    void connect() {
        sockaddr_in remote_addr;

        this->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (this->sock == -1) throw netconnection_exception(strerror(errno));

        // in static memory, no need to call free
        struct hostent *remote_hostent = gethostbyname(host.c_str());

        if (remote_hostent == NULL) throw netconnection_exception("Remote host not found");

        if (remote_hostent->h_addr_list[0] == NULL) {
            throw host_not_found_exception("Remote host is not accessible");
        }

        remote_addr.sin_addr = *((in_addr *) remote_hostent->h_addr_list[0]);
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = htons(port);

        int ret_val;

        ret_val = ::connect(this->sock, (const sockaddr *) & remote_addr, sizeof (remote_addr));

        if (ret_val == -1) throw netconnection_exception(strerror(errno));
    }

    void process_request() {
        char get_request[] = "GET";
        char http_ver[] = "HTTP/1.0";
        char host_header[] = "Host:";

        std::stringstream sstr;

        sstr << get_request << " "
                << path << " "
                << http_ver << "\r\n"
                << host_header << " "
                << host << "\r\n"
                << "\n";

        //    std::cout << sstr.str() << std::endl;

        send(sock, sstr.str().c_str(), strlen(sstr.str().c_str()), 0);

        sstr.ignore();

        std::stringbuf buf;

        int recv_bytes;
        char sock_buf[BUFSIZE];

        int rows, cols;
        int cur_row = 0, cur_col = 0;

        if (get_terminal_width_height(1, &cols, &rows) == -1) {
            std::cerr << "get_terminal_width_height: " << strerror(errno) <<
                    "\nUsing default values for terminal width and height" << std::endl;
        }

        bool connection_is_closed = false;
        bool fist_screen = true;
        fd_set readfds;

        while (1) {
            if (connection_is_closed && (buf.in_avail() == 0)) {
                break;
            }

            FD_ZERO(&readfds);
            FD_SET(STDIN_FILENO, &readfds);

            if (!connection_is_closed)
                FD_SET(sock, &readfds);

            if (select(connection_is_closed ? STDIN_FILENO + 1 : sock + 1,
                    &readfds, NULL, NULL, NULL) == -1) {
            }

            if (!connection_is_closed && FD_ISSET(sock, &readfds)) {
                recv_bytes = recv(sock, sock_buf, sizeof (sock_buf), 0);

                // end of transmission
                if (recv_bytes == 0) {
                    connection_is_closed = true;
                }

                buf.sputn(sock_buf, recv_bytes);
            }

            if (fist_screen || FD_ISSET(STDIN_FILENO, &readfds)) {
                if(!fist_screen)
                    read(STDIN_FILENO, sock_buf, sizeof (sock_buf));
                fist_screen = false;
               
                while (true) {
                    char next_char;

                    next_char = (char) buf.sbumpc();

                    

                    if (next_char == '\t')
                        continue;

                    std::cout << next_char;

                    if (next_char == '\n') {
                        cur_row++;
                        cur_col = 0;
                    }

                    cur_col++;

                    if (cur_col == cols) {
                        cur_row++;
                        cur_col = 0;
                        std::cout << "\n";
                    }

                    if (cur_row == rows - 1) {
                        std::cout << "Press enter to scroll...";
                        std::cout.flush();
                        cur_row = 0;
                        break;
                    }

                    if (buf.in_avail() == 0){
                        std::cout << "Press enter to scroll...";
                        break;
                    }
                }

            }


        }

        close(sock);
    }
};

int get_terminal_width_height(int fd, int *width, int *height) {
    struct winsize win = {0, 0, 0, 0};
    int ret = ioctl(fd, TIOCGWINSZ, &win);

    if (height) {
        if (!win.ws_row) {
            char *s = getenv("LINES");
            if (s) win.ws_row = atoi(s);
        }
        if (win.ws_row <= 1 || win.ws_row >= 30000)
            win.ws_row = 24;
        *height = (int) win.ws_row;
    }

    if (width) {
        if (!win.ws_col) {
            char *s = getenv("COLUMNS");
            if (s) win.ws_col = atoi(s);
        }
        if (win.ws_col <= 1 || win.ws_col >= 30000)
            win.ws_col = 80;
        *width = (int) win.ws_col;
    }

    return ret;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " url" << std::endl;
        return 1;
    }

    try {
        http_connection hc(argv[1]);
        hc.connect();
        hc.process_request();
    } catch (std::exception &ex) {
        std::cerr << ex.what() << std::endl;
    }
}
