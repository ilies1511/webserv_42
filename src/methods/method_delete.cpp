#include "Connection.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include <map>
#include <filesystem>
#include <sstream>

void	Connection::is_dir_Case(void)
{
	const bool path_exists = std::filesystem::exists(_full_path);

	if (!std::filesystem::is_directory(_full_path)) {
		set_full_status_code(path_exists ? 409 : 404);
		return;
	}
	if (rmdir(_full_path.c_str()) == 0) {
		set_full_status_code(204);
	}
	else
	{
		switch (errno) {
			case ENOTEMPTY:	set_full_status_code(409); break;
			case EACCES:	// Fallthrough
			case EPERM:		set_full_status_code(403); break;
			case ENOENT:	set_full_status_code(404); break;
			default:		set_full_status_code(500);
		}
		errno = 0;
	}
}

void Connection::is_file_Case()
{
	// Prüfe explizit auf Datei
	if (std::filesystem::is_directory(_full_path)) {
		set_full_status_code(409);
		return;
	}
	if (unlink(_full_path.c_str()) == 0) {
		set_full_status_code(204);
	}
	else
	{
		switch (errno) {
			case ENOENT:	set_full_status_code(404); break;
			case EACCES:	// Fallthrough
			case EPERM: 	set_full_status_code(403); break;
			case EISDIR:	set_full_status_code(409); break;
			default:		set_full_status_code(500);
		}
		errno = 0;
	}
}

/*
	Testcases:
		curl -v -X DELETE http://localhost:9035/files/sample.txt
		curl -v -X DELETE http://localhost:9035/files/empty_dir/
		curl -v -X DELETE http://localhost:9035/files/nonexistent.txt

		curl -v -X DELETE http://localhost:9035/var/www/data/files/
*/
void	Connection::handle_delete(void)
{
	P_DEBUG("ALLOOOO aus handle_delete entry");

	_full_path = _absolute_path;
	std::cout << "_full_path: " << _absolute_path << "\n";
	try
	{
		bool has_trailing_slash = \
			!request.uri->path.empty() && request.uri->path.back() == '/';

		if(has_trailing_slash)
		{
			is_dir_Case();
		}
		else if (!has_trailing_slash) {
			is_file_Case();
		}
		else {
			assert(0);
			set_full_status_code(404);
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		set_full_status_code(404); // Ungültiger Pfad
	}
	catch(const std::exception& e)
	{
		P_DEBUG("alo");
		printer::LogException(e, __FILE__, __FUNCTION__, __LINE__);
		set_full_status_code(500);
	}
}


/*
	Ueberpruefen ob Ressource existiert prüfe zuerst, ob die Ressource
	existiert, ob sie gelöscht werden kann (Datei oder leeres Verzeichnis)
	und liefere den passenden Status (204, 404, 403 oder 409).
	if (!(ressource_exists))
	{
		set_full_status_code(404);
	}
	if (is_file)
	{
		if (!unlink(path2file)) {
			set_full_status_code(403);
		}
	}
	else if(is_dir)
	{
		if (not_empty) {
			set_full_status_code(409);
			return ;
		}
		if (rmdir(path2dir) failed) {
			set_full_status_code(403);
			return ;
		}
	}
*/
