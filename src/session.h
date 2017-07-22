#ifndef _SESSION_H_
#define _SESSION_H_
#include "common.h"
typedef struct session
{
	uid_t uid;
	int ctrl_fd;
	//control conn
	char cmdline[MAX_COMMAND_LINE];
	char cmd[MAX_COMMAND];
	char arg[MAX_ARG];
	//fuzi connect fd 
	int parent_fd;
	int child_fd;
}session_t;


void begin_session(session_t *sess);

#endif /*_SESSION_H_*/
