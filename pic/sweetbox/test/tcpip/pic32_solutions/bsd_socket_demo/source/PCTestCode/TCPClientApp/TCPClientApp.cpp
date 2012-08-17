// PCSocket.cpp : Defines the entry point for the console application.
//


#include "windows.h"
#include "conio.h"
#include "stdio.h"

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET sk_Local;
	sockaddr_in picINet;
	sockaddr_in localINet;
	int Port = 6653;
	char SendBuf[1024];
	int BufLen = 1024;

	//---------------------------------------------
	// Initialize Winsock
	WSAStartup(MAKEWORD(2,2), &wsaData);

	//---------------------------------------------
	// Create a socket for sending data
	sk_Local = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	picINet.sin_family = AF_INET;
	picINet.sin_port = htons(Port);
	picINet.sin_addr.s_addr = inet_addr(argv[1]);

	localINet.sin_family = AF_INET;
	localINet.sin_port = 0;
	localINet.sin_addr.s_addr = 0;

        bind( sk_Local, (sockaddr*)&localINet, sizeof( localINet ) );

        connect( sk_Local, (sockaddr*)&picINet, sizeof(picINet) );

	strcpy( SendBuf, "Hello from Win\r\n" );
	BufLen = strlen( SendBuf );

        struct sockaddr fromAdr;
        char rcvbuf[50];

        int cntr = 0;
        for( cntr = 0; cntr < 500; cntr++ )
        {
                send( sk_Local, SendBuf, BufLen+1, 0 );
                recv( sk_Local, rcvbuf, BufLen+1, 0 );
                printf( "Data Echo Counter = %d\r", cntr );
        }

        printf( "Data Echo Counter = %d\n\n", cntr );

	//---------------------------------------------
	// When the application is finished sending, close the socket.
	printf("Finished sending. Closing socket.\n");
	closesocket(sk_Local);

	//---------------------------------------------
	// Clean up and quit.
	printf("Exiting.\n");
	WSACleanup();
	return 0;

}

