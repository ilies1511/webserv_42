#include "Server.hpp"
#define PORT "9034"   // Port we're listening on

// OCF -- BEGIN
Server::Server(const std::string &port) : _connections{}, listener_fd(-1), _pollfds{}, _port(port)
{
	(void)port;
	printer::ocf_printer("Server", printer::OCF_TYPE::DC);
}

Server::~Server(void)
{
	printer::ocf_printer("Server", printer::OCF_TYPE::D);
	for (auto& pfd : _pollfds)
	{
		close(pfd.fd);
	}
	close(listener_fd);
}
// OCF -- END

// Methodes -- BEGIN
void Server::get_listener_socket(void)
{
	int	rv;

	int yes = 1; // For setsockopt() SO_REUSEADDR, below
	struct addrinfo hints, *ai, *p;
	// Get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(NULL, _port.c_str(), &hints, &ai)) != 0)
	{
		// fprintf(stderr, "pollserver: %s\n", gai_strerror(rv));
		// exit(1);
		throw (std::runtime_error("pollserver: " + std::string(gai_strerror(rv))));
	}
	// 🔴 RAII: Smart Pointer mit Custom-Deleter für addrinfo (nun keine Notwendigkeit mehr, freeaddrinfo(ai) manuell zu callen)
	std::unique_ptr<struct addrinfo, decltype(&freeaddrinfo)> ai_raii(
		ai,				// Übergebe das allokierte addrinfo-Objekt
		&freeaddrinfo  	// Custom-Deleter-Funktion
	);
	for (p = ai; p != NULL; p = p->ai_next)
	{
		listener_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener_fd < 0)
			continue ;
		// Lose the pesky "address already in use" error message
		if (setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			throw (std::runtime_error("setsockopt Failed"));
			close(listener_fd);
		}
		if (bind(listener_fd, p->ai_addr, p->ai_addrlen) < 0)
		{
			close(listener_fd);
			continue ;
		}
		break ;
	}
	// If we got here, it means we didn't get bound
	if (p == NULL)
	{
		throw (std::runtime_error("Failed to bind"));
		// this->listener_fd = -1;
		// return ;
	}
	// freeaddrinfo(ai); // All done with this --> not needed since 🔴 RAII: Smart Pointer mit Custom-Deleter für addrinfo
	// Listen
	if (listen(listener_fd, 10) == -1)
	{
		close(listener_fd);
		throw (std::runtime_error("Failed to listen"));
		// this->listener_fd = -1;
		// return ;
	}
}

void Server::add_to_pollfds(int new_fd)
{
	struct pollfd	new_element;

	// _pollfds.push_back({newfd, POLLIN, 0}); //Kuerzer aber nicht leserlich
	new_element.fd = new_fd;
	new_element.events = POLLIN; // Check ready-to-read
	new_element.revents = 0;
	this->_pollfds.emplace_back(new_element);
}

// void Server::del_from_pollfds(int index)
// {
// 	if (index < 0 || static_cast<size_t>(index) >= _pollfds.size())
// 		return ;
// 	_pollfds.erase(_pollfds.begin() + index);
// }

void Server::del_from_pollfds(size_t index)
{
	if (index >= _pollfds.size())
		return;
	std::swap(_pollfds[index], _pollfds.back());
	_pollfds.pop_back();
}

void Server::poll_loop(void)
{
	int newfd;        // Newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr; // Client address
	socklen_t addrlen;
	char buf[256];    // Buffer for client data
	char remoteIP[INET6_ADDRSTRLEN];

	// Set up and get a listening socket
	get_listener_socket();
	if (listener_fd == -1)
	{
		// fprintf(stderr, "error getting listening socket\n");
		// exit(1);
		throw (std::runtime_error("error getting listening socket\n"));
	}
	struct pollfd	init_listener;

	init_listener.fd = listener_fd;
	// Add the listener to set
	this->_pollfds.emplace_back(init_listener);
	_pollfds.at(0).events = POLLIN;
	// _pollfds[0].events = POLLIN; // Report ready to read on incoming connection
	// Main loop

	for(;;)
	{
		int poll_count = poll(&_pollfds[0], (nfds_t)_pollfds.size(), -1);

		if (poll_count == -1)
		{
			perror("poll");
			exit(1);
		}
		// Run through the existing connections looking for data to read
		for(size_t i = 0; i < _pollfds.size(); i++)
		{
			// Check if someone's ready to read
			if (_pollfds[i].revents & (POLLIN | POLLHUP)) // We got one!!
			{
				if (_pollfds[i].fd == listener_fd)
				{
					// If listener_fd is ready to read, handle new connection
					addrlen = sizeof remoteaddr;
					newfd = accept(listener_fd,
						(struct sockaddr *)&remoteaddr, &addrlen);
					if (newfd == -1) {
						perror("accept");
					}
					else
					{
						add_to_pollfds(newfd);
						printf("pollserver: new connection from %s on "
							"socket %d\n",
							inet_ntop(remoteaddr.ss_family,
								get_in_addr((struct sockaddr*)&remoteaddr),
								remoteIP, INET6_ADDRSTRLEN),
							newfd);
					}
				}
				else
				{
					// If not the listener_fd, we're just a regular client
					int nbytes = (int)recv(_pollfds[i].fd, buf, sizeof buf, 0);

					int sender_fd = _pollfds[i].fd;

					if (nbytes <= 0)
					{
					// Got error or connection closed by client
						if (nbytes == 0) {
							// Connection closed
							printf("pollserver: socket %d hung up\n", sender_fd);
						}
						else
						{
							perror("recv");
						}
						close(_pollfds[i].fd); // Bye!
						del_from_pollfds(i);
						i--;
					}
					else
					{
					// CLIENTs MSG AUSGABE -- BEGIN
						//Methode 3 um txt & binary auszulesen
						printf("Received %d bytes of data\n", nbytes);
						printf("Raw data (hex): ");
						for (int k = 0; k < nbytes; k++)
						{
							printf("%02x ", (unsigned char)buf[k]);
						}
						printf("\n");

						// Sanitize input to handle both \r\n and stray \r or \n
						if (nbytes >= 2 && buf[nbytes - 2] == '\r' && buf[nbytes - 1] == '\n')
						{
						    buf[nbytes - 2] = '\n';
						    nbytes -= 1; // Correctly adjust the byte count
						}
						else if (nbytes >= 1 && (buf[nbytes - 1] == '\r' || buf[nbytes - 1] == '\n'))
						{
						    buf[nbytes - 1] = '\n';
						}
						fwrite(buf, 1, (size_t)nbytes, stdout);
						printf("\n");

						// We got some good data from a client
						//Methode 2 um nur textdata und Binary auszulesen
						// printf("Received %d bytes of data\n", nbytes);
						// fwrite(buf, 1, (size_t)nbytes, stdout);
						// printf("\n");

						//Methode 1 um nur textdata auszulesen
						// buf[nbytes] = '\0';
						// printf("Received data: %s\n", buf);
					// CLIENTs MSG AUSGABE -- END
						for(size_t j = 0; j < _pollfds.size(); j++)
						{
							// Send to everyone!
							int dest_fd = _pollfds[j].fd;

							// Except the listener_fd and ourselves
							if (dest_fd != listener_fd && dest_fd != sender_fd)
							{
								if (send(dest_fd, buf, (size_t)nbytes, 0) == -1) {
									perror("send");
								}
							}
						}
					}
				} // END handle data from client
			} // END got ready-to-read from poll()
		} // END looping through file descriptors
	} // END for(;;)--and you thought it would never end!
}

// Get sockaddr, IPv4 or IPv6:
void *Server::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Methodes -- END
