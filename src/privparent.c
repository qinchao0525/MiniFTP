#include "privparent.h"
#include "session.h"
void handle_parent(session_t *sess)
{
	char cmd;
	while(1)
	{
		read(sess->parent_fd, &cmd,1);
		//inner command and process
	}
}
