#pragma once

#include "route.hpp"

class serverConfig {
	private:
		std::size_t					 		_port;
		std::vector<std::string>		    _server_name;
        std::map<size_t, std::string>    	_error_pages;
        std::size_t							_client_max_body_size;

        bool							    _GETOption;
        bool								_POSTOption;
       	std::string							_root;
        std::vector<route>					_location;


	public:
		explicit serverConfig(const size_t port);
		~serverConfig();

		//getters
		std::size_t						getPort() const;
		std::vector<std::string>		getServerName() const;
		std::map<size_t, std::string>	getErrorPages() const;
		std::size_t						getClientMaxBodySize() const;

		bool							getGETOption() const;
		bool							getPostOption() const;
		std::string						getRoot() const;
		std::vector<route>				getLocation() const;

		//setters
		void	setPort(std::size_t port);
		void	setServerName(const std::vector<std::string>& serverName);
		void	setErrorPages(std::size_t errorNbr, const std::string& path);
		void	setClientMaxBodySize(std::size_t size);

		void	setGETOption(bool option);
		void	setPOSTOption(bool option);
		void	setRoot(std::string& path);
		void	setLocation(route Route);

};
