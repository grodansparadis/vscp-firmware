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
#include <windows.h>
#include <iostream>
#include <time.h>
#include <assert.h>

#include "HexEngine.h"
#include "CommEngine.h"

using namespace std;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

typedef struct {
	    unsigned char  start; 
		unsigned char length;
		unsigned char cmd;
		unsigned char data[256];

}frameDataType;


#define FRAME_INIT(frame_,length_,cmd_)    \
				frame_.start   = '!';      \
				frame_.length  = length_;  \
				frame_.cmd     = cmd_	


#define FRAME_SET_CRC(frame_)																\
				frame_.data[frame_.length] = lnkCrc(										\
												(unsigned char*)&frame_.length,			    \
												(unsigned char*)&frame_.data[frame_.length]  \
											);

#define FRAME_SET_DATA_BYTE(frame_,idx_,byte_)	\
			    frame_.data[idx_] = byte_;
				 



//---------------------------------------------------------------------------

static bool		     lnkDoSync(unsigned long tout = 20000);
static unsigned char lnkCrc(unsigned char * start,unsigned char * end);
static bool			 lnkGetAck(int count=10,int timeout=5);
static void			 lnkSendFrame(frameDataType * framePtr);
static bool			 lnkSendCommand(unsigned char cmd,unsigned int address);
static bool          lnkVerifyAnswer(
						const char  * data,
						unsigned long address
					 );
static bool			 lnkCleanOperational( 
						unsigned long address,
						unsigned int  pageSize,
						int		      pageDivider
					 );
static bool			 lnkMarkOperational( 
						unsigned long address,
						unsigned int  pageSize,
						int		      pageDivider,
						int           marker
					 ); 
static bool	         lnkSendData( 
						 unsigned int  pageSize,
						 int		   pageDivider,
						 bool		   verify,
						 unsigned long markAddress,
						 int		   marker
					 );

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

static unsigned char lnkCrc(unsigned char * start,unsigned char * end)
{
   unsigned char crc = 0;


   while(start < end){
     crc   ^= *start;
	 start ++;
   }
   
   return crc;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

static void lnkSendFrame(frameDataType * framePtr)
{
    comm_Write((char*)framePtr,framePtr->length+4);
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

static bool lnkGetAck(int count,int timeout)
{
	char buffer[10];
    int  cnt;
	int  ii;
	int  jj;



	for(jj = 0; jj < count;jj++){
		cnt = comm_Read(buffer,sizeof(buffer)-1,timeout);

		for(ii= 0; ii <cnt;ii++){
			if(buffer[ii] == 'A'){
				return true;
			}
		}	
	}

	return false;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

static bool			 lnkSendCommand(unsigned char cmd,unsigned int address)
{
	bool			 result = false;
	frameDataType	 cmdFrame;
	int				 kk;

	// Prepare the frame

	FRAME_INIT(cmdFrame,2,cmd);
    FRAME_SET_DATA_BYTE(cmdFrame,0,(unsigned char)(address & 0x00ff));
	FRAME_SET_DATA_BYTE(cmdFrame,1,(unsigned char)((address >> 8) & 0x00ff));
	FRAME_SET_CRC(cmdFrame);

	for(kk = 0; kk < 5; kk++){

       lnkSendFrame(&cmdFrame);

	   if(lnkGetAck() == true){			     
		   result = true;
		   break;
	   }
	}

    return result;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool lnkDoSync(unsigned long tout)
{
	int				 ii;
	unsigned long    endTime;
	bool			 result = false;
    frameDataType	 syncFrame;

	
	FRAME_INIT(syncFrame,2,'F');
    FRAME_SET_DATA_BYTE(syncFrame,0,'0');
	FRAME_SET_DATA_BYTE(syncFrame,1,'0');
	FRAME_SET_CRC(syncFrame);

    cout << "Wait for sync     -> ";

	for(ii = 0; ii < 20; ii++){

		cout <<"*";

		endTime = GetTickCount() + tout / 20;
		
		while(GetTickCount() < endTime)
		{
           lnkSendFrame(&syncFrame);
		   if(lnkGetAck(1,1) == true){
			   result = true;
			   goto outerLoop;
		   }
		}
		;		
	}

outerLoop:

    if(result == true){
		cout << "\nWait for sync     -> Sync found,boot loader activated\n";
	}else
	{
		cout << "\nWait for sync     -> Sync not found\n";
	}

	return result;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

static bool   lnkVerifyAnswer(const char * data,unsigned long size)
{
	char buffer[256];
	bool result;
	int  cnt;
	int  ii;
	int  idx = 0;



	for(ii = 0; ii < 10; ii++){
	  cnt = comm_Read(&buffer[idx],max(1,sizeof(buffer)-idx),5);
	  idx += cnt;

	  if(idx >= (size+1)){
		  break;
	  }
    }

	if( idx >= (size+1) ){
		if( (buffer[size] == 'A') && (memcmp(buffer,data,size) == 0) ){
			result = true;
		}else
		{
			result = false;	
		}
	}else
	{
		result = false;
	}

	return result;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

static bool	lnkSendData( unsigned int  pageSize,
						 int		   pageDivider,
						 bool		   verify,
						 unsigned long markAddress,
						 int		   marker
){
  
  unsigned long min;
  unsigned long max;
  unsigned long	address;
  unsigned long size;
  unsigned long	pageAddress;
  unsigned long bankOffset = 0;  
  unsigned long pageMin;
  unsigned long pageMax;
  unsigned long pageMarker;
  int           ii;
  int			jj;
  int           kk;
  int			attempt;
  bool		    result = false;
  frameDataType	dataFrame;
  frameDataType	verifyFrame;


  assert( (pageSize % pageDivider) == 0 );


  // Mark old application as 'obsolete'

  if(lnkCleanOperational(markAddress,pageSize,pageDivider) != true){
		cout << "\nOperation error   -> Application marker cannot be erased\n";
		goto outerLoop;
  }


  //Prepare new application

  FRAME_INIT(dataFrame,(pageSize/pageDivider) + 2,0);
 
  
  // Get data range
  min = hex_GetMinAddress();
  max = hex_GetMaxAddress();

  // Calculate page 
  pageMin    = (min / pageSize);
  pageMax    = (max / pageSize);
  pageMarker = (markAddress / pageSize);
  size       = pageSize/pageDivider;

  if( pageMax >= pageMarker){
		cout << "\nOperation error   -> Application overlaps marker page\n";
		goto outerLoop;  
  }

  


  cout << "Write/verify data -> 0%";

  for(ii = pageMin; ii <= pageMax;ii++){

	  if( (pageMax-pageMin) > 0){		  
			  cout << "\rWrite/verify data -> ";
			  cout << (int)(  (100*(ii-pageMin)) / (pageMax-pageMin) ) << "%";
	  }

	  pageAddress = ii *  pageSize; 	


	  // Set proper flash bank
	  if(pageAddress >= 1024 * 64){
			// We reached high flash bank (Mega128 specific)
			if(bankOffset == 0){
			   // Change flash bank
			   bankOffset = 1024 * 64;
			   if(lnkSendCommand('C',0x01) != true){
					cout << "\nOperation error   -> Bank change command error\n";
					goto outerLoop;
				}
			}
	  }// Set proper flash bank

      // Erase page first
	  if(lnkSendCommand('A',pageAddress) != true){
		cout << "\nOperation error   -> Page erase command error\n";
		goto outerLoop;
	  }// Erase page first

	  // Fill temporary buffer 
	  for(jj = 0; jj < pageDivider; jj++){

		 address = pageAddress + jj * size;


		 FRAME_SET_DATA_BYTE(dataFrame,0,(unsigned char)((address - bankOffset) & 0x00ff));
		 FRAME_SET_DATA_BYTE(dataFrame,1,(unsigned char)(((address - bankOffset) >> 8) & 0x00ff));
		 
		 for(kk = 0; kk < size;kk++){ 
			 FRAME_SET_DATA_BYTE( dataFrame,kk+2,*hex_GetData(address + kk) );
		 }

		 FRAME_SET_CRC(dataFrame);
	 
		 // Send data frame
		 // In case of errors resend it
		 for(attempt = 0; attempt < 5; attempt++){
		   lnkSendFrame(&dataFrame);

		   if(lnkGetAck()== true){
		     break;
		   }
		 }// Fill temporary buffer 
		 

		 if(attempt == 5){
		   // Failure	
		   cout << "\nOperation error   -> Page fill command error\n";
           goto outerLoop;
		 }

	  }// Fill temporary buffer



      // Write page
	  if(lnkSendCommand('B',pageAddress) != true){
		cout << "\nOperation error   -> Page write command error\n";
		goto outerLoop;
	  }// Write page 



	  // Verify page
	  if(verify == true){
		  for(jj = 0; jj < pageDivider; jj++){
			 
			 address = pageAddress + jj * size;

			 FRAME_INIT(verifyFrame,2,'E');
		     FRAME_SET_DATA_BYTE(verifyFrame,0,(unsigned char)((address - bankOffset)& 0x00ff));
			 FRAME_SET_DATA_BYTE(verifyFrame,1,(unsigned char)(((address -bankOffset) >> 8) & 0x00ff));
			 FRAME_SET_CRC(verifyFrame);

			 for(attempt = 0; attempt < 5; attempt++){
				lnkSendFrame(&verifyFrame);

				if(lnkVerifyAnswer(hex_GetData(address),size) == true){
					break;					
				}
			 }

			 if(attempt == 5){
				cout << "\nOperation error   -> Page verify command error\n";
				goto outerLoop;
			 }
		  }
	  }// Verify page
  }

  cout << "\n";

  //Mark program as operational
  if(lnkMarkOperational(markAddress,pageSize,pageDivider,marker) != true){
		cout << "Operation error   -> Application activation error\n";
		goto outerLoop;
  }//Mark program as operational

  // Start application
  if(lnkSendCommand('D',0) != true){
		cout << "Operation error   -> Cannot start the application\n";
		goto outerLoop;
  }// Start application
 

  result = true;


outerLoop:


  return result;	
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

static bool	  lnkCleanOperational( unsigned long address,
			 					   unsigned int  pageSize,
								   int		     pageDivider
){

	unsigned int  pageAddress;
	bool          bankChanged = false;


	// Make possible bank change
	if( (address >= 1024*64) && (bankChanged == false) ){
		address    -= 1024*64;
		bankChanged = true;

		if(lnkSendCommand('C',0x01) != true){
			cout << "\nOperation error   -> Bank change command error\n";
	        return false;
		}
	}

	pageAddress = (address / pageSize) * pageSize;

	// Erase the page with the marker
	if(lnkSendCommand('A',pageAddress) != true){
		cout << "\nOperation error   -> Page erase command error\n";
		return false;
	}

	// Change back the bank
    if(bankChanged == true){
		if(lnkSendCommand('C',0x00) != true){
			cout << "\nOperation error   -> Bank change command error\n";
	        return false;
		}
	}




	return true;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

static bool   lnkMarkOperational(unsigned long address,
								 unsigned int  pageSize,
								 int		   pageDivider,
								 int           marker
){
    frameDataType frame;

	unsigned int  frameAddress;
	unsigned int  frameSize;
	unsigned int  markerOffset;
	int			  attempt;
	int           kk;

	// Note : it is assumed here that the page 
	//        is not occupied by the application code

	// Make possible bank change
	if( address >= 1024*64){
		address  -= 1024*64;
		if(lnkSendCommand('C',0x01) != true){
			cout << "\nOperation error   -> Bank change command error\n";
			return false;
		}
	}

	// Prepare frame for the 'operational' marker
	frameSize	  = (pageSize/pageDivider);
	frameAddress  = (address / frameSize) * frameSize;
    markerOffset  = address - frameAddress;


	FRAME_INIT(frame,frameSize + 2,'0');
    FRAME_SET_DATA_BYTE(frame,0,(unsigned char)(frameAddress& 0x00ff));
    FRAME_SET_DATA_BYTE(frame,1,(unsigned char)((frameAddress >> 8) & 0x00ff));

	for(kk = 2; kk < frameSize + 2;kk++){ 
		 FRAME_SET_DATA_BYTE( frame,kk+2, 0xff );
	}

    FRAME_SET_DATA_BYTE(frame,markerOffset+2  ,(marker >> 8) & 0x00ff);
    FRAME_SET_DATA_BYTE(frame,markerOffset+2+1,(marker) & 0x00ff);

    FRAME_SET_CRC(frame);

	

	// Fill temporary buffer 
	for(attempt = 0; attempt < 5; attempt++){
	  lnkSendFrame(&frame);
	  if(lnkGetAck()== true){
			break;	
	  }
	}
	
	if(attempt == 5){
		cout << "\nOperation error   -> Page fill command error\n";
		return false;
	}

	// Write page
	if(lnkSendCommand('B',frameAddress) != true){
		cout << "\nOperation error   -> Page write command error\n";
		return false;
	}

	return true;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool    lnk_sendAvrFile(bool verify)
{
	bool result = false;

	if(lnkDoSync() == true){
		// My version for Mega128 with 1k boot loader
		result = lnkSendData(256,4,true,0x0001fbfe,0xabba);
	}

	return result;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------