#ifndef POLLFDWRAPPER_HPP
#define POLLFDWRAPPER_HPP

#include "FdWrapper.hpp"
#include <poll.h>

class PollFdWrapper
{
private:
	FdWrapper _fd_wrapper;
	struct pollfd _pollfd;

public:
	explicit PollFdWrapper(int fd, short events);
	explicit PollFdWrapper(const struct pollfd& pfd);
	~PollFdWrapper();
	PollFdWrapper& operator=(const PollFdWrapper&);
	// Move-Semantik
	PollFdWrapper(PollFdWrapper&& other) noexcept;
	PollFdWrapper& operator=(PollFdWrapper&& other) noexcept;
	// Zugriffsmethoden
	const struct pollfd& get_pollfd() const;
	int fd() const;
	short revents() const;
	// Kopieren verbieten
	PollFdWrapper(const PollFdWrapper&) = delete;
};

#endif
