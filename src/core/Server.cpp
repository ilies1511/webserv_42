#include "Server.hpp"
#include "Connection.hpp"
#include <cstring>
#include <stdio.h>

#define PORT "9034"   // Port we're listening on

Server::Server(const serverConfig &conf, Core &core)
	:	_core(core),
		// _pollfds{},
		_connections{},
		listener_fd(-1),
		_port(std::to_string(conf.getPort())),
		_config(conf)
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
		//TODO: Mit Connection Instance arbeiten --> neues Objekt
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
	for (auto& p : _core._pollfds) //TODO: 01.04.2025 Core Class std::vector<int> _listener_fds hinzufuegen
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
// Methodes -- END
