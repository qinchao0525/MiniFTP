#include "ftpclient.h"
#include <cstdlib>
#include <stdio.h>
#include <cstring>
#include <fstream>

ftpclient::ftpclient()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int ret;

	wWersionRequested = MAKEWORD(2, 2);
	ret = WSAStartup(wVersionRequested, &wsaData);
	if (ret!=0)
	{
		printf("WSAStartip() failed!\n");
	}
	if(LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion)!=2)
	{
		WSACleanup();
		printf("Invalid winsock version!\n");
	}
	isConnect = false;
}

void ftpclient::start()
{
	char c[100];
	char d[100];
	printf("Enter help for help measures, quit to quit\n");
	
	while(1)
	{
		scanf("%s", c);
		if(strcmp(c, "help")==0)
		{
			printf("get [filename] -- download file\n
			put[filename] -- upload file\n
			ftp [ip] -- enter ftp\n
			pwd -- show pwd\n
			cd [dirname] -- change dir\n
			close -- close connection\n
			quit -- quit client.\n");
		}
		else if (strcmp(c, "get")==0)
		{
			scanf("%s", d);
			strcat(c, " ");
			strcat(c, d);
			if(!isConnect)
			{
				printf("you haven't connect to any server!\n");
			}
			else
				sendRequest(c);
		}
		else if(strcmp(c, "put")==0)
		{
			scanf("%s", d);
			strcat(c, " ");
			strcat(c, d);
			if(!isConnect)
			{
				printf("you haven't connect to any server!\n");
			}
			else
				sendRequest(c);
		}
		else if(strcmp(c, "ftp")==0)
		{
			scanf("%s", d);
			if(!isConnect&&connect2Host(d))
			{
				isConnect=true;
			}
			else if(isConnect)
			{
				printf("you have already connected to server\n
					please close the connection before connect 
					to a new server\n");
			}
		}
		else if(strcmp(c, "pwd")==0)
		{
			if(!isConnect)
			{
				printf("you haven't connect to any server!\n");
			}
			else
				sendRequest(c);
		}
		else if(strcmp(c, "cd")==0)
		{
			scanf("%s", d);
			strcat(c, " ");
			strcat(c, d);
			if(!isConnect)
			{
				printf("you haven't connect to any server!\n");
			}
			else
				sendRequest(c);
		}
		else if(strcmp(c, "quit")==0)
		{
			if(isConnect)
			{
				isConnect = false;
				send(clientSocket, c, strlen(c)+1, 0);
				closesocket(clientSocket);
			}
		}
		else
			printf("syntex error\n");
	}
}

bool 
