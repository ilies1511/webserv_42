#include "webserv.hpp"

#include <netdb.h>

void play(char **args)
{
	struct addrinfo	*result;
	int error = getaddrinfo(args[1], NULL, NULL, &result);
	if(error == 0)
	{
		struct addrinfo	*item = result;
		while(item)
		{
			char hostname[NI_MAXHOST] = "";
			// Namen extrahieren, wenn möglich
			error = getnameinfo(item->ai_addr, item->ai_addrlen,
			hostname, NI_MAXHOST, NULL, 0, 0);
			if(error == 0)
			{
				std::cout << hostname;
			}
			//	17.2 Adressen 561, IP-Adresse als String extrahieren, wenn möglich
			error |= getnameinfo(item->ai_addr, item->ai_addrlen,
			hostname, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if(error == 0)
			{
				std::cerr << " " << hostname << "\n";
			}
			else
			{
				std::cerr << args[1] << " : Fehler in getnameinfo().\n";
			}
			item = item->ai_next;
		}
		freeaddrinfo(result);
	}
	else
	{
		std::cout << args[1] << " kann nicht ermittelt werden.\n";
	}
}
