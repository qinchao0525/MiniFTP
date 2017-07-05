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
	
	int listenfd=tcp_server(NULL, 5188);
	int conn;
	pid_t pid;

	while(1)
	{
		conn=accept_timeout(listenfd, NULL, 0);
		if(conn==-1)
			ERR_EXIT("accept_timeout");
		pid=fork();
		if(pid==-1)
			ERR_EXIT("fork");

		if(pid==0)
		{
			close(listenfd);
			begin_session(conn);
		}
		else
			close(conn);
	}
	return 0;  
}
