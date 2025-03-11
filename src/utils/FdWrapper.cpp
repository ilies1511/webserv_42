#include "FdWrapper.hpp"

FdWrapper::FdWrapper(int fd) : _fd(fd)
{}

FdWrapper::~FdWrapper(void)
{
	if (_fd != -1)
	{
		close (_fd);
	}
}

int	FdWrapper::getFD() const
{
	return (_fd);
}


// Move constructor
FdWrapper::FdWrapper(FdWrapper&& other) noexcept : _fd(other._fd) {
    other._fd = -1; // Invalidate the source
}

// Move assignment operator
FdWrapper& FdWrapper::operator=(FdWrapper&& other) noexcept {
    if (this != &other) {
        if (_fd != -1) {
            close(_fd);
        }
        _fd = other._fd;
        other._fd = -1;
    }
    return *this;
}
