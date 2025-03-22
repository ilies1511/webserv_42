#ifndef Buffer_HPP
#define Buffer_HPP

#include <iostream>
#include <string>
#include <vector>

class Buffer
{
	private:
		static constexpr size_t INITIAL_SIZE = 4096;
	public:
		std::string _buffer;
		Buffer(void);
		~Buffer(void);
	private:
	//OCF -- BEGIN
		Buffer(const Buffer& other) = delete;
		Buffer& operator=(const Buffer& other) = delete;
	//OCF -- END

	//Methodes -- BEGIN
	public:
		// ssize_t	read(int fd);
		// ssize_t	write(int fd) const;
		// bool	contains_crlf() const;
		// void	consume(size_t n);

		const char*	data() const;
		size_t size() const;
	//Methodes -- ENDs
};

#endif
