#include "privsock.h"
#include "common.h"
#include "sysutil.h"
//init
void priv_sock_init(session_t *sess)
{
	int sockfds[2];//socket pair/ shared pair like pipe.
	if(socketpair(PF_UNIX, SOCK_STREAM, 0, sockfds)<0)//unix domain protocol
		ERR_EXIT("socketpair");
	
	sess->parent_fd = sockfds[0];
	sess->child_fd = sockfds[1];

		
}
//close socket fd
void priv_sock_close(session_t *sess)
{
	if(sess->child_fd != -1)
	{
		close(sess->child_fd);
		sess->child_fd = -1;
	}
	if(sess->parent_fd != -1)
	{
		close(sess->parent_fd);
		sess->parent_fd = -1;
	}
}
//set parent context
void priv_sock_set_parent_context(session_t *sess)
{
	if(sess->child_fd != -1)
	{
		close(sess->child_fd);//parents do not need child fd.
		sess->child_fd = -1;
	}

}
//set chile context
void priv_sock_set_child_context(session_t *sess)
{
	if(sess->parent_fd != -1)
	{
		close(sess->parent_fd);//chile do not need parentd fd.
		sess->parent_fd = -1;
	}
}

//send cmd from ftp server to nobody
void priv_sock_send_cmd(int fd, char cmd)
{
	int ret;
	ret=writen(fd, &cmd, sizeof(cmd));//send cmd to nobody process.
	if(ret!=sizeof(cmd))
	{
		fprintf(stderr, "priv_sock_send_cmd error\n");
		exit(EXIT_FAILURE);
	}
}
//nobody get cmd from child.
char priv_sock_get_cmd(int fd)
{
	char res;//cmd
	int ret;
	ret = readn(fd, &res, sizeof(res));
	if(ret!=sizeof(res))
	{
		fprintf(stderr, "priv_Sock_get_cmd error\n");
		exit(EXIT_FAILURE);
	}
	return res;
}
//send cmd from nobody to ftp-server.
void priv_sock_send_result(int fd, char res)
{
	int ret;
	ret=writen(fd, &res, sizeof(res));//send cmd
	if(ret!=sizeof(res))
	{
		fprintf(stderr, "priv_sock_send_result error\n");
		exit(EXIT_FAILURE);
	}
}
//get result from nobody
char priv_sock_get_result(int fd)
{
	char res;
	int ret;
	ret = readn(fd, &res, sizeof(res));//get cmd
	if(ret!=sizeof(res))
	{
		fprintf(stderr, "priv_Sock_get_result error\n");
		exit(EXIT_FAILURE);
	}
	return res;
}


//send int number(port)
void priv_sock_send_int(int fd, int the_int)//port
{
	int ret;
	ret=writen(fd, &the_int, sizeof(the_int));//send port
	if(ret!=sizeof(the_int))
	{
		fprintf(stderr, "priv_sock_send_int error\n");
		exit(EXIT_FAILURE);
	}
}
int priv_sock_get_int(int fd)
{
	int  the_int;
	int ret;
	ret = readn(fd, &the_int, sizeof(the_int));//get port
	if(ret!=sizeof(the_int))
	{
		fprintf(stderr, "priv_Sock_get_int error\n");
		exit(EXIT_FAILURE);
	}
	return the_int;
}
//send char (ip)
void priv_sock_send_buf(int fd, const char* buf, unsigned int len)
{
	priv_sock_send_int(fd, (int)len);//send lenght of string.
	int ret = writen(fd, buf, len);//read length chars in fd.
	if(ret!=(int)len)
	{
		fprintf(stderr, "priv_sock_send_buf error\n");
		exit(EXIT_FAILURE);
	}
}
void priv_sock_recv_buf(int fd, char *buf,unsigned int len)
{
	unsigned int recv_len = (unsigned int)priv_sock_get_int(fd);
	if(recv_len > len)
	{
		fprintf(stderr, "priv_sock_recv_buf error\n");
		exit(EXIT_FAILURE);
	}
	
	int ret=readn(fd, buf, recv_len);
	if(ret!=(int)recv_len)
	{
		fprintf(stderr, "priv_sock_recv_buf error\n");
		exit(EXIT_FAILURE);
	}
}
//send fd
void priv_sock_send_fd(int sock_fd, int fd)
{
	send_fd(sock_fd, fd);
}
//recv fd.
int priv_sock_recv_fd(int sock_fd)
{
	return recv_fd(sock_fd);
}
