#include "Core.hpp"
#include <unordered_set>

//OCF -- BEGIN
Core::Core(void)
	:	tokenList(),
		_servers(),
		_server_confs(),
		_pollfds{},
		_deferred_close_fds{},
		_listener_fds{}
{}

Core::~Core(void)
{
	for (const auto& pfd : _pollfds) {
		if (pfd.fd >= 0) {
			::close(pfd.fd);
		}
	}
	_pollfds.clear();
}
//OCF -- END

//METHODS -- BEGIN

std::vector<struct pollfd>&	Core::getPollFds()
{
	return (_pollfds);
}

void	Core::addPollFd(int new_fd)
{
	struct pollfd	new_element;

	new_element.fd = new_fd;
	new_element.events = POLLIN | POLLOUT;
	new_element.revents = 0;
	this->_pollfds.emplace_back(new_element);
}

void	Core::addPollFdPrefilled(const pollfd &pfd)
{
	_pollfds.emplace_back(pfd);
}

void Core::removePollFd(int fd)
{
	_pollfds.erase(
		std::remove_if(_pollfds.begin(), _pollfds.end(),
			[fd](const pollfd &p) {
				if (p.fd == fd) {
					::close(p.fd);
					return (true);
				}
				return (false);
			}),
		_pollfds.end()
	);
}

pollfd	*Core::getPollFdElement(int fd)
{
	for (auto &p : _pollfds) {
		if (p.fd == fd) {
			return (&p);
		}
	}
	return (nullptr);
}

void Core::cleanup_deferred(void)
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
	// Entferne alle Einträge aus der Connection-Map, deren Key in to_close ist
	for (size_t i = 0; i < _servers.size(); i++) {
		// Verwende eine Referenz auf den Connection-Map des aktuellen Servers
		auto& conns = _servers.at(i)._connections;
		for (auto it = conns.begin(); it != conns.end(); ) {
			if (to_close.count(it->first) > 0) {
				std::cout << "Removed fd: " << it->first << " from connections\n";
				it = conns.erase(it);
			} else {
				++it;
			}
		}
	}
	// Leere den Vektor für deferred closures
	_deferred_close_fds.clear();
}

void	Core::poll_loop(void)
{
	getToken("configFiles/default.conf", tokenList);
	_server_confs = parsing(tokenList);
	for (auto conf : _server_confs) {
		// _servers.push_back(Server(conf)); //TODO: In Servers eine Reference zur Core instanz geben
		_servers.emplace_back(Server(conf, *this));
	}
	for (;;)
	{
		int poll_count = poll(_pollfds.data(), static_cast<nfds_t>(_pollfds.size()), -1);
		if (poll_count < 0)
		{
			perror("poll");
			exit(1);
		}
		for (size_t i = 0; i < _servers.size(); i++) {
			_servers[i].execute();
		}
		cleanup_deferred();
	}
}

//METHODS -- END
