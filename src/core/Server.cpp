#include "Server.hpp"
#include "Connection.hpp"
#include <cstring>
#include <stdio.h>

#define PORT "9034"   // Port we're listening on

// OCF -- BEGIN
Server::Server(const std::string &port) : _connections{}, listener_fd(-1), _pollfds{}, _port(port)
{
	(void)port;
	printer::ocf_printer("Server", printer::OCF_TYPE::DC);
	init_listener_socket();
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

void	Server::regular_Client_handler(size_t &i)
{
	// char buf[256];    // Buffer for client data
	// If not the listener_fd, we're just a regular client
	// int nbytes = (int)recv(_pollfds[i].fd, buf, sizeof buf, 0);
	/*
		TODO: recv mit Buffer der Client Instance und dessen Buffer (Class)
		Attribut / gleiches gilt auch fuer sendv()
	*/
	int	fd = _pollfds.at(i).fd;
	auto it = _connections.find(fd);
	if (it == _connections.end())
	{
		ft_closeNclean(i);  // Handle missing connection
		return;
	}

	char	*buf = (char *)_connections[_pollfds[i].fd]->_InputBuffer.data();
	char	*buf_output = (char *)_connections[_pollfds[i].fd]->_OutputBuffer.data();
	(void)buf_output;
	size_t buf_size = _connections[_pollfds[i].fd]->_InputBuffer.size();
	// size_t buf_size_output = _connections[_pollfds[i].fd]->_OutputBuffer.size();

	int nbytes = (int)recv(_pollfds[i].fd, buf, buf_size, 0);
	// int nbytes = (int)recv(_pollfds[i].fd, buf, \
	// 		sizeof (this->_connections[_pollfds[i].fd]->_InputBuffer), 0);

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
		// close(_pollfds[i].fd); // Bye!
		// del_from_pollfds(i);
		// del_from_map(_pollfds[i].fd);
		ft_closeNclean(i);
		//TODO: Mit Connection Instance arbeiten --> Object mit das dem Key entspricht aus Map loeschen
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
				std::cout << "buf len: " << std::strlen(buf) << "\n";
				std::cout << "buf content: " << buf << "\n";
				if (send(dest_fd, buf, (size_t)nbytes, 0) == -1) {
					perror("send");
				}
			}
		}
	}
} // END handle data from client

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

void Server::poll_loop(void)
{
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
				// If listener_fd is ready to read, handle new connection
				if (_pollfds[i].fd == listener_fd)
					new_connection_handler();
				else
				{
					std::cout << "Var. i PRE regular Client Handler " << i <<"\n";
					regular_Client_handler(i);
					std::cout << "Var. i POST regular Client Handler " << i <<"\n";
				}
			} // END got ready-to-read from poll()
		} // END looping through file descriptors
	} // END for(;;)--and you thought it would never end!
}

// Methodes -- END
