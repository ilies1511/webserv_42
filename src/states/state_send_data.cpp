#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
// #include "Request.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include "StaticFileHandler.hpp"
#include <map>

void	Connection::send_data(void)
{
	/*
		0. immer event beim POLLFD immer (POLLIN & POLLOUT) initen, statt ueber enable_pollout() zu gehen
		1. CHECK ob _fdConnection revent auf POLLOUT
		2. wenn nein, return
		3. wenn ja, dann send-Vorgang starten: bytes_send = send(_fdConnection, respone_buffer,  respone_buffer.size(), MSG_DONTWAIT)
			--> Checken ob alles geschickt wurde, und erst wenn bytes_send der response.size() entspricht
				dann Connection closen. Wenn nicht, dann position merken, wieder ueber poll gehen und
				da dann wieder ansaetzen und senden;s
				(siehe Client.cpp) --> send()
		4. Close connection
			- close_later: nach jeder Iter cleanup
	*/
	// if !SOCKET_POLLOUT
	// {
	// 	return
	// }
	// printer::debug_putstr("In PRE send", __FILE__, __FUNCTION__, __LINE__);
	printer::debug_putstr("In PRE send File Content", __FILE__, __FUNCTION__, __LINE__);
	if (!check_revent(_fdConnection, POLLOUT))
	{
		printer::debug_putstr("In alo Case", __FILE__, __FUNCTION__, __LINE__);
		return ;
	}
	_last_activity = std::chrono::steady_clock::now();
	// _OutputBuffer._buffer.assign(_current_response.response_inzemaking.begin(), _current_response.response_inzemaking.end());
	_OutputBuffer._buffer = _current_response.response_inzemaking;
	size_t	left_to_send = _OutputBuffer._buffer.length() - (size_t)sent_bytes;
	ssize_t	sent = send(this->_fdConnection, this->_OutputBuffer.data() + \
					sent_bytes, left_to_send, MSG_DONTWAIT);
	if (sent > 0)
	{
		this->sent_bytes = sent_bytes + sent;
		printer::debug_putstr("In Body send", __FILE__, __FUNCTION__, __LINE__);
		std::cout << "Sent Response:\n"
				  << std::string(_OutputBuffer.data() + sent_bytes - sent, (size_t)sent) << "\n";
		this->_OutputBuffer._buffer.clear();
		if (sent_bytes == (ssize_t)_current_response.response_inzemaking.size()) {
			finished_sending = true;
			std::cout << "sent_bytes: " << sent_bytes << ", sent: " << sent << "\n";
			printer::debug_putstr("sent == response_inzemaking.size() 'case'",\
				__FILE__, __FUNCTION__, __LINE__);
			ft_closeNcleanRoot(_fdFile);
			ft_closeNcleanRoot(this->_fdConnection);
			return ;
		}
	} // TODO: 01.04.25 --> mv _OutputBuffer._buffer = _cgi->getOutput(); in CGI part damit handling uebrlall gleich bleibt HOTFIX should be fixed !!!!
	// else if (_cgi.has_value()) {
	// 	//TODO: mv in CGI Part // _OutputBuffer._buffer = _cgi->getOutput();
	// 	const std::string tmp = _cgi->getOutput();
	// 	_OutputBuffer._buffer.assign(tmp.begin(), tmp.end());
	// 	send(this->_fdConnection, this->_OutputBuffer.data(), _OutputBuffer._buffer.size(), 0);
	// 	ft_closeNcleanRoot(this->_fdConnection);
	// 	return;
	// }
	_state = State::SEND;
	// /*
	// 	TODO: 17.03.25
	// 		Partiell send Handling
	// 		Da komme ich nur hier, wenn ich noch was senden muss
	// 		Folgender Block gerade ueberfluessig
	// */
	// printer::debug_putstr("POST in handle_output", __FILE__, __FUNCTION__, __LINE__);
	// /*
	// 	response_buffer ko
	// */
	// // if !finished // Bei langer Response (1GB), viele Calls notwendih
	// // 	return
	// _server.ft_closeNclean(this->_fdFile);
	// _server.ft_closeNclean(this->_fdConnection);
	// printer::debug_putstr("POST in handle_output", __FILE__, __FUNCTION__, __LINE__);
	// return ;
}
