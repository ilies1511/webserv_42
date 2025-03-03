#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <memory>
#include <vector>
#include <unordered_map>
#include <poll.h>
#include "printer.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include "Connection.hpp"

#define TIMEOUT 1000

class Server
{
	private:
		// std::unordered_map<int, std::chrono::steady_clock::time_point> _last_activity;
		std::unordered_map< int, std::unique_ptr<Connection> > _connections;
		// std::unordered_map< int, std::unique_ptr<Connection> >	_connections;
		int										listener_fd;
		std::vector< struct pollfd> 			_pollfds; //Stores FD of Connections
		const std::string						_port;
	//OCF -- BEGIN
	public:
		Server(const std::string &port);
		~Server(void);
	private:
		Server(void) = delete;
		Server& operator=(const Server& other) = delete;
		Server(const Server& other) = delete;
	//OCF -- END

	//Methodes -- BEGIN
	public:
		void	poll_loop(void);
	//Utils
	// Get sockaddr, IPv4 or IPv6:
		void	init_listener_socket(void);
		void	get_listener_socket(void); //Only one per Server
		void	add_to_pollfds(int new_fd);
		void	del_from_pollfds(size_t index);
		void	*get_in_addr(struct sockaddr *sa);
		void	new_connection_handler(void);
		void	regular_Client_handler(size_t &i);
		void	setup_non_blocking(int fd);
		void	add_to_map(int fd);
		void	del_from_map(int fd);
		void	ft_closeNclean(size_t i);
		//TODO:
		// void	check_timeouts(void);
	//Methodes -- END

};

#endif
