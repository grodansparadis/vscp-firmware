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


//---------------------------------------------------------------------------

#include "HexEngine.h"
#include <stdio.h>
#include <iostream>


using namespace std;

//---------------------------------------------------------------------------


static char           dataBuffer[128 * 1024];
static unsigned long  minAddress;
static unsigned long  maxAddress;	 

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

int getHexDigit(char character)
{

	character = toupper(character);

	if( (character >= '0') && (character <= '9') ){
		return character -'0';
	}else if( (character >= 'A') && (character <= 'F') )
	{
		return character - 'A' + 10;
	}else
	{
		return 0;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ostream & printAddress(ostream & istr,unsigned long address,int width = 5)
{
	   istr << hex;
	   istr << "0x";

	   istr.fill('0');
	   istr.width(width);
	   istr << address;

	   return istr;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

int getHexPair(char **pos, unsigned char * crc)
{
  int result;

 
  result  = getHexDigit(*(*pos)) << 4;
  (*pos) ++;
  result |= getHexDigit(*(*pos));
  (*pos) ++;

  (*crc) += result;

  return result;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool getHexData(char ** pos,int lineDataLength,char * data,unsigned char * crc)
{
	int				ii;
	unsigned char   fileCRC; 
	unsigned char   dummy;
	


	for(ii = 0; ii < lineDataLength; ii++){
	   data[ii]	= getHexPair(pos,crc);
	}

    fileCRC = getHexPair(pos,&dummy);
	if( fileCRC != (unsigned char)(~(*crc) + 1)){
 	   return false;
	}else
	{
       return true;
	}

}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool   hex_Open(const char * hexFile)
{
   FILE			  * file;
   char				line[1024];
   char				data[1024];
   char			  * pos;
   bool				result        = true;
   bool				eofFound      = false;
   

   unsigned long	baseAddress   = 0;
   unsigned long	currAddress   = 0;

   int				lineStringLength;
   int				lineDataLength;
   unsigned long	lineAddress;
   int				lineType;
   int				lineCount = 0;
   int				ii;

   unsigned char		 crc;

   minAddress = sizeof(dataBuffer);
   maxAddress = 0;
   memset(dataBuffer,0xff,sizeof(dataBuffer));

     
   
   cout << "Open hex file     -> " << hexFile << '\n';

   if( (file  = fopen( hexFile , "rt" )) == NULL ){
	  cout << "Open hex file     -> Error,the file cannot be opened "<< '\n';
	  return false;	
   }
      

   while( (fgets( line, sizeof(line) - 1 , file ) != NULL) && (result == true) ){

	   if(eofFound == true){
		 // there should be nothing after EOF marker
		 result = false;
	     cout << "Open hex file     -> Error,extra data after EOF record (line #"<< lineCount << ")" << '\n';
		 break;
	   }

       crc = 0;
	   pos = line;

	   if(*pos++ != ':'){
		 // missing colon ? 
		 result = false;
		 cout << "Open hex file     -> Error,missing colon (line #"<< lineCount << ")" << '\n';		 
		 break;
	   }

	   lineStringLength =  strlen(line);
       lineDataLength   =  getHexPair(&pos,&crc);
	   lineAddress      =  getHexPair(&pos,&crc) << 8;
	   lineAddress     |=  getHexPair(&pos,&crc);

	   if( lineStringLength != (1+ 2 + 4 + 2 + lineDataLength *2 + 2+  1) ){
		 // wrong length of the data stream ? 
		 result = false;
	     cout << "Open hex file     -> Error,wrong line length (line #"<< lineCount << ")" << '\n';		 
		 break;
	   }

	   lineType = getHexPair(&pos,&crc);
	   
	   if(getHexData(&pos,lineDataLength,data,&crc)!= true ){
		 // wrong length of the data stream ? 
		 result = false;
	     cout << "Open hex file      -> Error,wrong CRC (line #"<< lineCount << ")" << '\n';		 
		 break;
	   }

	   switch( lineType )
	   {

		 case 0x00:
			 {
				 for(ii = 0; ii < lineDataLength; ii ++ ){

					currAddress = baseAddress + lineAddress + ii;
					if (currAddress > sizeof(dataBuffer) ){
						 // wrong length of the data stream ? 
						 result = false;
					     cout << "Open hex file     -> Error,too big address (line #"<< lineCount << ")" << '\n';		 
						 break;
					}

					dataBuffer[currAddress] = data[ii];

					if(currAddress < minAddress){
						minAddress = currAddress;
					}

					if(currAddress > maxAddress){
						maxAddress = currAddress;
					}


				 }

			 }break;

		 case 0x01:
			 {
				eofFound = true;
			 }break;

		 case 0x04:
			 {
				baseAddress = (lineAddress << 16);
			 }break;

		 case 0x02:
			 {
				baseAddress = (lineAddress << 4);
			 }break;


		 default:
			{
			    result = false;
		        cout << "Open hex file      -> Error,wrong record type (line #"<< lineCount << ")" << '\n';		 
			}break;
	   }
		




	   lineCount ++;
   }


   if( (eofFound != true) && (result == true)){
	 // missing EOF marker
	 result = false;
	 cout << "Open hex file      -> Error, missing EOF record" << '\n';		 
   }


   if(result == true){
	   

	   cout <<  "Open hex file     -> Start address: ";  
	   printAddress(cout,maxAddress); 
	   cout << "\n";

	   cout <<  "Open hex file     -> End   address: ";
	   printAddress(cout,minAddress);
	   cout << "\n";

	   cout <<  "Open hex file     -> Length       : ";
	   printAddress(cout,maxAddress - minAddress);
	   cout <<	"( " << dec << maxAddress - minAddress << " bytes)\n";


   }

   fclose(file);

   return result;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

unsigned long    hex_GetMinAddress()
{
   return minAddress;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

unsigned long   hex_GetMaxAddress()
{
   return maxAddress;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

const char * hex_GetData(unsigned long startAddress)
{
   if (startAddress > sizeof(dataBuffer) ){
     startAddress = startAddress % sizeof(dataBuffer);
   }

   return &dataBuffer[startAddress];
}

