#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
// #include "Request.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include "StaticFileHandler.hpp"
#include <map>

void	Connection::write_file(void)
{
	/*
		Dafuer fuer CGI, um in Pipes zu schreiben --> CGI spezifisch:

		Kontext: z.B ein POST-Request
		state = State::PROCESS;
		das folgende gehoert in den PROCESS Statw
		0. Uri zu path Umwandlung, Method und cgi Determinierung usw.
		1. CGI skript starten mit fork, dub etc, sprich minishell exec

			pipe(pipe_in);
			pipe(pipe_out);
			->
			fork()
			->
			CHILD:
			dub2(pipe_in[0], STDIN);
				pipe_in ist dafuer da, den Request-Body als STDIN zu mappen
					Das CGI Skript verwendet genau diesen Input um auszufuehren
				pipe_out ist dafuer da, den Output des Skript in STDOUT zu mappn
					 --> output des Skripts statt in STDOUt in pipe_out
					 	 gespeichert, also quasi die ganze HTTP-REesponse ist in
						 Pipe_out gespeichert, was wir dann wieder mittels read in
						 ein Buffer (z.B OutBuffer reinlesen), was wiederrum
						 verwenden wird um gesendet zu werden (Erklaerung zu post execve)


			close(pipe_in[0]);
			close(pipe_in[1]);
			dup2(pipe_out[1], STDOUT);
			close(pipe_out[0]);
			close(pipe_out[1]);
			->
			execve();

			Im Parent : PARENT(webserv):
			close(pipe_in[0]);
			close(pipe_out[1]);
			->
			Kontext: wie gekommen POST-Request und das CGI liest die Daten vom
						stdin, in das davor den Request Body schreiben. --> nach dem forken
			state = State::WRITE;
			das folgende gehoert in den WRITE State
			write(pipe_in[1], request_body, buffer_size);
			if (alles geschrieben)
			{
				close(pipe_in[1])
				state = was auch immer Sinn macht --> State eben dafuer praktisch sich non-konform eine Posiotion zu merken oder hinzuwechseln
			}
			->
			Der Output des ausgefuehrten CGI-Skripts und damit einhergehend dessen
			Output (vom Skript) wird in die pipe_out reingeschrieben.
			In diesem Schritt lesen wir nun aus jener pipe mittels read in den
			response_buffer
			state = State::READ
			das folgende gehoert in den READ:
			read_returnval = read(pipe_out[0], response_buffer, buffer_size);
			if (read_returnval <= 0)
			{
				if (read_returnval == 0)
				{
					close(pipe_out[0])
					fertig, also READ_STATE verlassen und auf PROCESS/EXECUTE
					return
				}
			}
			siehe auch webserv/www/eval/post/upload.py --> Beispiel fuer CGI Skript
			->
			send(socket_fd, response_buffer, buffer_size);
	*/
}
