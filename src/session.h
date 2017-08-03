#ifndef _SESSION_H_
#define _SESSION_H_
#include "common.h"
typedef struct session
{
	//uid
	uid_t uid;
	//control connection.
	int ctrl_fd;
	//control conn
	char cmdline[MAX_COMMAND_LINE];
	char cmd[MAX_COMMAND];
	char arg[MAX_ARG];
	//address
	struct sockaddr_in *port_addr;
	int pasv_listen_fd;//pasv mode
	//
	int data_fd;
	//fuzi connect fd 
	int parent_fd;
	int child_fd;
	//file status
	int is_ascii;
	long long restart_pos;
}session_t;


void begin_session(session_t *sess);

#endif /*_SESSION_H_*/
