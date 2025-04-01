#ifndef Core_HPP
#define Core_HPP

#include "webserv.hpp"

class Core
{
	private:

	//OCF -- BEGIN
	public:
		std::vector<TOKEN>			tokenList;
		std::vector<Server>			_servers;
		std::vector<serverConfig>	_server_confs;
		std::vector< struct pollfd>	_pollfds; //Stores FD of Connections
		std::vector<int>			_deferred_close_fds;
	public:
		Core(void);
		~Core(void);
	private:
		Core(const Core& other) = delete;
		Core& operator=(const Core& other) = delete;
	//OCF -- END

	//Methodes -- BEGIN
	public:
		static Core&	getInstance();
		void			poll_loop(void);
		void			cleanup_deferred(void);
		void			add_to_pollfds_Root(int new_fd);
		void			del_from_pollfds_Root(int fd);
		void			ft_closeNclean_Root(int fd);
		pollfd*			getPollFdElement_Root(int fd);
		void			add_to_pollfds_prefilled_Root(pollfd &new_element);
		void			cleanup_deferred_Root(void);
		void			check_connection_timeouts_Root(void);
	//Methodes -- END
};

#endif
