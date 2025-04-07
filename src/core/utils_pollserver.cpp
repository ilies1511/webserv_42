#include "Server.hpp"
#include <unordered_set>
#include <algorithm>

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
	// RAII: Smart Pointer mit Custom-Deleter für addrinfo (nun keine Notwendigkeit mehr, freeaddrinfo(ai) manuell zu callen)
	std::unique_ptr<struct addrinfo, decltype(&freeaddrinfo)> ai_raii(
		ai,				// Übergebe das allokierte addrinfo-Objekt
		&freeaddrinfo  	// Custom-Deleter-Funktion
	);
	for (p = ai; p != NULL; p = p->ai_next)
	{
		listener_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		// std::cout << "socket: " << listener_fd << std::endl;
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
	// freeaddrinfo(ai); // All done with this --> not needed since RAII: Smart Pointer mit Custom-Deleter für addrinfo
	// Listen
	if (listen(listener_fd, 128) == -1)
	{
		close(listener_fd);
		throw (std::runtime_error("Failed to listen"));
	}
}

// Set up and get a listening socket
void Server::init_listener_socket(void)
{
	get_listener_socket();
	if (listener_fd == -1)
	{
		throw (std::runtime_error("error getting listening socket\n"));
	}
	struct pollfd init_listener;
	init_listener.fd = listener_fd;
	init_listener.events = POLLIN;
	init_listener.revents = 0;

	P_DEBUG(("INIT_LISTENER - listening on port: " + std::to_string(this->_config.getPort()) + "\n").c_str());
	printer::Header("In Init_Listener_Socket");
	_core._pollfds.emplace_back(init_listener);
	_core._listener_fds.emplace_back(init_listener.fd);
	#ifdef DEBUG
	std::cout << "POST emplace back " << _core._pollfds.size() << "\n";
	int flags = fcntl(_core._pollfds.back().fd, F_GETFL, 0);
	std::cout << PURPLE << __FILE__ << " " << __FUNCTION__ << " :Socket "
				<< _core._pollfds.back().fd << " flags: " << flags
				<< " (Non-Blocking: " << (flags & O_NONBLOCK ? "YES" : "NO") << NC << "\n";
	for (size_t i = 0; i < _core._pollfds.size(); i++)
	{
		printf("FD: %d\n", _core._pollfds.at(i).fd);
		printf("Event: %d\n", _core._pollfds.at(i).events);
	}
	#endif
}


void Server::add_to_pollfds(int new_fd)
{
	struct pollfd	new_element;

	// _pollfds.push_back({newfd, POLLIN, 0}); //Kuerzer aber nicht leserlich
	new_element.fd = new_fd;
	new_element.events = POLLIN | POLLOUT; // Check ready-to-read
	new_element.revents = 0;
	_core._pollfds.emplace_back(new_element);
}

void Server::add_to_pollfds_prefilled(pollfd &new_element)
{
	printer::Header("[STATUS]: Add new pollfd prefilled element to pollfd with fd: " \
			+ std::to_string(new_element.fd));
	_core._pollfds.emplace_back(new_element);
}

//Improved Stand: 08.03.2025
void Server::del_from_pollfds(int fd)
{
	auto it = std::remove_if(_core._pollfds.begin(), _core._pollfds.end(),
		[fd](const pollfd& pfd) { return pfd.fd == fd; });
	if (it != _core._pollfds.end())
	{
		_core._pollfds.erase(it, _core._pollfds.end());
		std::cout << "Removed fd: " << fd << " from pollfds\n";
	}
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
	int	old_err = errno;
	if (fcntl(fd, F_SETFL, O_NONBLOCK | O_CLOEXEC) == -1)
	{
		std::cerr << "fcntl(F_SETFL) failed: " << strerror(errno) << std::endl;
			close(listener_fd);
			return ;
	}
	#ifdef DEBUG
	// flags = fcntl(fd, F_GETFL, 0);
	// std::cout << PURPLE << __FILE__ << " " << __FUNCTION__ << " :Socket " << fd << " flags: " << flags << " (Non-Blocking: " << (flags & O_NONBLOCK ? "YES" : "NO") << NC << "\n";
	#endif
	errno = old_err;
}

void	Server::del_from_map(int fd)
{
	auto it = this->_connections.find(fd);
	if (it != _connections.end())
	{
		_connections.erase(it);
	}
}

// Obsoletes (new: ft_closeNclean(int fd))
void Server::ft_closeNclean(int fd)
{
	if (fd >= 0) {
		std::cout << "Flagged Closing fd: " << fd << "\n";
	}
	// Markiere den fd zur späteren Entfernung aus _pollfds und _connections
	_core._deferred_close_fds.push_back(fd);
}

void	Server::add_to_map(int fd)
{
	//Maybe need to add some checks (ignore addition, if key already known)
	this->_connections.emplace(fd, std::make_unique<Connection>(fd, *this));
	return ;
}

pollfd	*Server::getPollFd()
{
	return (_core._pollfds.data());
}

void Server::enable_output(int fd)
{
	for(auto& pfd : _core._pollfds)
	{
		if(pfd.fd == fd)
		{
			pfd.events = POLLOUT;
			break;
		}
	}
}

pollfd* Server::getPollFdElement(int fd)
{
	for (auto& p : _core._pollfds) {
		if (p.fd == fd) {
			return &p;
		}
	}
	return (nullptr);
}

void Server::check_connection_timeouts(void)
{
	for (auto& [fd, conn] : _connections)
	{
		if (conn->is_timed_out()) {
			std::cout << coloring("Timeout: Queueing connection " + \
					std::to_string(fd) + " for closure\n", LIGHT_RED);
			_core._deferred_close_fds.push_back(fd);
		}
	}
}
