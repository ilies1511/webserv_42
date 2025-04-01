#include "Server.hpp"
#include "Connection.hpp"
#include <cstring>
#include <stdio.h>

#define PORT "9034"   // Port we're listening on

// OCF -- BEGIN
// Server::Server(const std::string &port)
// 	:	_core
// 		_pollfds{},
// 		_connections{},
// 		listener_fd(-1),
// 		_port(port)
// {
// 	(void)port;
// 	printer::ocf_printer("Server", printer::OCF_TYPE::DC);
// 	init_listener_socket();
// }
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
	// for (auto& pfd : _pollfds)
	// {
	// 	close(pfd.fd);
	// }
	// close(listener_fd);
}
// OCF -- END

// Methodes -- BEGIN
void	Server::new_connection_handler(void)
{
	// if (_pollfds[_i].revents & (POLLIN | POLLHUP))
	// {
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
	// }
}

// void Server::poll_loop(void)
// {
// 	for (;;)
// 	{
// 		int poll_count = poll(_pollfds.data(), static_cast<nfds_t>(_pollfds.size()), 0);
// 		if (poll_count < 0)
// 		{
// 			perror("poll");
// 			exit(1);
// 		}
// 		execute(); //Kuenftiger Refactor wird einen Loop haben, der durch die Server durchiteriert
// 	}
// }

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
	for (const auto& [key, conn] : _connections)
	{
		// printer::debug_putstr("PRE execute Layer 1", __FILE__, __FUNCTION__, __LINE__);
		conn->execute_layer2(); // Zugriff auf unique_ptr-Inhalt
		// printer::debug_putstr("POST execute Layer 1", __FILE__, __FUNCTION__, __LINE__);
	}

	//Check for New-Connection Bewusst unten und nicht als aller erstes
	//TODO: check in function that retuns a bool if listener_fd was triggert with revent POLLIN
	for (auto& p : _core._pollfds)
	{
		if (p.fd == listener_fd)
		{
			if (p.revents & POLLIN) {
				P_DEBUG("In Server Execute Loop pre new_connection_handler\n");
				new_connection_handler();
			}
			// if (p.revents & POLLHUP) {
			// 	//handle Connnection Hung Up;
			// }
			break;
		}
	}
	check_connection_timeouts();
	// cleanup_deferred();

	// if (_pollfds[_i].revents & (POLLIN)) //Die Condition muss angepasst werden, da ich nicht mehr global mit durchiteriere
	// {
	// 	if (_pollfds[_i].fd == listener_fd)
	// 		new_connection_handler();
	// }
}
// Methodes -- END
