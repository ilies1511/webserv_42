#include "Server.hpp"

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
		setup_non_blocking(listener_fd);
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

void	Server::init_listener_socket(void)
{
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
	printer::Header("In Init_Listener_Socket");
	std::cout << "PRE emplace back " << _pollfds.size() << "\n";
	this->_pollfds.emplace_back(init_listener);
	std::cout << "POST emplace back " << _pollfds.size() << "\n";
	_pollfds.at(0).events = POLLIN;
	// _pollfds[0].events = POLLIN; // Report ready to read on incoming connection
	// Main loop
	int flags = fcntl(_pollfds.at(0).fd, F_GETFL, 0);
	std::cout << PURPLE << __FILE__ << " " << __FUNCTION__ << " :Socket " << _pollfds.at(0).fd << " flags: " << flags << " (Non-Blocking: " << (flags & O_NONBLOCK ? "YES" : "NO") << NC << "\n";
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
	std::cout << RED << "In " << __FUNCTION__ << " " << __LINE__ << " Size pre Del: " << _pollfds.size() << NC << "\n";
	std::swap(_pollfds[index], _pollfds.back());
	_pollfds.pop_back();
	std::cout << RED << "In " << __FUNCTION__ << " " << __LINE__ << " Size POST Del: " << _pollfds.size() << NC << "\n";
}

// Get sockaddr, IPv4 or IPv6:
void *Server::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void Server::setup_non_blocking(int fd)
{
	//Warum errno auf old setzten, was macht errno ?
	int	old_err = errno;
	// Non-Blocking für alle Sockets (F_SETFL)
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
	{
		std::cerr << "fcntl(F_GETFL) failed: " << strerror(errno) << std::endl;
			close(listener_fd);
			return ;
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::cerr << "fcntl(F_SETFL) failed: " << strerror(errno) << std::endl;
			close(listener_fd);
			return ;
	}
	flags = fcntl(fd, F_GETFL, 0);
	std::cout << PURPLE << __FILE__ << " " << __FUNCTION__ << " :Socket " << fd << " flags: " << flags << " (Non-Blocking: " << (flags & O_NONBLOCK ? "YES" : "NO") << NC << "\n";
	// FD_CLOEXEC nur für macOS (F_SETFD)
	#ifdef __APPLE__
	if (fcntl(fd, F_SETFD, FD_CLOEXEC) == -1)
	{
		std::cerr << "fcntl(F_SETFD) failed: " << strerror(errno) << std::endl;
			close(listener_fd);
			return ;
	}
	#endif
	errno = old_err;
}

////TODO: TimeOuts
// void	Server::check_timeouts(void)
// {}


void	Server::del_from_map(int fd)
{
	auto it = this->_connections.find(fd);
	if (it != _connections.end())
	{
		_connections.erase(it);
	}
}

void	Server::ft_closeNclean(size_t i)
{
	// close(_pollfds[i].fd); // Bye!

	printer::debug_putstr("alo", __FILE__, __FUNCTION__, __LINE__);
	if (i >= _pollfds.size())
	{
		std::cerr << "Error: Invalid index " << i
				  << " for _pollfds (size = " << _pollfds.size() << ")\n";
		del_from_map(_pollfds[i].fd);
		return ;
	}
	const int fd = _pollfds[i].fd;
	if (fd >= 0)
	{
		close(fd);
		std::cout << "Closed connection on fd " << fd << "\n";
	}
	else
	{
		std::cerr << "Warning: Invalid fd " << fd << " at index " << i << "\n";
	}
	del_from_pollfds(i);
	del_from_map(_pollfds[i].fd);
}

void	Server::add_to_map(int fd)
{
	//Maybe need to add some checks (ignore addition, if key already known)
	// this->_connections.insert({fd, new Connection(fd)});
	this->_connections.emplace(fd, std::make_unique<Connection>(fd, *this));
	return ;
}
