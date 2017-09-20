#include "common.h"
#include "session.h"
#include "ftpproto.h"
#include "privparent.h"
#include "privsock.h"
#include <pwd.h>

void begin_session(session_t *sess)
{

	//unix domain connection
	/*int sockfds[2];
	if(socketpair(PF_UNIX, SOCK_STREAM, 0, sockfds)<0)
		ERR_EXIT("socketpair");
	*/	
	priv_sock_init(sess);

	pid_t pid;
	pid=fork();
	if(pid<0)
		ERR_EXIT("fork");
	if(pid==0)
	{
		//ftp service process
		/*close(sockfds[0]);
		sess->child_fd=sockfds[1];*/
		priv_sock_set_child_context(sess);
		sess->ctrl_fd=priv_sock_recv_fd(sess->child_fd);
		handle_child(sess);
	}
	else//nobody process
	{
		
		//change parent process to nobody process.
		priv_sock_set_parent_context(sess);
		priv_sock_send_fd(sess->parent_fd, sess->p_ctrl_fd);
		/*
		close(sockfds[1]);
		sess->parent_fd=sockfds[0];*/
		handle_parent(sess);
	}
}
