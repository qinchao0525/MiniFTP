#ifndef _PRIVSOCK_H_
#define _PRIVSOCK_H_
#include "session.h"

//inner self-define protocol 
//using for ftp service communicate with nobody.


//ftp service request nobody
#define PRIV_SOCK_GET_DATA_SOCK    1
#define PRIV_SOCK_PASV_ACTIVE      2
#define PRIV_SOCK_PASV_LISTEN      3
#define PRIV_SOCK_PASV_ACCEPT      4
//nobody request ftp service
#define PRIV_SOCK_RESULT_OK        1
#define PRIV_SOCK_RESULT_BAD       2
//contest setting
void priv_sock_init(session_t *sess);
void priv_sock_close(session_t *sess);
void priv_sock_set_parent_context(session_t *sess);
void priv_sock_set_child_context(session_t *sess);

//cmd and result
void priv_sock_send_cmd(int fd, char cmd);
char priv_sock_get_cmd(int fd);
void priv_sock_send_result(int fd, char res);
char priv_sock_get_result(int fd);

//send int number(port)
void priv_sock_send_int(int fd, int the_int);
int priv_sock_get_int(int fd);
//send char (ip)
void priv_sock_send_buf(int fd, const char* buf, unsigned int len);
void priv_sock_recv_buf(int fd, char *buf,unsigned int len);
//send fd
void priv_sock_send_fd(int sock_fd, int fd);
int priv_sock_recv_fd(int sock_fd);

#endif /* _PRIVSOCK_H_*/
