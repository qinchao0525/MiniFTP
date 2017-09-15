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
	char cmdline[MAX_COMMAND_LINE];//a line of cmd
	char cmd[MAX_COMMAND];//cmd
	char arg[MAX_ARG];//value
	//address
	struct sockaddr_in *port_addr;
	int pasv_listen_fd;//pasv mode
	//rate limited
	unsigned int bw_upload_rate_max;
	unsigned int bw_download_rate_max;
	/////////time value for sleep value
	long bw_transfer_start_sec;
	long bw_transfer_start_usec;
	//
	int data_fd;//fd of data transfer
	//fuzi connect fd 
	int parent_fd;
	int child_fd;

	//file status
	int is_ascii;
	long long restart_pos;//for file restore
	//
	char *rnfr_name;
}session_t;


void begin_session(session_t *sess);

#endif /*_SESSION_H_*/
