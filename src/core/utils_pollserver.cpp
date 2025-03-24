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
	new_element.events = POLLIN | POLLOUT; // Check ready-to-read
	new_element.revents = 0;
	this->_pollfds.emplace_back(new_element);
}

void Server::add_to_pollfds_prefilled(pollfd &new_element)
{
	printer::Header("[STATUS]: Add new pollfd prefilled element to pollfd with fd: " \
			+ std::to_string(new_element.fd));
	this->_pollfds.emplace_back(new_element);
}

// void Server::del_from_pollfds(int index)
// {
// 	if (index < 0 || static_cast<size_t>(index) >= _pollfds.size())
// 		return ;
// 	_pollfds.erase(_pollfds.begin() + index);
// }

//Improved Stand: 08.03.2025
void Server::del_from_pollfds(int fd)
{
	auto it = std::remove_if(_pollfds.begin(), _pollfds.end(),
		[fd](const pollfd& pfd) { return pfd.fd == fd; });
	if (it != _pollfds.end())
	{
		_pollfds.erase(it, _pollfds.end());
		std::cout << "Removed fd: " << fd << " from pollfds\n";
	}
}

//Replaced by del_from_pollfds(int fd)
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

//Improved Version: fd based handling
// void Server::ft_closeNclean(int fd)
// {
// 	del_from_map(fd);
// 	auto it = std::find_if(_pollfds.begin(), _pollfds.end(),
// 		[fd](const pollfd& pfd) { return pfd.fd == fd; });
// 	if (it != _pollfds.end())
// 	{
// 		close(it->fd);
// 		_pollfds.erase(it);
// 		_connections.erase(fd); // Entferne aus der Map
// 	}
// 	del_from_pollfds();
// }

// Obsoletes (new: ft_closeNclean(int fd))
void Server::ft_closeNclean(int fd)
{
	// if (fd >= 0)
	// {
	// 	std::cout << "Flagged Closing fd: " << fd << "\n";
	// 	close(fd);
	// }
	// Markiere den fd zur späteren Entfernung aus _pollfds und _connections
	_deferred_close_fds.push_back(fd);
}

// Back-Up Stand: 24.03.25 03:07
// void Server::ft_closeNclean(int fd)
// {
// 	// 1. Schließe den Socket
// 	if (fd != -1)
// 	{
// 		std::cout << "Closing fd: " << fd << "\n";
// 		close(fd);
// 	}
// 	// 2. Entferne aus der Connection-Map
// 	// if (_connections.count(fd)) {
// 	//     std::cout << "Removing from connections map\n";
// 	//     _connections.erase(fd);
// 	// }
// 	del_from_map(fd);
// 	// 3. Entferne aus pollfds
// 	del_from_pollfds(fd);
// 	// 4. Setze FD auf ungültigen Wert
// 	fd = -1;
// }

// Obsoletes (new: ft_closeNclean(int fd))
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

pollfd	*Server::getPollFd()
{
	return (_pollfds.data());
}

void Server::enable_output(int fd)
{
	for(auto& pfd : this->_pollfds)
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
	for (auto& p : _pollfds) {
		if (p.fd == fd) {
			return &p;
		}
	}
	return (nullptr);
}

// void	Server::cleanup_connections(void)
// {
// 	for (int fd : connections_to_remove) {
// 		del_from_map(fd);
// 		del_from_pollfds(fd);
// 	}
// 	connections_to_remove.clear();
// }

void Server::cleanup_deferred(void)
{
	// Baue eine Hash-Set aus den FDs, die geschlossen werden sollen
	std::unordered_set<int> to_close(_deferred_close_fds.begin(), _deferred_close_fds.end());
	// Entferne alle pollfd-Einträge, deren fd in to_close enthalten ist
	_pollfds.erase(
		std::remove_if(_pollfds.begin(), _pollfds.end(),
			[&to_close](const pollfd& pfd) {
				if (to_close.count(pfd.fd) > 0) {
					close(pfd.fd);
					return (true);
				} else {
					return (false);
				}
			}),
		_pollfds.end()
	);
	// _pollfds.erase(
	// 	std::remove_if(_pollfds.begin(), _pollfds.end(),
	// 		[&to_close](const pollfd& pfd) {
	// 			return to_close.count(pfd.fd) > 0;
	// 		}),
	// 	_pollfds.end()
	// );
	// Entferne alle Einträge aus der Connection-Map, deren Key in to_close ist
	for (auto it = _connections.begin(); it != _connections.end(); ) {
		if (to_close.count(it->first) > 0) {
			std::cout << "Removed fd: " << it->first << " from connections\n";
			it = _connections.erase(it);
		}
		else {
			++it;
		}
	}
	// Leere den Vektor für deferred closures
	_deferred_close_fds.clear();
}

// void Server::cleanup_deferred(void)
// {
// 	for (int fd : _deferred_close_fds)
// 	{
// 		// dell from _pollfds:
// 		auto it = std::remove_if(_pollfds.begin(), _pollfds.end(),
// 			[fd](const pollfd& pfd) { return pfd.fd == fd; });
// 		if (it != _pollfds.end())
// 		{
// 			_pollfds.erase(it, _pollfds.end());
// 			std::cout << "Removed fd: " << fd << " from pollfds\n";
// 		}
// 		//dell form Connection-Map
// 		del_from_map(fd);
// 	}
// 	_deferred_close_fds.clear();
// }
