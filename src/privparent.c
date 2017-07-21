#include "privparent.h"
#include "session.h"
void handle_parent(session_t *sess)
{
	char cmd;
	while(1)
	{
		//child-parent connection
		read(sess->parent_fd, &cmd,1);
		//parsing inner command and process
		//processing command
	}
}
