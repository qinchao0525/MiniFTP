#ifndef _FTPCLIENT_H_
#define _FTPCLIENT_T_
class ftpclient
{
private:
	enum { 
		SERVER_PORT = 9999,
		BUFFER_SIZE = 4096
	};
	sockadder_in serverChannel;
	char buffer[BUFFER_SIZE];
	int serverSocket;
	int clientSocket;
	bool isConnect;
	char name[50];

	bool getFile();
	bool putFile();
	bool acknowledge();
	bool sendRequest(char * instruction);
	bool connect2Host(const char* hostName);
	bool getWorkDir();

public:
	ftpClient();
	~ftpClient();
	void start();
}
#endif /*_FTPCLIENT_H_*/
