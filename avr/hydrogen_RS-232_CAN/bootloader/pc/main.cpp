// Copyright (C) 2002 Jaroslaw Karwik, 
//   Mente Marine Oy <jaroslaw.karwik@mente-marine.com>
// 
// This file is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; 
// 
// In addition to the permissions in the GNU General Public License, the
// author gives you unlimited permission to link the
// compiled version of this file into combinations with other programs,
// and to distribute those combinations without any restriction coming
// from the use of this file.  (The General Public License restrictions
// do apply in other respects; for example, they cover modification of
// the file, and distribution when not linked into a combine
// executable.)
// 
// This file is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.  
// 

#include <windows.h>
#include <iostream>

#include "CommEngine.h"
#include "HexEngine.h"
#include "LinkEngine.h"


using namespace std;


void printUsage()
{
   cout << "Use : loadmega128.exe  comNr comSpeed filename\n";
}


int main( int argc, char *argv[ ])
{
    int comNr;
	int comSpeed;
	int result = -1;


	if ( argc != 4 ){
		printUsage();
		return -1;
	}

	cout << "\nAVR boot loader data sender (version 1.0 from "__DATE__")\n";
    cout << "Copyright (C) Jaroslaw Karwik, Mente Marine Oy\n";
	cout << "Contact address: jaroslaw.karwik@mente-marine.com\n\n";    

	comNr = atoi(argv[1]);
	comSpeed = atoi(argv[2]);

	if( (comNr == 0) || (comSpeed == 0) ){
		printUsage();
		return -1;
	}

	if(hex_Open(argv[3]) != true){
		return -1;
	}
	
	if(comm_Init() != true ){
		cout << "Communication     -> Problems with COM initialization!\n";
		return -1;
	}

	do {
		if(comm_Open( comNr) != true ){
			cout << "Communication     -> Cannot open COM port!\n";
			break;
		}

		if(comm_Setup( comSpeed,NOPARITY,ONESTOPBIT) != true ){
			cout << "Communication     -> Cannot setup COM port!\n";
			break;
		}
		
		if(lnk_sendAvrFile() == true){
			result = 0;
		}

	} while( 0 );

	comm_Close();
    comm_Release();

	if(result == 0){
		cout << "Program update    -> Success!\n";
	}else
	{
		cout << "Program update    -> Failure\n";
	}

	return result;

}