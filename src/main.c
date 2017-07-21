#include <stdio.h>
#include "common.h"
#include "sysutil.h"
#include "session.h"
#include "str.h"
#include "parseconf.h"

#define ERR_EXIT(m)\
        do\
	{\
	    perror(m);\
            exit(EXIT_FAILURE);\
        }while(0)

int main()
{
	parseconf_load_file("miniftpd.conf");
	if( getuid() != 0 )//root?
	{
		fprintf(stderr, "miniftpd:must be started as root\n");
		exit(EXIT_FAILURE);
	}
	session_t sess={-1, "","","", -1, -1};
	
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
			sess.ctrl_fd=conn;
			begin_session(&sess);
		}
		else
			close(conn);
	}
	return 0;  
}
