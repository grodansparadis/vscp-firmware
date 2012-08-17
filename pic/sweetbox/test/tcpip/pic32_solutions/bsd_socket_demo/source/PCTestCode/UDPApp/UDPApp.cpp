// PCSocket.cpp : Defines the entry point for the console application.
//

#include "windows.h"
#include "conio.h"
#include "stdio.h"

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET SendSocket;
	sockaddr_in RecvAddr;
	int Port = 6652;
	char SendBuf[1024];
	int BufLen = 1024;

	//---------------------------------------------
	// Initialize Winsock
	WSAStartup(MAKEWORD(2,2), &wsaData);

	//---------------------------------------------
	// Create a socket for sending data
	SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//---------------------------------------------
	// Set up the RecvAddr structure with the IP address of
	// the receiver (in this example case "123.456.789.1")
	// and the specified port number.
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(Port);
	RecvAddr.sin_addr.s_addr = inet_addr(argv[1]);

	strcpy( SendBuf, "Hello from Win\r\n" );
	BufLen = strlen( SendBuf );
	//---------------------------------------------
	// Send a datagram to the receiver

        struct sockaddr fromAdr;
        char rcvbuf[50];
        int tempz = sizeof(struct sockaddr);

        DWORD startTick = GetTickCount();
        int cntr = 0;

	for( int temp = 0;!kbhit(); temp++ )
	{
		//sprintf( SendBuf, "Hello From Win %04d\r\n", temp );
                sprintf( SendBuf, "%d ", temp );
		BufLen = strlen( SendBuf )+1;

		//printf("Send: %s\n", SendBuf);
		sendto(SendSocket,
		SendBuf,
		BufLen,
		0,
		(SOCKADDR *) &RecvAddr,
		sizeof(RecvAddr));

                //Sleep( 10 );

                if( recvfrom( SendSocket, rcvbuf, 50, 0, &fromAdr, &tempz  ) > 0 )
                        ;//printf( "Rcv: %s\r\n", rcvbuf );
                else
                        printf( "Rcv Datagram Error\n" );

                cntr++;

                if( cntr % 100 == 0 )
                        printf( "Echo Count = %d\r", cntr );

                if( GetTickCount() > (startTick + 10000) )
                        break;


	}

        printf( "\r\nMsg Counter = %d\n", cntr );

	//---------------------------------------------
	// When the application is finished sending, close the socket.
	printf("Finished sending. Closing socket.\n");
	closesocket(SendSocket);

	//---------------------------------------------
	// Clean up and quit.
	printf("Exiting.\n");
	WSACleanup();
	return 0;

}

