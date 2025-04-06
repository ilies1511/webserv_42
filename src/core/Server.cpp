#include "Server.hpp"
#include "Connection.hpp"
#include <cstring>
#include <stdio.h>

#define PORT "9034"   // Port we're listening on

Server::Server(const serverConfig &conf, Core &core)
	:	_core(core),
		_connections{},
		listener_fd(-1),
		_port(std::to_string(conf.getPort())),
		_config(conf),
		_cookies{}
{
	printer::ocf_printer("Server", printer::OCF_TYPE::DC);
	init_listener_socket();
}

Server::~Server(void)
{
	printer::ocf_printer("Server", printer::OCF_TYPE::D);
}
// OCF -- END

// Methodes -- BEGIN
void	Server::new_connection_handler(void)
{
	int	newfd;
	socklen_t addrlen;
	struct sockaddr_storage remoteaddr; // Client address
	char remoteIP[INET6_ADDRSTRLEN];

	// If listener_fd is ready to read, handle new connection
	addrlen = sizeof(remoteaddr);
	newfd = accept(listener_fd,
		(struct sockaddr *)&remoteaddr, &addrlen);
	if (newfd == -1) {
		perror("accept");
	}
	else
	{
		setup_non_blocking(newfd);
		add_to_pollfds(newfd);
		add_to_map(newfd);
		printf("pollserver: new connection from %s on "
			"socket %d\n",
			inet_ntop(remoteaddr.ss_family,
				get_in_addr((struct sockaddr*)&remoteaddr),
				remoteIP, INET6_ADDRSTRLEN),
			newfd);
	}
}

void	Server::execute(void)
{
	/*
		Ich muss mir irgendwie die Postion des listener_socket des jeweiligen
		Servers merken
		(Alternativ auch mit fd value direkt gehen, aber ineffizient weil O(n)
		statt indexbasiert mit O(1))
		im pollfd-Array
		speichern und wenn dessen revents auf POLLIN getriggert wird, muss
		ich eine neue Connection aufnehmen
	*/
	//range-based Loop
	// for (const auto& [key, conn] : _connections)
	for (const auto& [key, conn] : _connections) {
		conn->execute_layer2();
	}
	for (auto& p : _core._pollfds)
	{
		if (p.fd == listener_fd)
		{
			if (p.revents & POLLIN) {
				P_DEBUG("In Server Execute Loop pre new_connection_handler\n");
				new_connection_handler();
			}
			break;
		}
	}
	check_connection_timeouts();
}

bool	Server::is_valid_cookie(const std::string& cookie_string)
{
	// (void)cookie_string;
    // std::cout << coloring("IS valid cookie", BLUE) << std::endl;
    // std::cout << coloring("cookie size " + std::to_string(_cookies.size()) , BLUE) << std::endl;
	for (size_t i = 0; i < _cookies.size(); i++)
	{
		P_DEBUG("PRE Condition check\n");
		if (_cookies.at(i).cookie_string == cookie_string) {
			if (_cookies.at(i).getMaxAge() != 0 && _cookies.at(i).getMaxAge() <= std::time(nullptr)) {
				_cookies[i] = _cookies.back();
				_cookies.pop_back();
				return false;
			}
			return (true);
		}
		#ifdef DEBUG
		std::cout << coloring(_cookies[i].cookie_string,BLUE) << std::endl;
		#endif
	}
	return (false);
	// return (true);
}

// Methodes -- END
