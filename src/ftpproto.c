#include "ftpproto.h"
#include "sysutil.h"
#include "str.h"
#include "ftpcodes.h"
#include "tunable.h"
#include "privsock.h"

void ftp_reply(session_t *sess, int status, const char*text);
void ftp_lreply(session_t *sess, int status, const char* text);

void upload_common(session_t *sess, int is_append);
void limit_rate(session_t *sess, int bytes_transfered, int is_upload);

int list_common(session_t *sess, int detail);
int get_transfer_fd(session_t *sess);

int port_active(session_t *sess);
int pasv_active(session_t *sess);

static void do_user(session_t *sess);
static void do_pass(session_t *sess);
static void do_cwd(session_t *sess);
static void do_cdup(session_t *sess);
static void do_quit(session_t *sess);
static void do_port(session_t *sess);
static void do_pasv(session_t *sess);
static void do_type(session_t *sess);
static void do_stru(session_t *sess);
static void do_mode(session_t *sess);
static void do_retr(session_t *sess);
static void do_stor(session_t *sess);
static void do_appe(session_t *sess);
static void do_list(session_t *sess);
static void do_nlst(session_t *sess);
static void do_rest(session_t *sess);
static void do_abor(session_t *sess);
static void do_pwd(session_t *sess);
static void do_mkd(session_t *sess);
static void do_rmd(session_t *sess);
static void do_dele(session_t *sess);
static void do_rnfr(session_t *sess);
static void do_rnto(session_t *sess);
static void do_site(session_t *sess);
static void do_syst(session_t *sess);
static void do_feat(session_t *sess);
static void do_size(session_t *sess);
static void do_stat(session_t *sess);
static void do_noop(session_t *sess);
static void do_help(session_t *sess);

typedef struct ftpcmd
{
	const char*cmd;
	void (*cmd_handler)(session_t *sess);
}ftpcmd_t; 

static ftpcmd_t ctrl_cmds[]=
{
//conect control
{"USER", do_user},
{"PASS", do_pass},
{"CWD", do_cwd},
{"XCWD", do_cwd},
{"CDUP", do_cdup},
{"XCUP", do_cdup},
{"QUIT", do_quit},
{"ACCT", NULL},
{"SMNT", NULL},
{"REIN", NULL},
//cmd arg
{"PORT", do_port},
{"PASV", do_pasv},
{"TYPE", do_type},
{"STRU", do_stru},
{"MODE", do_mode},

//service cmd
{"RETR", do_retr},
{"STOR", do_stor},
{"APPE", do_appe},
{"LIST", do_list},
{"NLST", do_nlst},
{"REST", do_rest},
{"ABOR", do_abor},
{"\377\364\377\362ABOR", do_abor},
{"PWD", do_pwd},
{"XPWD", do_pwd},
{"MKD", do_mkd},
{"XMKD", do_mkd},
{"RMD", do_rmd},
{"XRMD", do_rmd},
{"DELE", do_dele},
{"RNFR", do_rnfr},
{"RNTO", do_rnto},
{"SITE", do_site},
{"SYST", do_syst},
{"FEAT", do_feat},
{"SIZE", do_size},
{"STAT", do_stat},
{"NOOP", do_noop},
{"HELP", do_help},
{"STOU", NULL},
{"ALLO", NULL},
};

void handle_child(session_t *sess)
{
	ftp_reply(sess, 220, "miniftpd 0.1");
	int ret;
	while(1)
	{
		memset(sess->cmdline, 0, sizeof(sess->cmdline));
		memset(sess->cmd, 0, sizeof(sess->cmd));
		memset(sess->arg, 0, sizeof(sess->arg));
		/*get cmd */
		ret=readline(sess->ctrl_fd, sess->cmdline, MAX_COMMAND_LINE);
		if(ret==-1)
			ERR_EXIT("readline");
		else if(ret==0)
			exit(EXIT_SUCCESS);
		printf("cmdline=[%s]\n", sess->cmdline);
		//remove \r\n
		str_trim_crlf(sess->cmdline);//cut cmd and value
		printf("cmdline=[%s]\n", sess->cmdline);
		//parsing ftp command and args
		str_split(sess->cmdline, sess->cmd, sess->arg, ' ');
		printf("cmd=[%s], arg=[%s]\n", sess->cmd, sess->arg);
		//process command
		/*if(strcmp("USER", sess->cmd)==0)
			do_user(sess);
		else if(strcmp("PASS", sess->cmd)==0)
			do_pass(sess);*/

		int i=0;
		int size=sizeof(ctrl_cmds)/sizeof(ctrl_cmds[0]);
		for(i=0; i<size; i++)
		{
			if(strcmp(ctrl_cmds[i].cmd, sess->cmd)==0)
			{
				if(ctrl_cmds[i].cmd_handler != NULL)
					ctrl_cmds[i].cmd_handler(sess);
				else
					ftp_reply(sess, FTP_COMMANDNOTIMPL, "unimplement command.");
				break;
			}
		}
		if(i == size)
			ftp_reply(sess, FTP_BADCMD, "unknown command.");
	}
}

void ftp_reply(session_t *sess, int status, const char* text)
{
	char buf[1024]={0};
	sprintf(buf, "%d %s\r\n", status, text);
	writen(sess->ctrl_fd, buf, strlen(buf));
}

void ftp_lreply(session_t *sess, int status, const char* text)
{
	char buf[1024]={0};
	sprintf(buf, "%d-%s\r\n", status, text);
	writen(sess->ctrl_fd, buf, strlen(buf));
}

int list_common(session_t *sess, int detail)
{
	DIR *dir=opendir(".");
	if(dir==NULL)
		return 0;

	struct dirent *dt;
	struct stat sbuf;
	while((dt=readdir(dir))!=NULL)
	{
		if(lstat(dt->d_name, &sbuf)<0)
			continue;
		if(dt->d_name[0]=='.')
			continue;

		char buf[1024] = {0};
		if(detail)
		{
			const char *perms = statbuf_get_perms(&sbuf);

			int off=0;
			off += sprintf(buf, "%s", perms);
			off += sprintf(buf+off, "%3d %-8d %-8d", (int)sbuf.st_nlink, sbuf.st_uid, sbuf.st_gid);
			off += sprintf(buf+off, " %8lu ", (unsigned long)sbuf.st_size);


			const char *datebuf = statbuf_get_date(&sbuf);
			off += sprintf(buf+off, "%s", datebuf);		
			//file name
			if(S_ISLNK(sbuf.st_mode))
			{
				char tmp[1024]={0};
				readlink(dt->d_name, tmp, sizeof(tmp));
				off += sprintf(buf+off, "%s -> %s\r\n", dt->d_name, tmp);
			}
			else
				off +=sprintf(buf+off, " %s\r\n", dt->d_name);
		}
		else
		{
				sprintf(buf, " %s\r\n", dt->d_name);
		}

		writen(sess->data_fd, buf, strlen(buf));
	}
	closedir(dir);
	return 1;
}

void limit_rate(session_t *sess, int bytes_transfered, int is_upload)
{
	long curr_sec = get_time_sec();
	long curr_usec = get_time_usec();

	double elapsed;
	elapsed = curr_sec - sess->bw_transfer_start_sec;
	elapsed += (double)(curr_usec-sess->bw_transfer_start_usec)/(double)1000000;
	if(elapsed <= 0)
		elapsed = 0.01;

	//current transfer rate
	unsigned int bw_rate = (unsigned int)((double)bytes_transfered / elapsed);

	double rate_ratio;
	if(is_upload)
	{
		if(bw_rate < sess->bw_upload_rate_max)
		{
			return ;
		}
		rate_ratio =  bw_rate / sess->bw_upload_rate_max ;
	}
	else
	{
		if(bw_rate < sess->bw_download_rate_max)
		{
			return;
		}
		rate_ratio = bw_rate / sess->bw_download_rate_max ;
	}
	double pause_time;
	pause_time = (rate_ratio - (double)1) * elapsed;

	nano_sleep(pause_time);

	sess->bw_transfer_start_sec = get_time_sec();
	sess->bw_transfer_start_usec = get_time_usec();
}

void upload_common(session_t *sess, int is_append)
{
	//create data connection
	if (get_transfer_fd(sess)==0)
		return;
	
	//
	long long offset =  sess->restart_pos;
	sess->restart_pos = 0;

	//open file.
	int fd=open(sess->arg, O_CREAT | O_WRONLY, 0666);
	if(fd==-1)
	{
		ftp_reply(sess, FTP_UPLOADFAIL, "Could not create file.");
		return;
	} 

	//add w lock
	int ret;
	ret = lock_file_write(fd);
	if(ret==-1)
	{
		ftp_reply(sess, FTP_FILEFAIL, "Could not lock file.");
		return;
	}

	//stor rest+stor appe
	if(!is_append && offset==0)//stor
	{
		ftruncate(fd, 0);
		lseek(fd, 0, SEEK_SET);
	}
	else if(!is_append && offset != 0) //rest + stor
	{
		lseek(fd, offset, SEEK_SET);
	}
	else if(is_append)
	{
		lseek(fd, 0, SEEK_END);
	}

	//relocate. break point reconnect.
	if(offset!=0)
	{
		ret = lseek(fd, offset, SEEK_SET);//
		if(ret==-1)
		{
			ftp_reply(sess, FTP_FILEFAIL, "Failed to open file");
			return ;
		}

	}

	struct stat sbuf;
	ret=fstat(fd, &sbuf);
	if(!S_ISREG(sbuf.st_mode))
	{
		ftp_reply(sess, FTP_FILEFAIL, "could not create file.");
		return ;
	}

	//150 reply
	char text[1024]={0};
	if(sess->is_ascii)
	{
		sprintf(text, "Opening ascii mode data connection for %s (%lld bytes).", sess->arg, (long long)sbuf.st_size);
	}
	else
	{
		sprintf(text, "Opening binary mode data connection for %s (%lld bytes).", sess->arg, (long long)sbuf.st_size);
	}

	ftp_reply(sess, FTP_DATACONN, text);

	int flag=0;
	//upload file
	char buf[1024];
	sess->bw_transfer_start_sec = get_time_sec();
	sess->bw_transfer_start_usec = get_time_usec();

	//sleeptime = 
	while(1)
	{
		ret=read(sess->data_fd, buf, sizeof(buf));
		if(ret==-1)
		{
			if(errno==EINTR)
			{
				continue;
			}
			else
			{
				flag=2;
				break;
			}
		}
		else if(ret==0)
		{
			flag=0;
			break;
		}

		limit_rate(sess, ret, 1);
	
		if(	writen(fd, buf, ret) != ret )
		{
			flag=1;
			break;
		}
	}
	
	//close connection
	close(sess->data_fd);
	sess->data_fd=-1;

	//clsoe file
	close(fd);

	if(flag==0)
	{
		//226
	    ftp_reply(sess, FTP_TRANSFEROK, "Transfer complete.");
	}
	else if(flag==1)
	{
	    ftp_reply(sess, FTP_BADSENDFILE, "Failure writting to local file.");
	}
	else if(flag==2)
	{
	    ftp_reply(sess, FTP_BADSENDNET, "Failure reading from network stream.");
	}
}

static void do_user(session_t *sess)
{
	struct passwd *pw=getpwnam(sess->arg);
	if(pw==NULL)
	{
		//have no user
		ftp_reply(sess, FTP_LOGINERR, "Login incorrect." );
		return;
	}

	sess->uid = pw->pw_uid;
	ftp_reply(sess, FTP_GIVEPWORD, "Please sepecify the password.");

}

static void do_pass(session_t *sess)
{
	
	struct passwd *pw=getpwuid(sess->uid);
	if(pw==NULL)
	{
		//have no user
		ftp_reply(sess, FTP_LOGINERR, "Login incorrect." );
		return;
	}
	
	struct spwd *sp=getspnam(pw->pw_name);
	if(sp==NULL)
	{
		//have no user
		ftp_reply(sess, FTP_LOGINERR, "Login incorrect." );
		return;
	}

	//encrypt the password
	char *encrypted_pass = crypt(sess->arg, sp->sp_pwdp);
	//specify the passwd
	if(strcmp(encrypted_pass, sp->sp_pwdp)!=0)
	{
		//have no user
		ftp_reply(sess, FTP_LOGINERR, "Login incorrect." );
		return;
	}
	
	umask(tunable_local_umask);
	setegid(pw->pw_gid);
	seteuid(pw->pw_uid);
	chdir(pw->pw_dir);

	ftp_reply(sess, FTP_LOGINOK, "Login successful.");
}

static void do_cwd(session_t *sess)
{
		if (chdir(sess->arg)<0)//chage directory
		{
			ftp_reply(sess, FTP_FILEFAIL, "Failed to chage directory.");
		}
		ftp_reply(sess, FTP_CWDOK, "Directory successfully changed.");

}
static void do_cdup(session_t *sess)
{
		if (chdir("..")<0)//chage directory
		{
			ftp_reply(sess, FTP_FILEFAIL, "Failed to chage directory.");
		}
		ftp_reply(sess, FTP_CWDOK, "Directory successfully changed.");
		
}
static void do_quit(session_t *sess)
{
}
static void do_port(session_t *sess)
{
	unsigned int v[6];
	sscanf(sess->arg, "%u,%u,%u,%u,%u,%u", &v[2], &v[3], &v[4], &v[5], &v[0], &v[1]);
	sess->port_addr=(struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	memset(sess->port_addr, 0, sizeof(struct sockaddr_in));
	sess->port_addr->sin_family = AF_INET;
	unsigned char *p = (unsigned char *)&sess->port_addr->sin_port;
	p[0]=v[0];
	p[1]=v[1];

	p=(unsigned char*)&sess->port_addr->sin_addr;
	p[0]=v[2];
	p[1]=v[3];
	p[2]=v[4];
	p[3]=v[5];

	ftp_reply(sess, FTP_PORTOK, "PORT command successful.")	;
}
static void do_pasv(session_t *sess)
{
	char ip[16]={0};
	getlocalip(ip);
	
/*
	sess->pasv_listen_fd=tcp_server(ip, 0);
	struct sockaddr_in addr;
	socklen_t addrlen=sizeof(addr);
	if( getsockname(sess->pasv_listen_fd, (struct sockaddr*)&addr, &addrlen) < 0)
		ERR_EXIT("getsockname");
	
	unsigned short port=ntohs(addr.sin_port);
*/
	priv_sock_send_cmd(sess->child_fd, PRIV_SOCK_PASV_LISTEN);
	unsigned short port = (int)priv_sock_get_int(sess->child_fd);
	
	unsigned v[4];
	sscanf(ip, "%u.%u.%u.%u", &v[0], &v[1], &v[2], &v[3]);
	char text[1024]={0};
	sprintf(text, "Enter Passive Mode (%u,%u,%u,%u,%u,%u).",v[0], v[1], v[2], v[3], port>>8, port & 0xFF );
	ftp_reply(sess, FTP_PASVOK, text);
}
static void do_type(session_t *sess)
{
	if(strcmp(sess->arg, "A")==0)
	{
		sess->is_ascii = 1;
		ftp_reply(sess, FTP_TYPEOK, "Switching to ASCII mode.");
	}
	else if(strcmp(sess->arg, "I")==0)
	{
		sess->is_ascii = 0;
		ftp_reply(sess, FTP_TYPEOK, "Switching to Binary mode.");
	}
	else
		ftp_reply(sess, FTP_BADCMD, "Unrecognised Type command.");
}
static void do_stru(session_t *sess)
{
}
static void do_mode(session_t *sess)
{
}
static void do_retr(session_t *sess)
{
	//create data connection
	if (get_transfer_fd(sess)==0)
		return;
	
	//
	long long offset =  sess->restart_pos;
	sess->restart_pos = 0;

	//open file.
	int fd=open(sess->arg, O_RDONLY);
	if(fd==-1)
	{
		ftp_reply(sess, FTP_FILEFAIL, "Fail to open file.");
		return;
	} 

	//add rw lock
	int ret;
	ret = lock_file_read(fd);
	if(ret==-1)
	{
		ftp_reply(sess, FTP_FILEFAIL, "fail to open file");
		return;
	}

	//is ordinary file
	struct stat sbuf;
	ret = fstat(fd, &sbuf);
	if( !S_ISREG(sbuf.st_mode) )
	{
		ftp_reply(sess, FTP_FILEFAIL, "Failed to open file");
		return ;
	}

	//relocate. break point reconnect.
	if(offset!=0)
	{
		ret = lseek(fd, offset, SEEK_SET);
		if(ret==-1)
		{
			ftp_reply(sess, FTP_FILEFAIL, "Failed to open file");
			return ;
		}

	}

	//150 reply
	char text[1024]={0};
	if(sess->is_ascii)
	{
		sprintf(text, "Opening ascii mode data connection for %s (%lld bytes).", sess->arg, (long long)sbuf.st_size);
	}
	else
	{
		sprintf(text, "Opening binary mode data connection for %s (%lld bytes).", sess->arg, (long long)sbuf.st_size);
	}

	ftp_reply(sess, FTP_DATACONN, text);

	int flag=0;
	//download file
	char buf[4096];
	while(1)
	{
		ret=read(fd, buf, sizeof(buf));
		if(ret==-1)
		{
			if(errno==EINTR)
			{
				continue;
			}
			else
			{
				flag=1;
				break;
			}
		}
		else if(ret==0)
		{
			flag=0;
			break;
		}

		if(	writen(sess->data_fd, buf, ret) != ret )
		{
			flag=2;
			break;
		}
	}
	
	//close connection
	close(sess->data_fd);
	sess->data_fd=-1;

	//clsoe file
	close(fd);

	if(flag==0)
	{
		//226
	    ftp_reply(sess, FTP_TRANSFEROK, "Transfer complete.");
	}
	else if(flag==1)
	{
	    ftp_reply(sess, FTP_BADSENDFILE, "Failure reading from local file.");
	}
	else if(flag==2)
	{
	    ftp_reply(sess, FTP_BADSENDNET, "Failure writting to network stream.");
	}
}
static void do_stor(session_t *sess)
{
	upload_common(sess, 0);
}
static void do_appe(session_t *sess)
{
	upload_common(sess, 1);
}
//port model is active.
int port_active(session_t *sess)
{
	if(sess->port_addr)	
	{
		if(pasv_active(sess))
		{
			fprintf(stderr, "both port and pasv are active.");
			exit(EXIT_FAILURE);
		}
		return 1;
	}
	else
		return 0;
}
int pasv_active(session_t *sess)
{
/*
	if(sess->pasv_listen_fd != -1)
	{
		if(port_active(sess))
		{
			fprintf(stderr, "both port and pasv are active.");
			exit(EXIT_FAILURE);
		}
		return 1;
	}
*/
	priv_sock_send_cmd(sess->child_fd, PRIV_SOCK_PASV_ACTIVE);
	int active = priv_sock_get_int(sess->child_fd);
	if(active)
	{
		if(port_active(sess))
		{
			fprintf(stderr, "both port and pasv are active.");
			exit(EXIT_FAILURE);
		}
		return 1;
	}
	return 0;
}
int get_port_fd(session_t *sess)//creating data socket fd 
{
		
		priv_sock_send_cmd(sess->child_fd, PRIV_SOCK_GET_DATA_SOCK);
		unsigned short port = ntohs(sess->port_addr->sin_port);
		char *ip = inet_ntoa(sess->port_addr->sin_addr);
		priv_sock_send_int(sess->child_fd, (int)port);
		priv_sock_send_buf(sess->child_fd, ip, strlen(ip));

		//get result.
		char res=priv_sock_get_result(sess->child_fd);
		if(res==PRIV_SOCK_RESULT_BAD)
			return 0;
		else if (res==PRIV_SOCK_RESULT_OK)
		{
			sess->data_fd = priv_sock_recv_fd(sess->child_fd);
		}
		return 1;
}

int get_pasv_fd(session_t *sess)
{
	priv_sock_send_cmd(sess->child_fd, PRIV_SOCK_PASV_ACCEPT);
	char res = priv_sock_get_result(sess->child_fd);
	if(res==PRIV_SOCK_RESULT_BAD)
	{
		return 0;
	}
	else if(res==PRIV_SOCK_RESULT_OK)
	{
		sess->data_fd = priv_sock_recv_fd(sess->child_fd);
	}
	return 1;
}

int get_transfer_fd(session_t *sess)
{
	//test last command
	if(!port_active(sess) && !pasv_active(sess))
	{
		ftp_reply(sess, FTP_BADSENDCONN, "Use port or pasv first.");
		return 0;
	}
	// port mode
	int ret=1;
	if(port_active(sess))
	{
		//tcp client(20 port)
		/*int fd = tcp_client(0);
		if( connect_timeout(fd, sess->port_addr, tunable_connect_timeout)<0)
		{
			close(fd);
			return 0;
		}
		sess->data_fd = fd;*/
		if(get_port_fd(sess)==0)
			ret=0;
	}
	// if it is pasv mode.
	if(pasv_active(sess))
	{
/*
		int fd = accept_timeout(sess->pasv_listen_fd, NULL, tunable_accept_timeout);
		close(sess->pasv_listen_fd);
		if(fd==-1)//create fd failure
		{
			close(sess->pasv_listen_fd);
			return 0;
		}
		
		sess->data_fd = fd;
*/
		if(get_pasv_fd(sess)==0)
		{
			ret=0;
		}
	}
	if(sess->port_addr)
	{
		free(sess->port_addr);
		sess->port_addr=NULL;
	}
	
	return ret;
}
//transfer list
static void do_list(session_t *sess)
{
	//create data connection
	if (get_transfer_fd(sess)==0)
		return;
	//150 reply
	ftp_reply(sess, FTP_DATACONN, "Here comes the directory listing.");
	//transfer list
	list_common(sess, 1);
	//close connection
	close(sess->data_fd);
	sess->data_fd=-1;
	//226
	ftp_reply(sess, FTP_TRANSFEROK, "Direscory send ok.");
}


static void do_nlst(session_t *sess)
{
	//create data connection
	if (get_transfer_fd(sess)==0)
		return;
	//150 reply
	ftp_reply(sess, FTP_DATACONN, "Here comes the directory listing.");
	//transfer list
	list_common(sess, 0);
	//close connection
	close(sess->data_fd);
	sess->data_fd=-1;
	//226
	ftp_reply(sess, FTP_TRANSFEROK, "Direscory send ok.");
}
static void do_rest(session_t *sess)
{
	sess->restart_pos = str_to_longlong(sess->arg);
	char text[1024]={0};
	sprintf(text, "Restart position accepted (%lld).", sess->restart_pos);
	ftp_reply(sess, FTP_RESTOK, text);
}
static void do_abor(session_t *sess)
{
}
static void do_pwd(session_t *sess)
{
	char text[1024]={0};
	char dir[1024+1]={0};
	getcwd(dir, 1024);
	sprintf(text, "\"%s\"", dir);
	ftp_reply(sess, FTP_PWDOK, text);
}
static void do_mkd(session_t *sess)
{
		if(mkdir(sess->arg, 0777)<0)
		{
			return ;
		}
		char text[4096]={0};
		if(sess->arg[0]=='/')
		{
			sprintf(text, "%s created", sess->arg);
		}
		else
		{
			char dir[4096+1]={0};
			getcwd(dir, 4096);
			if(dir[strlen(dir)-1] == '/')
			{
				sprintf(text, "%s%s created", dir, sess->arg);
			}
			else
			{
				sprintf(text, "%s/%s created", dir, sess->arg);
			}
		}
		ftp_reply(sess, FTP_MKDIROK, text);
}
static void do_rmd(session_t *sess)
{
	if(rmdir(sess->arg)<0)
	{
		ftp_reply(sess, FTP_FILEFAIL, "Remove directory operation failed.");		
	}
		ftp_reply(sess, FTP_RMDIROK, "Remove directory operation successful.");		
}
static void do_dele(session_t *sess)
{
	if(unlink(sess->arg)<0)
	{
		ftp_reply(sess, FTP_FILEFAIL, "Delete operation failed.");		
		return ;
	}
	ftp_reply(sess, FTP_DELEOK, "Delete operation successful.");
}
static void do_rnfr(session_t *sess)
{
}
static void do_rnto(session_t *sess)
{
}
static void do_site(session_t *sess)
{
}
static void do_syst(session_t *sess)
{
	ftp_reply(sess, FTP_SYSTOK, "UNIX type: L8");
}
static void do_feat(session_t *sess)
{
	ftp_lreply(sess, FTP_FEAT, "Features:");
	writen(sess->ctrl_fd, " EPRT\r\n", strlen(" EPRT\r\n"));	
	writen(sess->ctrl_fd, " EPSV\r\n", strlen(" EPSV\r\n"));	
	writen(sess->ctrl_fd, " MDTM\r\n", strlen(" MDTM\r\n"));	
	writen(sess->ctrl_fd, " PASV\r\n", strlen(" PASV\r\n"));	
	writen(sess->ctrl_fd, " REST STREAM\r\n", strlen(" REST STREAM\r\n"));	
	writen(sess->ctrl_fd, " SIZE\r\n", strlen(" SIZE\r\n"));	
	writen(sess->ctrl_fd, " TUFS\r\n", strlen(" TUFS\r\n"));	
	writen(sess->ctrl_fd, " UTF8\r\n", strlen(" UTF8\r\n"));
	ftp_reply(sess, FTP_FEAT, "END");	
}

static void do_size(session_t *sess)
{
	struct stat buf;
	if(	stat(sess->arg, &buf) < 0)
	{
		ftp_reply(sess, FTP_FILEFAIL, "SIZE operation failed.");
		return;
	}
	if(!S_ISREG(buf.st_mode))
	{
		ftp_reply(sess, FTP_FILEFAIL, "Could not get file size.");
		return ;

	}
	char text[1024]={0};
	sprintf(text, "%lld", (long long)buf.st_size);
	ftp_reply(sess, FTP_SIZEOK, text);
}
static void do_stat(session_t *sess)
{
}
static void do_noop(session_t *sess)
{
}
static void do_help(session_t *sess)
{
}

