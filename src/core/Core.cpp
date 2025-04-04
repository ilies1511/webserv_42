#include "Core.hpp"
#include <atomic>
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

void	Core::poll_loop(void)
{
	getToken("configFiles/default.conf", tokenList);
	_server_confs = parsing(tokenList);
	for (auto conf : _server_confs) {
		// _servers.push_back(Server(conf)); //TODO: In Servers eine Reference zur Core instanz geben
		_servers.emplace_back(Server(conf, *this));
	}
	while (running.load())
	{
		int poll_count = poll(_pollfds.data(), static_cast<nfds_t>(_pollfds.size()), -1);
		if (poll_count < 0)
		{
			perror("poll");
			std::cout << "alo\n";
			if (errno != EINTR)
			{
				running = false;
			}
		}
		for (size_t i = 0; i < _servers.size(); i++) {
			_servers[i].execute();
		}
		cleanup_deferred();
	}
}

//METHODS -- END
