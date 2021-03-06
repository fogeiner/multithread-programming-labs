#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <iostream>

#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <list>

#define DEBUG
//#undef DEBUG

#include "../libs/Fd_set/Fd_set.h"
#include "Forwarded_connection/Forwarded_connection.h"


inline int max(int v1, int v2) {
	return v1 > v2 ? v1 : v2;
}

int parse_arguments(int argc, char *argv[], unsigned short &local_port, unsigned short &remote_port, char *&remote_host) {
	const int NUM_OF_EXPECTED_ARGUMENTS = 4;
	if (argc != NUM_OF_EXPECTED_ARGUMENTS) {
		return -1;
	}

	local_port = htons(atoi(argv[1]));
	remote_port = htons(atoi(argv[3]));
	remote_host = argv[2];
#ifdef DEBUG
	std::clog << "local port in n.rep.: " << local_port << "\n"
		<< "remote port in n.rep.: " << remote_port << "\n"
		<< "remote host: " << remote_host << std::endl;
#endif
	return 0;
}

int init_tcp_socket() {
	int socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return socket;
}

void init_local_sockaddr(sockaddr_in &local_addr, unsigned short local_port) {
	bzero(&local_addr, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = local_port;
	local_addr.sin_addr.s_addr = INADDR_ANY;
}

int init_remote_host_sockaddr(sockaddr_in &remote_addr, char *remote_host, unsigned short remote_port) {
	// in static memory, no need to call free
	struct hostent *remote_hostent = gethostbyname(remote_host);

	if (remote_hostent == NULL) {
		return -1;
	}

	bzero(&remote_addr, sizeof(remote_addr));
	remote_addr.sin_addr = *((in_addr *) remote_hostent->h_addr_list[0]);
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = remote_port;
	return 0;
}

// add sockets to FD_SETs depending on what we can do with them
void init_fds(Fd_set &readfds, Fd_set &writefds, int listening_socket, std::list<Forwarded_connection*> &connections) {
#ifdef DEBUG
	std::clog << "Adding fds to FD_SET: " << std::endl;
#endif

	// deleting all descriptors from previous operations
	readfds.zero();
	writefds.zero();

#ifdef DEBUG
	std::clog << "read listening socket: " << listening_socket << "\n";
#endif
	// adding listening socket; always
	readfds.set(listening_socket);

#ifdef DEBUG
	std::clog << "Forwarded connections number: " << connections.size() << std::endl;
#endif
	for (std::list<Forwarded_connection*>::iterator i = connections.begin();
			i != connections.end(); ++i) {
		Forwarded_connection *fc = *i;
#ifdef DEBUG
		std::clog << "Processing forwarded connection: " << fc
			<< " server socket: " << fc->server_socket() << " client socket " << fc->client_socket()
			<< std::endl;
#endif

		// if both sockets are closed then we don't need such Forwarded_connection any more
		if ((fc->client_socket() == Forwarded_connection::CLOSED_SOCKET) &&
				(fc->server_socket() == Forwarded_connection::CLOSED_SOCKET)) {
#ifdef DEBUG
			std::clog << "deleting forwarded connection" << std::endl;
#endif
			delete fc;
			i = connections.erase(i);
			i--;
			continue;
		}

		if (fc->client_socket() != Forwarded_connection::CLOSED_SOCKET) {
			// we are willing to read from not closed sockets
			readfds.set(fc->client_socket());
#ifdef DEBUG
			std::clog << "read: " << fc->client_socket() << " ";
#endif
			if ((0 != fc->server_to_client_msgs_count()) || (fc->server_socket() == Forwarded_connection::CLOSED_SOCKET)) {
#ifdef DEBUG
				std::clog << "write: " << fc->client_socket() << " ";
#endif
				// and if we also have something to write to them we shall check write ability
				writefds.set(fc->client_socket());
			}
		}

		// all the same if true for the server-side of connection
		if (fc->server_socket() != Forwarded_connection::CLOSED_SOCKET) {
			readfds.set(fc->server_socket());
#ifdef DEBUG
			std::clog << "read: " << fc->server_socket() << " ";
#endif

			if ((0 != fc->client_to_server_msgs_count()) || (fc->client_socket() == Forwarded_connection::CLOSED_SOCKET)) {
#ifdef DEBUG
				std::clog << "write: " << fc->server_socket() << " ";
#endif
				writefds.set(fc->server_socket());
			}
		}

	}
}

int add_client_connection(int listening_socket, sockaddr_in &remote_addr, std::list<Forwarded_connection*> &connections) {
#ifdef DEBUG
	std::clog << "Accepting new connection" << std::endl;
#endif
	int client_sock = accept(listening_socket, NULL, NULL);
	if (-1 == client_sock) {
		return -1;
	}

	int server_sock = init_tcp_socket();
	if (-1 == server_sock) {
		return -1;
	}

	if (-1 == connect(server_sock, (const sockaddr *) & remote_addr, sizeof (remote_addr))) {
		return -1;
	}

	Forwarded_connection *fc = new Forwarded_connection(server_sock, client_sock);

	connections.push_back(fc);
#ifdef DEBUG
	std::clog << "New client connection." << "\n" << "server socket: " << server_sock
		<< " client socket: " << client_sock << std::endl;
	std::clog << "Total forwarded connections: " << connections.size() << std::endl;
#endif
	return 0;
}


int main(int argc, char* argv[]) {

	struct sigaction act;
	signal(SIGINT, exit);

	// ignore SIGPIPE
	// needed in case remote host closes connection
	// right before we are going to write to socket
	act.sa_handler=SIG_IGN;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;
	sigaction(SIGPIPE, &act, NULL); 


	unsigned short remote_port;
	unsigned short local_port;
	char *remote_host;

#ifdef DEBUG
	std::clog << "Parameters: " << std::endl;
	for (int i = 0; i < argc; ++i) {
		std::clog << "argv[" << i << "]=" << argv[i] << std::endl;
	}
#endif

	if (-1 == parse_arguments(argc, argv, local_port, remote_port, remote_host)) {
		std::cerr << "Usage: " << argv[0] << " local_port {remote_hostname|remote_ip} remote_port" << std::endl;
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;

	init_local_sockaddr(local_addr, local_port);

	if (-1 == init_remote_host_sockaddr(remote_addr, remote_host, remote_port)) {
		std::cerr << "Getting remote_host info: " << hstrerror(h_errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	int listening_socket = init_tcp_socket();

	if (-1 == listening_socket) {
		std::cerr << "socket(): " << strerror(errno) << std::endl;
	}

	{
		int flag_value = 1;
		// was needed in case programm crashed without freeing listening port
		if(-1 == setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &flag_value, sizeof(flag_value))){
			std::cerr << "setsockopt(): " << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	if (-1 == bind(listening_socket, (const sockaddr *) & local_addr, sizeof (local_addr))) {
		std::cerr << "bind(): " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	int backlog = 5;
	if (-1 == listen(listening_socket, backlog)) {
		std::cerr << "listen(): " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	Fd_set readfds, writefds;
	std::list<Forwarded_connection*> connections;

	while(true) {
		init_fds(readfds, writefds, listening_socket, connections);

		int avaliable_sockets;

#ifdef DEBUG
		std::clog << "Getting to select" << std::endl;
#endif

//		const int SELECT_TIMEOUT_SECONDS = 1;
//		struct timeval tv;
//		tv.tv_sec = SELECT_TIMEOUT_SECONDS;
//		tv.tv_usec = 0;


		avaliable_sockets = select(max(readfds.max_fd(), writefds.max_fd()) + 1, &readfds.fdset(),
				&writefds.fdset(), NULL, NULL /*&tv*/);
		
		// for test purposes; also it lets you make emulation of the slow net services,
		// for instance a slow webserver
		// usleep(1000000/16);

#ifdef DEBUG
		std::clog << "sockets avaliable: " << avaliable_sockets << std::endl;
#endif

		if (-1 == avaliable_sockets) {
			std::cerr << "select(): " << strerror(errno) << std::endl;

			// if interrupted by signal, do nothing, else stop
			if(errno == EINTR)
				continue;
			else
				exit(EXIT_FAILURE);
		}

		// if a new client connecting
		if (readfds.isset(listening_socket)) {
			if (-1 == add_client_connection(listening_socket, remote_addr, connections)) {
				std::cerr << "initing new connection: " << strerror(errno) << std::endl;
			}
		}

#ifdef DEBUG
		std::clog << "Getting to connections forwarding" << std::endl;
#endif

		// doing avaliable work
		for (std::list<Forwarded_connection*>::iterator i = connections.begin();
				i != connections.end(); ++i) {
			Forwarded_connection *fc = *i;

			if(fc->client_socket() != Forwarded_connection::CLOSED_SOCKET)
			if (writefds.isset(fc->client_socket())) {
				fc->client_write();
			}

			if(fc->server_socket() != Forwarded_connection::CLOSED_SOCKET)
			if (writefds.isset(fc->server_socket())) {
				fc->server_write();
			}

			if(fc->client_socket() != Forwarded_connection::CLOSED_SOCKET)
			if (readfds.isset(fc->client_socket())) {
				fc->client_read();
			}

			if(fc->server_socket() != Forwarded_connection::CLOSED_SOCKET)
			if (readfds.isset(fc->server_socket())) {
				fc->server_read();
			}

		}

// making cyclic shift; not needed any more
//		if(connections.size() > 1){
//			Forwarded_connection *fc = connections.front();
//			connections.pop_front();
//			connections.push_back(fc);
//		}

	}
	return (EXIT_SUCCESS);

}
