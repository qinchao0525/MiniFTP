#ifndef _SYS_UTIL_H
#define _SYS_UTIL_H

int getlocalip(char *ip);

void activate_nonblock(int fd);
void deactivate_nonblock(int fd);

int read_timeout(int fd, unsigned int wait_seconds);
int write_timeout(int fd, unsigned int wait_seconds);
int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);
int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);

ssize_t readn(int fd, void*buf, size_t count);
ssize_t writen(int fd, const void*buf, size_t count);
ssize_t recv_peek(int sockfd, void* buf, size_t len);
ssize_t recv_peek(int sockfd, void* buf, size_t len);

void send_fd(int sock_fd, int fd);
void recv_fd(const int sock_fd);

#endif /*_SYS_UTIL_H*/
