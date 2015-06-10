#include <string>
#include <cerrno>
#include <cstdio>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>
#include <signal.h>

#include <unistd.h>
#include <pthread.h>

#define DEBUG
#undef DEBUG

#if defined( sun ) || defined ( __sun )
#define MSG_NOSIGNAL 0
#endif

#include "../libs/ChunkBuffer/Buffer.h"
#include "../libs/Fd_set/Fd_set.h"
#include "../libs/Terminal/Terminal.h"


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

int GET_send_request(int &socket, std::string &url, std::string &path, std::string &host) {

	std::string request = "GET " + url + " HTTP/1.0\r\n\r\n";
#ifdef DEBUG
	fprintf(stdout, "Sending request: %s\n", request.c_str()) ;
#endif

	int need_to_send = request.length();
	int sent = 0, sent_total = 0;

	while(need_to_send != 0){
		sent = ::send(socket, request.c_str() + sent_total, need_to_send - sent_total, MSG_NOSIGNAL);
		if (sent == -1) {
			return -1;
		}
		sent_total += sent;
		need_to_send -= sent;
	}
	return 0;
}

void print_error(int err){
	const int BUFSIZE = 256;
	char b[BUFSIZE];
	const char *fmt_str = "Error: %s\n";
#ifdef __GNU
	char *msg_ptr;
	msg_ptr = ::strerror_r(err, b, sizeof(b));
	fprintf(stderr, fmt_str, msg_ptr);
#else
	::strerror_r(err, b, sizeof(b));
	fprintf(stderr, fmt_str, b); 
#endif
}

struct connection{
	const static int CLOSED_SOCKET = -1;
	int socket;
	pthread_mutex_t cm;
	pthread_cond_t cv;
	Buffer buf;

	connection(int sock) {
		this->socket = sock;
		pthread_mutex_init(&cm, NULL);
		pthread_cond_init(&cv, NULL);
	}

	~connection() {
		pthread_mutex_destroy(&cm);
		pthread_cond_destroy(&cv);
	}
};

extern "C"
void *recv_thread(void *conn_ptr){
	struct connection *con = static_cast<struct connection*>(conn_ptr);
	const int BUFSIZE = 4 * 1024;
	char b[BUFSIZE];

	int read;

	while(1) {
		read = ::recv(con->socket, b, sizeof(b), 0);

		pthread_mutex_lock(&con->cm);
		if(read == -1){
			print_error(errno);
			::close(con->socket);
			con->socket = connection::CLOSED_SOCKET;
#ifdef DEBUG
			fprintf(stdout, "Recv thread finished\n");
#endif
			pthread_cond_signal(&con->cv);
			pthread_mutex_unlock(&con->cm);
			pthread_exit(NULL);
		}

		if(read == 0){
			::close(con->socket);
			con->socket = connection::CLOSED_SOCKET;
#ifdef DEBUG
			fprintf(stdout, "Recv thread finished\n");
#endif
			pthread_cond_signal(&con->cv);
			pthread_mutex_unlock(&con->cm);
			pthread_exit(NULL);
		}

		con->buf.push_back(b, read);
		pthread_cond_signal(&con->cv);
		pthread_mutex_unlock(&con->cm);
	}
	return NULL;
}

extern "C"
void *print_thread(void *conn_ptr){

	struct connection *con = static_cast<struct connection*>(conn_ptr);

	int rows, cols;
	const int STDIN_BUFSIZE = 128;
	char stdin_buf[STDIN_BUFSIZE];
	int print_screen_counter = 1;

	if (get_terminal_width_height(STDOUT_FILENO, &cols, &rows) == -1) {
		print_error(errno);
	}

#ifdef DEBUG
	fprintf(stdout, "Terminal size: %dx%d\n", cols, rows);
#endif

	if(!isatty(STDIN_FILENO)){
		fprintf(stderr, "Standard input device is not a terminal\n");
		exit(EXIT_FAILURE);
	}

	if(term_save_state() == -1){
		print_error(errno);
		exit(EXIT_FAILURE);
	}
	try{
		if(term_canon_off() == -1){
			print_error(errno);
			exit(EXIT_FAILURE);
		}

		const char msg_to_press_key[] = "Press space to scroll...";
		bool can_print = true;
		int cur_row = 0, cur_col = 0;
		const int TAB_WIDTH = 8;


		while(1) {
			pthread_mutex_lock(&con->cm);

			while(con->buf.is_empty()){
				if(con->socket == connection::CLOSED_SOCKET){

					if(term_restore_state() == -1){
						print_error(errno);
					}

					pthread_mutex_unlock(&con->cm);
					pthread_exit(NULL);
				}
				pthread_cond_wait(&con->cv, &con->cm);
			}

			if(!can_print){
				// input from user
				int read;
				read = ::read(STDIN_FILENO, stdin_buf, sizeof (stdin_buf));
				print_screen_counter += (read == 0) ? 1 : read;
				can_print = true;
			} else {

				Chunk *chunk = con->buf.pop_front();

				int chunk_size = chunk->size();
				const char *b = chunk->buf();


				if(cur_row == -1){
					putchar('\n');
					cur_row = 0;
				}

				for (int i = 0; i < chunk_size; ++i) {
					int s = b[i];
					if(s == '\n'){
						cur_row++;
						cur_col = 0;
					} else if (s == '\t'){
						cur_col = TAB_WIDTH * ((cur_col + TAB_WIDTH)/TAB_WIDTH);
					} else if (isprint(s)){
						cur_col++;
					} else {
						continue;
					}

					putchar(b[i]);

					if (cur_col > cols - 1) {
						cur_col = 0;
						cur_row++;
						putchar('\n');
					}

					if (cur_row == rows - 1) {
						cur_row = -1;
						cur_col = 0;
						fputs(msg_to_press_key, stdout);
						fflush(stdout);
						print_screen_counter--;
						if(print_screen_counter == 0){
							can_print = false;
						}
						i++;
						con->buf.put_back_front(chunk, i);
						break;
					}
				}

				delete chunk;
			}

			pthread_mutex_unlock(&con->cm);
		}
	} catch(std::exception &ex){
		if(term_restore_state() == -1){
			print_error(errno);
		}
		exit(EXIT_FAILURE);
	}	

	return NULL;
}

int main(int argc, char *argv[]) {
	struct sigaction act;

	act.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &act, NULL);


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
		fprintf(stderr, "%s\n", "Invalid input");
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

	if (GET_send_request(serv_socket, url, path, host) == -1) {
		print_error(errno);
		return EXIT_FAILURE;
	}

	struct connection con(serv_socket);

	pthread_t recv_tid;
	int ret;

	if((ret = pthread_create(&recv_tid, NULL, &recv_thread, &con)) != 0){
		print_error(ret);
		return EXIT_FAILURE;
	}

	print_thread(&con);

	if((ret = pthread_join(recv_tid, NULL)) != 0){
		print_error(ret);
		return EXIT_FAILURE;
	}
}
