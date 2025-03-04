#pragma once

#include <unordered_map>
#include <iostream>
#include "route.hpp"

class serverConfig {
	private:
		std::string								_ip;
		std::size_t					 			_port;
		std::vector<std::string>				_server_name;
        std::unordered_map<size_t, std::string>	_error_pages;
        std::size_t								_client_max_body_size; // in bytes

		std::vector<std::string>				_limit_except;
       	std::string								_root;
		double									_timeout; // in seconds
        // std::vector<route>						_location;
		std::unordered_map<std::string, route>	_location;



	public:
		serverConfig();
		~serverConfig();

		void	printData();

		//getters
		std::string						        getIP() const;
		std::size_t						        getPort() const;
		std::vector<std::string>		        getServerName() const;
		std::unordered_map<size_t, std::string>	getErrorPages() const;
		std::size_t								getClientMaxBodySize() const;

		std::vector<std::string>		        getLimitsExcept() const;
		std::string						        getRoot() const;
		double									getTimeout() const;
		// std::vector<route>				        getLocation() const;
		std::unordered_map<std::string, route>	getLocation() const;
		//setters
		void	setIP(const std::string& ip);
		void	setPort(std::size_t port);
		void	setServerName(const std::string& serverName);
		void	setErrorPages(std::size_t errorNbr, const std::string& path);
		void	setClientMaxBodySize(std::size_t size);

		void	setLimitsExcept(const std::string& option);
		void	setRoot(const std::string& path);
		void	setTimeout(double value);
		// void	setLocation(route Route);
		void	setLocation(const std::string& path, const route& Route);

};
