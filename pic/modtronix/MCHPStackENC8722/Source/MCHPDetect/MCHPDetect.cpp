/*********************************************************************
 *
 *     Microchip Announcement Detector
 *
 *********************************************************************
 * FileName:        MCHPDetect.cpp
 * Dependencies:    Must be linked with Ws2_32.lib
 * Processor:       PIC18
 * Complier:        Microsoft Visual C++ 6.0
 *		    Borland C++ Builder 6
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 *
 *
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder	10/8/04	Original
********************************************************************/

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers

#define UDPPort	       30303

#include <Winsock2.h>
#include <iostream>
#include <stdio.h>


				
int main(int argc, char* argv[])
{
	WSADATA			WSAData;
	SOCKET			MySocket;
	char			PacketPayload[1500];
	int			ret;
	sockaddr_in		MySocketInfo;
	sockaddr		RemoteAddr;
	int			RemoteAddrSize;
	unsigned int	        MessageCount = 1;


	if(WSAStartup((short)0x0202, &WSAData) != 0)
	{
        printf("WSAStartup failed!\n");
        return 1;
	}


	MySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(MySocket == INVALID_SOCKET)
	{
		std::cout << "Unable to create a socket.\n";
		WSACleanup();
		return 1;
	}


	MySocketInfo.sin_family = AF_INET;
	MySocketInfo.sin_port = htons(UDPPort);
	MySocketInfo.sin_addr.S_un.S_addr = htonl(ADDR_ANY);
	ZeroMemory(&MySocketInfo.sin_zero, sizeof(MySocketInfo.sin_zero));

	ret = bind(MySocket, (const struct sockaddr *)&MySocketInfo, sizeof(MySocketInfo));
	if(ret == INVALID_SOCKET)
	{
		std::cout << "Bind procedure failed.\n";
		WSACleanup();
		return 1;
	}

	RemoteAddrSize = sizeof(RemoteAddr);

	std::cout << "Microchip Announcement Detector\n"
		     "Copyright (c) 2004 Microchip Technology, Inc. Ver. 1.0 (Oct 06 2004)\n"
		     "Written by Howard Henry Schlunder\n\n"
		     "Waiting for data on UDP port " << UDPPort << "...\n\n";


	while(1)
	{
		ret = recvfrom(MySocket, PacketPayload, sizeof(PacketPayload), 0, &RemoteAddr, &RemoteAddrSize);
		if(ret == SOCKET_ERROR)
		{
			std::cout << "Socket error while trying to receive.\n";
			WSACleanup();
			return 1;
		}

		PacketPayload[ret] = 0;	// Null terminate

		std::cout << "Total packets received: " << MessageCount++ << "\n"
				  << "Received " << ret << " bytes of data from " 
				  << (unsigned char)RemoteAddr.sa_data[2] + '\0' << '.' 
				  << (unsigned char)RemoteAddr.sa_data[3] + '\0' << '.' 
				  << (unsigned char)RemoteAddr.sa_data[4] + '\0' << '.' 
				  << (unsigned char)RemoteAddr.sa_data[5] + '\0' << '\n'
				  << PacketPayload << "\n\n";
	}
	

	WSACleanup();
	return 0;
}

