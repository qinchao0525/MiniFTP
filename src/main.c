#include <stdio.h>
#include "common.h"
#include "sysutil.h"
#include "session.h"
#include "str.h"
#include "parseconf.h"
#include "tunable.h"
#include "ftpproto.h"

#define ERR_EXIT(m)\
        do\
	{\
	    perror(m);\
            exit(EXIT_FAILURE);\
        }while(0)

int main()
{
	//list_common();
	//load config file.
	parseconf_load_file(MINIFTP_CONF);

	if( getuid() != 0 )//root?
	{
		fprintf(stderr, "miniftpd:must be started as root\n");
		exit(EXIT_FAILURE);
	}
	session_t sess={0, -1, -1, "","","", NULL, -1, 0,0,0,0, -1, -1, -1, 0, 0, NULL};
	
	sess.bw_upload_rate_max = tunable_upload_max_rate;
	sess.bw_download_rate_max = tunable_download_max_rate;

	signal(SIGCHLD, SIG_IGN);
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
			sess.p_ctrl_fd=conn;
			begin_session(&sess);
		}
		else
			close(conn);
	}
	return 0;  
}
