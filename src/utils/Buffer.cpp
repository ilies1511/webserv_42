#include "Buffer.hpp"

// : _buffer.reserve(INITIAL_SIZE)
Buffer::Buffer(void)
{
	_buffer.reserve(INITIAL_SIZE);
	std::cout << "Cap of _buffer: " << _buffer.capacity() << "\n";
}

Buffer::~Buffer(void) {}

// Methodes -- BEGIN
const char *Buffer::data() const
{
	return _buffer.data();
}

size_t Buffer::size() const
{
	return _buffer.size();
}
// Methodes -- END
