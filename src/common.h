#ifndef _COMMON_H_
#define _COMMON_H_

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <pwd.h>
#include <ctype.h>
#include <shadow.h>
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_EXIT(m)\
        do\
	{\
	    perror(m);\
            exit(EXIT_FAILURE);\
        }while(0)

#define MAX_COMMAND_LINE 1024
#define MAX_COMMAND 32
#define MAX_ARG 1024
#define MINIFTP_CONF "miniftpd.conf"

#endif
