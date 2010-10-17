#include <string>
#include <cerrno>
#include <cstdio>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>

#include <unistd.h>
#include <pthread.h>

#define DEBUG

#include "../libs/Buffer/Buffer.h"
#include "../libs/Fd_set/Fd_set.h"
#include "../libs/Terminal/terminal.h"


int init_tcp_socket() {
	int socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return socket;
}

int init_remote_host_sockaddr(sockaddr_in &remote_addr, const char *remote_host, u_int16_t remote_port) {
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
		fprintf(stderr, "URL required");
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
	fprintf(stdout, "Host: %s \tPath: %s\n", host.c_str(), path.c_str());
#endif
}

int GET_send_request(int &socket, std::string &url, std::string &host) {

	std::string request = "GET " + url + " HTTP/1.0\r\nHost: " + host + "\r\n\r\n";
#ifdef DEBUG
	fprintf(stdout, "Sending request: %s\n", request.c_str()) ;
#endif

	int send_size = request.length();
	int sent;

	sent = ::send(socket, request.c_str(), send_size, 0);

	if (sent != send_size) {
		return -1;
	}
}

void print_error(int err){
	const int BUFSIZE = 256;
	char b[BUFSIZE];
	strerror_r(err, b, sizeof(b));
	fprintf(stderr, "Error: %s\n", b);
}

struct connection{
	const static int CLOSED_SOCKET = -1;
	int socket;
	pthread_mutex_t cm;
	pthread_cond_t cv;
	Buffer buf;

	connection(int socket){
		this->socket = socket;
		pthread_mutex_init(&cm, NULL);
		pthread_cond_init(&cv, NULL);
	}

	~connection(){
		pthread_mutex_destroy(&cm);
		pthread_cond_destroy(&cv);
	}
};


void *recv_thread(void *conn_ptr){
	struct connection *con = static_cast<struct connection*>(conn_ptr);
	const int BUFSIZE = 4 * 1024;
	char b[BUFSIZE];

	int read;

	for(;;){
		read = ::recv(con->socket, b, sizeof(b), 0);
		
		if(read == -1){
			print_error(errno);
			::close(con->socket);
			con->socket = connection::CLOSED_SOCKET;
#ifdef DEBUG
			fprintf(stdout, "Recv thread finished");
#endif
			pthread_exit(NULL);
		}

		if(read == 0){
			::close(con->socket);
			con->socket = connection::CLOSED_SOCKET;
#ifdef DEBUG
			fprintf(stdout, "Recv thread finished");
#endif
			pthread_exit(NULL);
		}

		con->buf.push_back(b, read);

		pthread_mutex_lock(&con->cm);
		pthread_cond_signal(&con->cv);
		pthread_mutex_unlock(&con->cm);
	}
}

void *print_thread(void *conn_ptr){
	struct connection *con = static_cast<struct connection*>(conn_ptr);

	const int DEFAULT_SCREEN_WIDTH = 24;
	const int DEFAUL_SCREEN_HEIGHT = 80;
	int rows, cols;

	if (get_terminal_width_height(STDOUT_FILENO, &cols, &rows) == -1) {
		rows = DEFAUL_SCREEN_HEIGHT;
		cols = DEFAULT_SCREEN_WIDTH;
	}

#ifdef DEBUG
	fprintf(stdout, "Terminal size: %dx%d\n", cols, rows);
#endif

	const char msg_to_press_key[] = "Press enter to scroll...";
	bool can_print = true;
	int cur_row = 0, cur_col = 0;

	for (;;) {
		pthread_mutex_lock(&con->cm);

		while(con->buf.is_empty()){
			if(con->socket == connection::CLOSED_SOCKET){
				pthread_exit(NULL);
			}
			pthread_cond_wait(&con->cv, &con->cm);
		}

		if(!can_print){
			// input from user
			const int BUFSIZE = 512;
			char b[BUFSIZE];
			::read(STDIN_FILENO, b, sizeof (b));
			can_print = true;
		} else {

			Chunk *chunk = con->buf.pop_front();

			int chunk_size = chunk->size();
			const char *b = chunk->buf();

			for (int i = 0; i < chunk_size; ++i) {
				switch (b[i]) {
					case '\n':
						cur_row++;
						cur_col = 0;
						break;
					default:
						cur_col++;
				}

				putchar(b[i]);

				if (cur_col == cols) {
					cur_col = 0;
					cur_row++;
				}

				if (cur_row == rows - 1) {
					cur_row = cur_col = 0;
					puts(msg_to_press_key);
					fflush(stdout);
					can_print = false;
					con->buf.put_back_front(chunk, i);
					break;
				}
			}

			delete chunk;
		}

		pthread_mutex_unlock(&con->cm);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr,  "Usage: %s  url\n", argv[0]);
		return EXIT_FAILURE;
	}

	const int HTTP_DEFAULT_PORT = 80;
	const int BUFSIZE = 512;


	int serv_socket = init_tcp_socket();

	if (serv_socket == -1) {
		print_error(errno);
		return EXIT_FAILURE;
	}

	std::string host, path, url(argv[1]);

	if (parse_arguments(url, host, path) == -1) {
		return EXIT_FAILURE;
	}

	sockaddr_in remote_addr;

	if (init_remote_host_sockaddr(remote_addr, host.c_str(), htons(HTTP_DEFAULT_PORT)) == -1) {
		fprintf(stderr, "Error: %s\n", hstrerror(h_errno));
		return EXIT_FAILURE;
	}

	if (connect(serv_socket, (const sockaddr*) & remote_addr, sizeof (remote_addr)) == -1) {
		print_error(errno);
		return EXIT_FAILURE;
	}

	if (GET_send_request(serv_socket, url, host) == -1) {
		print_error(errno);
		return EXIT_FAILURE;
	}

	struct connection con(serv_socket);

	pthread_t recv_tid;
	int ret;

	if((ret = pthread_create(&recv_tid, NULL, recv_thread, &con)) != 0){
		print_error(ret);
		return EXIT_FAILURE;
	}

	print_thread(&con);

	if((ret = pthread_join(recv_tid, NULL)) != 0){
		print_error(errno);
		return NULL;
	}

}
