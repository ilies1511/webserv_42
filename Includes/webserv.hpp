#pragma once

//Includes -- BEGIN
#include <iostream>
#include <string>
#include <string.h>
#include <cstring>
#include "Extra/printer.hpp"
//Beej's Buch
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
//Includes -- END

//Defines -- BEGIN
# define MYPORT "3490" // the port users will be connecting to
# define BACKLOG 10 // how many pending connections queue will hold
#define PORT "9034"   // Port we're listening on
//Defines -- END

//Headers -- BEGIN
#include "playground.hpp"
#include "Server.hpp"
//Headers -- END

//FNC-Prototypes -- BEGIN
void	dummy_function(void);
void	src_function(void);
//FNC-Prototypes -- END
