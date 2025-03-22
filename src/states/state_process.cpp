#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "printer.hpp"
// #include "StaticFileHandler.hpp"
#include <map>
#include <filesystem>
#include <sstream>

void	Connection::handle_delete(void)
{}

void	Connection::handle_post(void)
{}

void	Connection::generate_autoindex(const std::filesystem::path& dir)
{
	std::ostringstream body;
	body << "<html><body><h1>Index of " << dir.filename() << "</h1><ul>";

	for (const auto& entry : std::filesystem::directory_iterator(dir))
	{
		std::string name = entry.path().filename().string();
		body << "<li><a href=\"" << name << "\">" << name << "</a></li>";
	}
	body << "</ul></body></html>";

	_current_response.status_code = "200";
	_current_response.reason_phrase = "OK";
	_current_response.http_version = "HTTP/1.1";
	_current_response.file_data = body.str();
	_current_response.headers["Content-Type"] = "text/html";
	_current_response.headers["Content-Length"] = std::to_string(_current_response.file_data.size());
	_current_response.headers["Connection"] = "close";
}

bool	Connection::file_exists_and_readable(const std::filesystem::path& p)
{
	return (std::filesystem::exists(p) \
			&& std::filesystem::is_regular_file(p) \
			&& access(p.c_str(), R_OK) == 0);
}

/*
	filled_request._method = "GET";
	filled_request._uri = "/index.html";
	filled_request._version = "HTTP/1.1";
	filled_request._body = "";
	// filled_request.is_finished = true;
	filled_request.is_finished = true;
	filled_request.readFile = true;
	filled_request.filename = "html/index.html";
*/

/*
	// TODO: 22.03.2025 status_code - *.html Mapping --> dynamic and no hardcode
*/
void	Connection::handle_get(void)
{
	std::cout << "\nALLOOOO aus handle_get entry\n";
	// bool res = false;
	// res = std::filesystem::is_directory("var/www/data/files/");
	// // std::filesystem::exists();
	// if (res) {
	// 	std::cout << coloring("\nIs dir !\n", GREEN);
	// }
	// else {
	// 	std::cout << coloring("\nNot dir!\n", RED);
	// }

	// this->request._uri = "var/www/data/files/trier";
	// this->request._uri = "var/www/data/files"; // No slash case
	this->request._uri = "var/www/data/files"; // No slash case

	std::filesystem::path full_path = "/Users/iziane/42/repo_webserv/webserv/" + this->request._uri;
	// std::filesystem::path full_path = std::filesystem::path("/Users/iziane/42/repo_webserv/webserv/") / this->request._uri;

	// std::filesystem::path full_path = "/Users/iziane/42/repo_webserv/webserv/" / this->request._uri;
	// std::filesystem::path full_path = "/Users/iziane/42/repo_webserv/webserv/" + this->request._uri;
	// std::filesystem::path full_path = "/Users/iziane/42/bserv/webserv/" + this->request._uri;
	// std::cout << "\nPRE weakly_canonical - full_path: " << full_path << "\n";
	/*
		Prevents crashes: with fs::canonical() --> throw exception if any part
		of the path didn't exist.
	*/
	full_path = std::filesystem::weakly_canonical(full_path);
	//TODO: 22.03 testen wie nginx das macht -->  "GET /" Request
	std::cout << "\nPOST weakly_canonical - full_path: " << full_path << "\n";
	this->_system_path = full_path;

	//TODO: setStatusCode
	//TODO: prepare_fdFile_param(status_code)
	bool has_trailing_slash = !request._uri.empty() && request._uri.back() == '/';
	// bool autoindex_enabled = true; // TODO: 22.03 mit Steffens Part zsm - LAter via Config
	bool autoindex_enabled = false; // TODO: 22.03 mit Steffens Part zsm - LAter via Config

	if (has_trailing_slash)//nginx assumes this is dir
	{
		std::cout << coloring("In has_trailing_slash()\n", TURQUOISE);
		//Case tritt auf, wenn path nicht existiert
		if (!std::filesystem::exists(full_path))
		{
			// _current_response.status_code = "404";
			_system_path = "errorPages/404.html";
			// prepare_fdFile(); // Setzt READ_FILE und next_state
			prepare_fdFile_param("404");
			return ;
		}
		std::filesystem::path index_file = full_path / "index.html"; // ==> "/Users/iziane/42/repo_webserv/webserv/var/www/data/files/index.html
		if (file_exists_and_readable(index_file)) {
			printer::debug_putstr("file_exists_and_readable(index_file) CASE", __FILE__, __FUNCTION__, __LINE__);
			// _current_response.status_code = "200";
			_system_path = index_file;
			// prepare_fdFile(); // Setzt READ_FILE und next_state
			prepare_fdFile_param("200");
			return ;
		}
		else if (autoindex_enabled) {
			// printer::debug_putstr("In handle_get() trailing case autoindex on", __FILE__, __FUNCTION__, __LINE__);
			//TODO: 22.03.2025 generate_autoindex(full_path);
			// prepare_fdFile(); // Setzt READ_FILE und next_state
			generate_autoindex(full_path);
			_state = State::ASSEMBLE;
			return ;
		}
		else {
			// std::cout << coloring("In handle_get() trailing case - 403 case " \
			// 		+ index_file.string(), CYAN);
			printer::debug_putstr("403 CASE", __FILE__, __FUNCTION__, __LINE__);
			// _current_response.status_code = "403";
			_system_path = "errorPages/403.html";
			prepare_fdFile_param("403"); // Setzt READ_FILE und next_state
			// generate_error_response(_current_response);
			// prepare_fdFile(); // Setzt READ_FILE und next_state
			// generate_error_response("403", "Forbidden");
			return ;
		}
	}
	else ////nginx assumes this is file
	{
		if (std::filesystem::exists(full_path))
		{
			if (std::filesystem::is_regular_file(full_path))
			{
				prepare_fdFile_param("200");
			}
			// else if (std::filesystem::is_directory(full_path))
			// {
			// 	// → nginx 301 Redirect mit `/` anhängen
			// 	_current_response.status_code = "301";
			// 	_current_response.reason_phrase = "Moved Permanently";
			// 	_current_response.headers["Location"] = request._uri + "/";
			// 	_current_response.headers["Content-Length"] = "0";
			// 	_current_response.headers["Connection"] = "close";
			// 	_state = State::ASSEMBLE;
			// 	return;
			// }
			else if (std::filesystem::is_directory(full_path))
			{
				// _current_response.status_code = "301";
				// _current_response.headers["Content-Length"] = "0";
				_current_response.headers["Content-Type"] =  "text/html";
				std::string corrected_uri = request._uri;
				if (!corrected_uri.empty() && corrected_uri.back() != '/')
					corrected_uri += '/';
				_current_response.headers["Location"] = corrected_uri;
				// _current_response.headers["Location"] = request._uri + "/";
				// _current_response.headers["Location"] = "/" + request._uri + "/";
				_current_response.headers["Connection"] = "close";
				_system_path = "html/301.html";
				prepare_fdFile_param("301");
				// _state = State::ASSEMBLE;
				return ;
				// assert (0);
			}
		}
		else
		{
			_system_path = "errorPages/404.html";
			prepare_fdFile_param("404");
			return ;
		}
	}
	/*
	//Case, wenn fullpath mit slash, sprich var/www/data/files/
	if (fullpath.end() == '/') //nginx assumes it's a dir
	{
		if (fullpath.is_dir)
		{
			if (is_index.html here)
			{
				serve index_file;
			}
			else if (autoindex on)
			{
				serve autoindex --> dynamisch generierte Verzeichnis Liste;
			}
			else
			{
				generate_403_forbidden
			}
		}
		else
		{
			generate_404_error;
		}
	}
	else //Case, wenn fullpath ohne slash, sprich var/www/data/files
	{
		//nginx searches for exactly the requested file
		if (file_exists)
		{
			serve_file
		}
		else (check if dir exists)
		{
			301_response
			append '/' to full URI and send it back
		}
		else
		{
			generate_404_erroresonse
		}
	}
	*/
}

//// Old Structur Stand: 20.03
// void	handle_get(void)
// {
// 	std::cout << coloring("In handle_get", PURPLE);
// 	if (std::filesystem::is_directory(this->_system_path)) {
// 		assert(0);
// 		// if (indexfile_here)
// 		// {
// 		// 	setpath2indexfile();
// 		// }
// 		// else if (autoindex_here) {

// 		// }
// 		// else {
// 		// 	error (404);
// 		// }
// 		// here: execute_file()
// 	} else {
// 		// 1. check if file --> if not, respone 404
// 		if (!std::filesystem::exists(_system_path))
// 		{
// 			prepare_ErrorFile(); //404
// 			return ;
// 		}
// 		// if (cgi)
// 		if (0)
// 		{
// 			//do_cgi
// 		}
// 		else
// 		{
// 			//do_normalFile
// 			/*
// 				TODO:	funcs sollen als params _state und _next_state haben und im
// 				function-body die jeweiligen Attribute darauf setzten
// 				davor if (request.readFile && !(this->_current_response.FileData))
// 			*/
// 			prepare_fdFile();
// 			/*
// 				prepare_fdFile() {
// 					do_normalFile //vielleicht damit anfangen
// 					if () {
// 						_fdFile = open();
// 						if (_fdFile)
// 						generate_error_response
// 						}
// 						check if file here (open)
// 						READ_FILE STATE
// 				}
// 			*/
// 		}
// 	}
// }

void	Connection::methode_handler(void)
{
	if (this->request._method == "GET")
	{
		std::cout << coloring("in methode_handler GET", PURPLE);
		handle_get();
	}
	else if (this->request._method == "POST")
	{
		std::cout << coloring("in methode_handler", PURPLE);
		handle_post();
	}
	else if (this->request._method == "DELETE")
	{
		handle_delete();
	}
	else {
		assert(0);
		//TODO: handle error for wrong methode
	}
}

void	Connection::entry_process(void)
{
	/*
	TODO: PART 1
		0. check if request was invalid --> parser sets status-code xy : read file, send to clieant (assemble_resonse)
		1. Allgemeine Schritte:
			- Path Variable zusammenbauen --> Config + Request = FilePath
			- if location is a redirect
				--> just send 30x + specific Header
			- Verifizierung ob Meth
	*/

	/*
		PART 2:
			methode_handler()
	*/

	request._method = "GET";
	request._uri = "/index.html";
	request._version = "HTTP/1.1";
	request._body = "";
	// ed_request.is_finished = true;
	request.is_finished = true;
	request.readFile = true;
	// request.filename = "html/index.html";
	// filled_request._headers = ;
	methode_handler();
	return ;
}











void	Connection::connection_process(void)
{
	std::cout << "Straight outta RECV\n";

	/*
		Dieser Block verstehe ich noch nicht gut genug
			- Alles von CGI muss in Pipe geschrieben und dann auch davon gelesen werden
			- GET-request muss FILE lesen (angegeben durch "uri")
			- Status Code muss man von FILE lesen (z.b 404.html)

	*/
	/*
		// Der Process muss auf Grundlage des fertigen Requests, zu
		ermittlern ob eine File geoffnet werden muss
		Logik fuer Faehigkeit, File zu lesen, in Buffer zu speichern, Body

		Response String Stueck fuer Stueck zusammenbauen und dabei Body lesen
		und dann Resonse generieren  --> siehe void Response::finish_up(void)
		Logik eigenen Buffer speichern.

		Z.B:

		response_str = "HTTP/1.1 200 OK\r\n" --> Header
				"Content-Type: text/plain\r\n"
				"Content-Length: 13\r\n\r\n"
				"Hello World!\n"; --> das was man aus der File liest

		Ueberlegungstatt OutputBuffer, std::string zu verwenden
			--> Gefahr: c_str am Ende nullterminiert, waehrend std::strings nicht

	*/
	// READ_FILE KOENNRE auch eine Sub-State sein.
	printer::debug_putstr("In Process State", __FILE__, __FUNCTION__, __LINE__);

	entry_process();
	return ;
	/*
		0. check if request was invalid --> parser sets status-code xy : read file, send to clieant (assemble_resonse)
		1. Allgemeine Schritte:
			- Path Variable zusammenbauen --> Config + Request = FilePath
			- if location is a redirect
				--> just send 30x + specific Header
			- Verifizierung ob Methode fuer directory valide, if inavlid assemble_response with appropriate response
		2. Method Handler:
			if (request._method  == GET)
			{
				handle_get();
			}
			- GET-Handler: handle_get();
				if (dir)
					if (indexfile_here)
					{
						setpath2indexfile();
					}
					else if (autoindex_here) {

					}
					else {
						error (404);
					}
				here: execute_file()
				if (cgi) {
					do_cgi
				}
				else {
					do_normalFile //vielleicht damit anfangen --> 	if (request.readFile && !(this->_current_response.FileData))
						if () {
							_fdFile = open();
							if (_fdFile)
								generate_error_response
						}
						check if file here (open)
					READ_FILE STATE
					return ;
				}

			- POST-Handler: handle_POST();

	*/

	//_current_response.process_request(this->request);

	//hier davor  --> get_file();
}
