#include "privparent.h"
#include "session.h"
#include "privsock.h"
#include "tunable.h"
#include "sysutil.h"

/*int capset(cap_user_header_t hdrp, cap_user_data_t datap)
{
	return syscall(__NR_capset, hdrp, datap);
}
*/
static void privop_pasv_get_data_sock(session_t *sess)//binding 20 and connecting
{
	unsigned short port = (unsigned short)priv_sock_get_int(sess->parent_fd);
	char ip[16]={0};
	priv_sock_recv_buf(sess->parent_fd, ip, sizeof(ip));
	
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);

	int fd=tcp_client(20);//created fd.
	if(fd==-1)
	{
		priv_sock_send_result(sess->parent_fd, PRIV_SOCK_RESULT_BAD);
		return;
	}
	if(connect_timeout(fd, &addr, tunable_connect_timeout) < 0)
	{
		close(fd);
		priv_sock_send_result(sess->parent_fd, PRIV_SOCK_RESULT_BAD);
		return ;
	}
	
	priv_sock_send_result(sess->parent_fd, PRIV_SOCK_RESULT_OK);
	priv_sock_send_fd(sess->parent_fd, fd);//send fd to server.
	close(fd);
}
static void privop_pasv_active(session_t *sess)
{
	int active;
	if(sess->pasv_listen_fd != -1)
		active = 1;
	else
		active = 0;
	priv_sock_send_int(sess->parent_fd, active);
}
static void privop_pasv_listen(session_t *sess)
{
	char ip[16]={0};
	getlocalip(ip);

	sess->pasv_listen_fd=tcp_server(ip, 0);
	struct sockaddr_in addr;
	socklen_t addrlen=sizeof(addr);
	if( getsockname(sess->pasv_listen_fd, (struct sockaddr*)&addr, &addrlen) < 0)
		ERR_EXIT("getsockname");
	
	unsigned short port=ntohs(addr.sin_port);
	priv_sock_send_int(sess->parent_fd, port);
	
}
static void privop_pasv_accept(session_t *sess)
{
		int fd = accept_timeout(sess->pasv_listen_fd, NULL, tunable_accept_timeout);
		close(sess->pasv_listen_fd);
		if(fd==-1)//create fd failure
		{
			priv_sock_send_result(sess->parent_fd, PRIV_SOCK_RESULT_BAD);
			return ;
		}
		
		priv_sock_send_result(sess->parent_fd, PRIV_SOCK_RESULT_OK);
		priv_sock_send_fd(sess->parent_fd, fd);
		close(fd);
	
}
//minimize privilege.
void minimize_privilege(void)
{
	struct passwd *pw=getpwnam("nobody");	
	if(pw==NULL)
		return;
	if( setegid(pw->pw_gid)<0 )
	{
		ERR_EXIT("setegid");
	}
	if( seteuid(pw->pw_uid)<0 )
	{
		ERR_EXIT("seteuid");
	}
	
	//set permission that can bind 20.
	struct __user_cap_header_struct cap_header;
	struct __user_cap_data_struct cap_data;

	memset(&cap_header, 0, sizeof(cap_header));
	memset(&cap_data, 0, sizeof(cap_data));

	cap_header.version = _LINUX_CAPABILITY_VERSION_1;
	cap_header.pid = 0;

	__u32 cap_mask = 0;
	cap_mask |= (1 << CAP_NET_BIND_SERVICE);

	cap_data.effective = cap_data.permitted = cap_mask;
	cap_data.inheritable = 0;

	capset(&cap_header, &cap_data);	

}

void handle_parent(session_t *sess)
{
	minimize_privilege();
	char cmd;
	while(1)
	{
		//child-parent connection
		//read(sess->parent_fd, &cmd,1);
		cmd = priv_sock_get_cmd(sess->parent_fd);
		//parsing inner command and process
		//processing command
		switch(cmd)
		{
		case PRIV_SOCK_GET_DATA_SOCK:
			privop_pasv_get_data_sock(sess);
			break;
		case PRIV_SOCK_PASV_ACTIVE:
			privop_pasv_active(sess);
			break;
		case PRIV_SOCK_PASV_LISTEN:
			privop_pasv_listen(sess);
			break;
		case PRIV_SOCK_PASV_ACCEPT:
			privop_pasv_accept(sess);
			break; 
		}
	}
}
