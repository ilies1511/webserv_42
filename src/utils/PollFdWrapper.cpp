#include "PollFdWrapper.hpp"

PollFdWrapper::PollFdWrapper(int fd, short events)
	: _fd_wrapper(fd) {
	_pollfd.fd = fd;
	_pollfd.events = events;
	_pollfd.revents = 0;
}

PollFdWrapper::PollFdWrapper(const struct pollfd& pfd)
	: _fd_wrapper(pfd.fd), _pollfd(pfd)
{}

PollFdWrapper::~PollFdWrapper(void)
{}

// struct pollfd	*PollFdWrapper::data()
// {
// 	return (&_pollfd);
// }

short PollFdWrapper::revents() const
{
	return (_pollfd.revents);
}

int	PollFdWrapper::fd() const
{
	return (_fd_wrapper.getFD());
}

const struct pollfd& PollFdWrapper::get_pollfd() const
{
	return (_pollfd);
}

PollFdWrapper::PollFdWrapper(PollFdWrapper&& other) noexcept
    : _fd_wrapper(std::move(other._fd_wrapper)), _pollfd(other._pollfd) {
    other._pollfd.fd = -1; // Invalidate the moved-from object's fd
}


// In PollFdWrapper.cpp
PollFdWrapper& PollFdWrapper::operator=(PollFdWrapper&& other) noexcept {
    if (this != &other) {
        _fd_wrapper = std::move(other._fd_wrapper); // Uses FdWrapper's move assignment
        _pollfd = other._pollfd;
        other._pollfd.fd = -1; // Invalidate the source
    }
    return *this;
}
