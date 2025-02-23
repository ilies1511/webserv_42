#ifndef CONNECTION_HPP
#define CONNECTION_HPP


class Connection
{
	private:

	public:
		Connection(void);
		Connection(const Connection& other);
		~Connection(void);
		Connection& operator=(const Connection& other);
};

#endif
