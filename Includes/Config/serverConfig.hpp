#pragma once

#include <unordered_map>
#include <iostream>
#include "route.hpp"

// path of this file: Includes/Config/serverConfig.hpp
class serverConfig {
	private:
		std::string								_ip; // default 0.0.0.0
		std::size_t					 			_port; // default 80
		std::vector<std::string>				_server_name;
        std::unordered_map<size_t, std::string>	_error_pages;
        std::size_t								_client_max_body_size; // in bytes

		// std::vector<std::string>				_limit_except; // allowed methods
       	std::string								_root;
		std::string								_index;
		double									_timeout; // in seconds
		std::unordered_map<std::string, route>	_location; // getRoute(const std::string& Route) to access Route;



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

		// std::vector<std::string>		        getLimitsExcept() const;
		std::string						        getRoot() const;
		std::string								getIndex() const;
		double									getTimeout() const;
		std::unordered_map<std::string, route>	getLocation() const;
		route&									getRoute(const std::string& Route);
		//setters
		void	setIP(const std::string& ip);
		void	setPort(std::size_t port);
		void	setServerName(const std::string& serverName);
		void	setErrorPages(std::size_t errorNbr, const std::string& path);
		void	setClientMaxBodySize(std::size_t size);

		// void	setLimitsExcept(const std::string& option);
		void	setRoot(const std::string& path);
		void	setIndex(const std::string& index);
		void	setTimeout(double value);
		void	setLocation(const std::string& path, const route& Route);

};
