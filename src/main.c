#include <stdio.h>
#include "common.h"

#define ERR_EXIT(m)\
        do\
	{\
	    perror(m);\
            exit(EXIT_FAILURE);\
        }while(0)

int main()
{
	if( getuid() != 0 )//root?
	{
		fprintf(stderr, "miniftpd:must be started as root\n");
		exit(EXIT_FAILURE);
	}
	return 0;  
}
