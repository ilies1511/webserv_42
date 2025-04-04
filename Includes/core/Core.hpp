#ifndef Core_HPP
#define Core_HPP

#include "webserv.hpp"
#include <atomic>

extern std::atomic<bool> running;

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
		std::vector<int>			_listener_fds;
	public:
		Core(void);
		~Core(void);
	private:
		Core(const Core& other) = delete;
		Core& operator=(const Core& other) = delete;
	//OCF -- END

	//Methodes -- BEGIN
	public:
		void						poll_loop(int argc, char *argv[]);
		void						cleanup_deferred(void);
		std::vector<struct pollfd>&	getPollFds(void);
		void						addPollFd(int new_fd);
		void						addPollFdPrefilled(const pollfd &pfd);
		void						removePollFd(int fd);
		pollfd						*getPollFdElement(int fd);
		//Methodes -- END
	};


void	sig_handler(int);

#endif
