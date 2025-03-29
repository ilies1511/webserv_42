#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include <map>
#include <filesystem>
#include <sstream>

// //Working Backup without errno handling
// void	Connection::is_dir_Case(void)
// {
// 	if (!std::filesystem::exists(_full_path)) {
// 		P_DEBUG("2nd 404 in is_dir_Case");
// 		// set_full_status_code(409);
// 		set_full_status_code(404);
// 		return ;
// 	}
// 	if (!std::filesystem::is_empty(_full_path)) {
// 		// Verzeichnis ist nicht leer, daher kann es nicht gelöscht werden – Conflict
// 		set_full_status_code(409);
// 	}
// 	else {
// 		if (rmdir(_full_path.c_str()) == 0) {
// 			set_full_status_code(204);
// 		}
// 		else {
// 			//TODO: add more precise error checks with errno
// 			set_full_status_code(403);
// 		}
// 	}
// }

void	Connection::is_dir_Case(void)
{
	// Prüfe explizit auf Verzeichnis (auch wenn Slash vorhanden ist)
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

// //Working Backup without errno handling
// void	Connection::is_file_Case(void)
// {
// 	if (!std::filesystem::exists(_full_path)) {
// 		P_DEBUG("3nd 404 in is_file_Case");
// 		set_full_status_code(404);
// 		return ;
// 	}
// 	if (std::filesystem::is_regular_file(_full_path))
// 	{
// 		if (unlink(_full_path.c_str()) == 0) {
// 			// Löschung erfolgreich: 204 No Content
// 			set_full_status_code(204);
// 		}
// 		else {
// 			//TODO: add more precise error checks with errno
// 			// Löschen schlug fehl (z.B. Berechtigungsproblem): 403 Forbidden
// 			set_full_status_code(403);
// 		}
// 	}
// 	else if (std::filesystem::is_directory(_full_path))
// 	{
// 		P_DEBUG("conflict case");
// 		set_full_status_code(409);
// 		return ;
// 	}
// 	// if (unlink(_full_path.c_str()) == 0) {
// 	// 	// Löschung erfolgreich: 204 No Content
// 	// 	set_full_status_code(204);
// 	// } else {
// 	// 	//TODO: add more precise error checks with errno
// 	// 	// Löschen schlug fehl (z.B. Berechtigungsproblem): 403 Forbidden
// 	// 	set_full_status_code(403);
// 	// }
// }

/*
	Testcases:
		curl -v -X DELETE http://localhost:9035/files/sample.txt
		curl -v -X DELETE http://localhost:9035/files/empty_dir/
		curl -v -X DELETE http://localhost:9035/files/nonexistent.txt

		curl -v -X DELETE http://localhost:9035/var/www/data/files/
*/
void	Connection::handle_delete(void)
{
	/*	TODO: Eearly Exit mit Steffens Part
		if (_server._config.getLocation()["/"].getAllowedMethods())
		{
		}
	*/

	// Erzeuge den vollständigen Pfad aus Root und Request URI.
	// Wichtig: request.uri sollte hier nur den URI-Teil enthalten, z. B. "/files/sample.txt"
	// _full_path = std::filesystem::weakly_canonical(_server_root + request.uri->path);

	auto root = "/Users/iziane/42/repo_webserv/webserv/";
	_full_path = std::filesystem::weakly_canonical(root + request.uri->path);
	std::cout << "DELETE - Full path: " << _full_path << "\n";

	P_DEBUG("alo");
	try
	{
		bool has_trailing_slash = \
			!request.uri->path.empty() && request.uri->path.back() == '/';
		// if (!std::filesystem::exists(_full_path)) {
		// 	std::cout << "In doesnt exist  Case\n";
		// 	P_DEBUG("1er 404");
		// 	set_full_status_code(404); // Ressource existiert nicht
		// 	return;
		// }
		if(has_trailing_slash)
		{
			is_dir_Case();
		}
		else if (!has_trailing_slash) {
			is_file_Case();
		}
		// else if (std::filesystem::is_regular_file(_full_path)) {
		// 	std::cout << "In is_regular_file() Case\n";
		// 	is_file_Case();
		// }
		// else if (std::filesystem::is_directory(_full_path)) {
		// 	std::cout << "In is_directory() Case\n";
		// 	is_dir_Case();
		// }
		// // Falls die Ressource weder Datei noch Verzeichnis ist, gib 404 zurück:
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
