#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include <map>
#include <filesystem>
#include <sstream>

void	Connection::handle_delete(void)
{
	_full_path = "/Users/iziane/42/repo_webserv/webserv/" + request.uri->path;
	_full_path = std::filesystem::weakly_canonical(_full_path);

	/*	TODO: Eearly Exit mit Steffens Part
		if (_server._config.getLocation()["/"].getAllowedMethods())
		{
		}
	*/

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

	// Erzeuge den vollständigen Pfad aus Root und Request URI.
	// Wichtig: request.uri sollte hier nur den URI-Teil enthalten, z. B. "/files/sample.txt"
	// _full_path = std::filesystem::weakly_canonical(_server_root + request.uri->path);

	auto root = "/Users/iziane/42/repo_webserv/webserv/";
	_full_path = std::filesystem::weakly_canonical(root + request.uri->path);
	std::cout << "DELETE - Full path: " << _full_path << "\n";

	if (!std::filesystem::exists(_full_path)) {
		set_full_status_code(404); // Ressource existiert nicht
		return;
	}

	// is_file Case
	if (std::filesystem::is_regular_file(_full_path)) {
		if (unlink(_full_path.c_str()) == 0) {
			// Löschung erfolgreich: 204 No Content
			set_full_status_code(204);
		} else {
			// Löschen schlug fehl (z.B. Berechtigungsproblem): 403 Forbidden
			set_full_status_code(403);
		}
		return;
	}

	// is_dir Case:
	if (std::filesystem::is_directory(_full_path))
	{
		if (!std::filesystem::is_empty(_full_path)) {
			// Verzeichnis ist nicht leer, daher kann es nicht gelöscht werden – Conflict
			set_full_status_code(409);
		} else {
			if (rmdir(_full_path.c_str()) == 0) {
				set_full_status_code(204);
			} else {
				set_full_status_code(403);
			}
		}
		return;
	}
	// Falls die Ressource weder Datei noch Verzeichnis ist, gib 404 zurück:
	set_full_status_code(404);
}
