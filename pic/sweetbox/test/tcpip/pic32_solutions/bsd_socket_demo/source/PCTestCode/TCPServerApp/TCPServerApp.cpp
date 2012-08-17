// PCSocket.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <stdio.h>
#include <conio.h>

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET sk_serv, s2;

	sockaddr_in localINet;
	int Port = 7000;
        int temp;
	char msg[25];

	//---------------------------------------------
	// Initialize Winsock
	WSAStartup(MAKEWORD(2,2), &wsaData);

	//---------------------------------------------
	// Create a socket for sending data
	sk_serv = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	localINet.sin_family = AF_INET;
	localINet.sin_port = htons(Port);
	localINet.sin_addr.s_addr = 0;
        
        bind( sk_serv, (sockaddr*)&localINet, sizeof( localINet ) );

        listen( sk_serv, 5 );

        printf( "waiting for connection on Port %d\n", Port );
        s2 = accept( sk_serv, NULL, NULL );

        printf( "Connection accepted. Waiting for Data\n" );
        int len = 0;
        while( len > 0 )
        {
                len = recv( s2, msg, 25, 0 );
                if( len <= 0 )
                {
                        break;
                }
                msg[10] = 0;
                printf( "Msg Rcvd: %s\n", msg );
                len = send( s2, msg, 11, 0 );
                if( len <= 0 )
                {
                        break;
                }

        }

	//---------------------------------------------
	// When the application is finished sending, close the socket.
	printf("Closing socket.\n");
	closesocket(s2);
	closesocket(sk_serv);
	//---------------------------------------------
	// Clean up and quit.
	printf("Exiting.\n");
	WSACleanup();
	return 0;

}

