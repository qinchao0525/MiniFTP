#include "ftpproto.h"
#include "sysutil.h"
#include "str.h"
#include "ftpcodes.h"
#include "common.h"

void ftp_reply(session_t *sess, int status, const char* text);

static void do_user(session_t *sess);
static void do_pass(session_t *sess);

void handle_child(session_t *sess)
{
	ftp_reply(sess, 220, "miniftpd 0.1");
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
		if(strcmp("USER", sess->cmd)==0)
			do_user(sess);
		else if(strcmp("PASS", sess->cmd)==0)
			do_pass(sess);
	}
}

void ftp_reply(session_t *sess, int status, const char* text)
{
	char buf[1024]={0};
	sprintf(buf, "%d %s\r\n", status, text);
	writen(sess->ctrl_fd, buf, strlen(buf));
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
	
	setegid(pw->pw_gid);
	seteuid(pw->pw_uid);
	chdir(pw->pw_dir);

	ftp_reply(sess, FTP_LOGINOK, "Login successful.");
}
