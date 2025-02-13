#include "webserv.hpp"
#include <netdb.h>
//Bejee Specefic



void playground::play(char **args)
{
	Log	log("webserv-playground");
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
				log.complain("ERROR", static_cast<std::string>(args[1]) + " : Fehler in getnameinfo().\n");
				// std::cerr << args[1] << " : Fehler in getnameinfo().\n";
			}
			item = item->ai_next;
		}
		freeaddrinfo(result);
	}
	else
	{
		log.complain("ERROR", static_cast<std::string>(args[1]) + " kann nicht ermittelt werden", __FILE__, __FUNCTION__, __LINE__);
		// std::cout << args[1] << " kann nicht ermittelt werden.\n";
	}
}

int	playground::beej_chapter5(int argc, char **argv)
{
	struct addrinfo	hints;
	struct addrinfo	*res;
	struct addrinfo	*p;
	int				status;
	char			ipstr[INET6_ADDRSTRLEN];

	(void)argc;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return 2;
	}
	printf("IP addresses for %s:\n\n", argv[1]);
	for (p = res; p != NULL; p = p->ai_next)
	{
		void	*addr; //entweder in6_addr (IPv6) oder in_addr (IPv4)
		char	*ipver;

		// get the pointer to the address itself,
		// different fields in IPv4 and IPv6:
		if (p->ai_family == AF_INET) // IPv4
		{
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = (char *)"IPv4"; //had to be explicitly casted
		}
		else // IPv6
		{
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = (char *)"IPv6";
		}
		// convert the IP to a string and print it:
		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr); // inet_ntop -- Internet address manipulation routines (network format (struct in_addr) to presentation format)
		printf("  %s: %s\n", ipver, ipstr);
	}
	freeaddrinfo(res); // free the linked list
	return (1511);
}
