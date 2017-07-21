#include "ftpproto.h"
#include "sysutil.h"
#include "str.h"

void handle_child(session_t *sess)
{
	writen(sess->ctrl_fd, "220 miniftpd 0.1\r\n", strlen("220 miniftpd 0.1\r\n"));//welcome page
	int ret;
	while(1)
	{
		memset(sess->cmdline, 0, sizeof(sess->cmdline));
		memset(sess->cmd, 0, sizeof(sess->cmd));
		memset(sess->arg, 0, sizeof(sess->arg));
		ret=readline(sess->ctrl_fd, sess->cmdline, MAX_COMMAND_LINE);
		if(ret==-1)
			ERR_EXIT("readline");
		else if(ret==0)
			exit(EXIT_SUCCESS);
		printf("cmdline=[%s]\n", sess->cmdline);
		//remove \r\n
		str_trim_crlf(sess->cmdline);
		printf("cmdline=[%s]\n", sess->cmdline);
		//parsing ftp command and args
		str_split(sess->cmdline, sess->cmd, sess->arg, ' ');
		printf("cmd=[%s], arg=[%s]\n", sess->cmd, sess->arg);
		//process command

	}
}
