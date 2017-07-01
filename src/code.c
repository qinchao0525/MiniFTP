
#define ERR_EXIT(m)\
        do\
	{\
	    perror(m);\
            exit(EXIT_FAILURE);\
        }while(0)
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

ssize_t readn(int fd, void*buf, size_t count)
{
	size_t nleft = count;
	ssize_t nread;
	char* bufp = (char*) buf;

	while(nleft>0)
	{
		if((nread=read(fd, bufp, nleft)) < 0)
		{	
			if(errno == EINTR)
				continue;
			return -1;
		}
		else if(nread==0)
			return count-nleft;
		bufp += nread;
		nleft -= nread;
	}
	return count;
}

ssize_t writen(int fd, const void*buf, size_t count)
{

	size_t nleft = count;
	ssize_t nwritten;
	char* bufp = (char*) buf;

	while(nleft>0)
	{
		if((nwritten=write(fd, bufp,nleft)) < 0)
		{	
			if(errno == EINTR)
				continue;
			return -1;
		}

		else if(nwritten==0)
			continue;
		bufp += nwritten;
		nleft -= nwritten;
	}
	return count;
}

ssize_t recv_peek(int sockfd, void* buf, size_t len)
{
	while(1)	
	{	
		int ret=recv(sockfd, buf, len, MSG_PEEK);
		if(ret==-1 && errno==EINTR)
			continue;
		return ret;
	}
}


ssize_t readline(int sockfd, void* buf, size_t maxline)
{
	int ret;
	int nread;
	char* bufp=buf;
	int nleft=maxline;
	while(1)
	{
		ret=recv_peek(sockfd, bufp, nleft);
		if(ret<0)
			return ret;
		else if(ret==0)
			return ret;
		nread=ret;
		int i;
		for(i=0; i<nread; i++)
		{
			if(bufp[i]=='\n')
			{
				ret=readn(sockfd, bufp, i+1);
				if(ret!=i+1)
					exit(EXIT_FAILURE);
				return ret;
			}
		}
		if( nread>nleft )
			exit(EXIT_FAILURE);

		nleft-=nread;
		ret=readn(sockfd, bufp, nread);
		if(ret!=nread)
			exit(EXIT_FAILURE);
		bufp+=nread;
	}
	return -1;
}
