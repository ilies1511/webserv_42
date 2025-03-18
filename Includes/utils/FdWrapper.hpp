#ifndef FdWrapper_HPP
#define FdWrapper_HPP

#include <iostream>
#include <string>
#include <unistd.h>
#include "printer.hpp"
#include <memory>

class FdWrapper
{
	private:
		int	_fd;
	public:
	//OCF -- BEGIN
		FdWrapper& operator=(const FdWrapper& other) = delete;
		// Implement move operations
		FdWrapper(FdWrapper&& other) noexcept;
		FdWrapper& operator=(FdWrapper&& other) noexcept;
		explicit FdWrapper(int fd = -1);
		~FdWrapper(void);
	private:
		FdWrapper(const FdWrapper& other) = delete;
	//OCF -- END

	//Methodes -- BEGIN
	public:
		int	getFD() const;
	//Methodes -- END
};
#endif
