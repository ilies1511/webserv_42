#include "Core.hpp"
#include <unordered_set>

//OCF -- BEGIN
Core::Core(void)
	:	tokenList(),
		_servers(),
		_server_confs(),
		_pollfds{},
		_deferred_close_fds{}
{}

Core::~Core(void) {}
//OCF -- END

//METHODS -- BEGIN

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
	// _pollfds.erase(
	// 	std::remove_if(_pollfds.begin(), _pollfds.end(),
	// 		[&to_close](const pollfd& pfd) {
	// 			return to_close.count(pfd.fd) > 0;
	// 		}),
	// 	_pollfds.end()
	// );
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
		// _servers.push_back(std::move(Server(conf)));
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

/*
int	main(void)
{

	std::vector<TOKEN> tokenList;
	getToken("configFiles/default.conf", tokenList);
	// printToken(tokenList);

	std::vector<Server> servers;
	std::vector<serverConfig> server_confs;
	try {
		server_confs = parsing(tokenList);
		// servers[0].getPort();
		// Server alo("9035");
		// Server alo(std::to_string(servers[0].getPort()));
		std::vector<Server>	servers;
		for (auto conf : server_confs) {
			servers.push_back(Server(conf));
		}
		for (;;)
		{
			int poll_count = poll(_pollfds.data(), static_cast<nfds_t>(_pollfds.size()), 0);
			if (poll_count < 0)
			{
				perror("poll");
				exit(1);
			}
			for (size_t i = 0; i < servers.size(); i++) {
				servers[i].execute();
			}
		// 	for ()
		// 	{
		// 		_server_vector.execute(); //Kuenftiger Refactor wird einen Loop haben, der durch die Server durchiteriert
		// 	}
		}
		// Server alo(servers[0]);
		// alo.poll_loop(); //MAIN LOOP
	} catch (const std::exception& e) {
		std::cout << coloring("Error: " + std::string(e.what()), RED) << std::endl;
		return 1;
	}
	// const std::string request = exampleGetRequest();
	// requestParser request_parser(request, servers);
	// request_parser.parseMessage();


	// Log	log("webserv");

	// log.complain("INFO", "Willkommen", __FILE__, __FUNCTION__, __LINE__);
	// printer::Header("main");
	// std::cout << "ALO aus main.cpp\n";
	// printer::Header("dummy func Call");
	// dummy_function();
	// printer::Header("src func Call");
	// src_function();
	// return (0);

	// Log	log("webserv");

	// log.complain("INFO", "Willkommen, connect to server via telnet localhost 9034 in an other terminal", __FILE__, __FUNCTION__, __LINE__);
	// try
	// {
	// 	Server alo("9034");
	// 	alo.poll_loop(); //MAIN LOOP
	// }
	// printServerConfig(servers);
	return (0);
}
*/



// int	main(void)
// {
// 	std::vector<TOKEN> tokenList;
// 	getToken("configFiles/default.conf", tokenList);
// 	// printToken(tokenList);

// 	std::vector<Server> servers;
// 	std::vector<serverConfig> server_confs;
// 	try {
// 		server_confs = parsing(tokenList);
// 		std::vector<Server>	servers;
// 		for (auto conf : server_confs) {
// 			servers.push_back(Server(conf));
// 		}
// 		for (;;)
// 		{
// 			int poll_count = poll(_pollfds.data(), static_cast<nfds_t>(_pollfds.size()), 0);
// 			if (poll_count < 0)
// 			{
// 				perror("poll");
// 				exit(1);
// 			}
// 			for (size_t i = 0; i < servers.size(); i++) {
// 				servers[i].execute();
// 			}
// 		}
// 	} catch (const std::exception& e) {
// 		std::cout << coloring("Error: " + std::string(e.what()), RED) << std::endl;
// 		return 1;
// 	}
// 	return (0);
// }
