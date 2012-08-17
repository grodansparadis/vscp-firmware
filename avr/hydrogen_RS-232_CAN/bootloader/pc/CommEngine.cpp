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
#include <stdio.h>

#include "CommEngine.h"

//---------------------------------------------------------------------------


static OVERLAPPED    osWrite, osRead ;
static HANDLE        idComDev = (HANDLE) -1;
static COMMTIMEOUTS  CommTimeOuts ;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool comm_Init(void)
{
   // create I/O event used for overlapped reads / writes

   osRead.hEvent = CreateEvent( NULL,    // no security
                                TRUE,    // explicit reset req
                                FALSE,   // initial event reset
                               NULL ) ; // no name
   if (osRead.hEvent == NULL)
   {
      return false ;
   }

   osWrite.hEvent = CreateEvent( NULL,    // no security
                                 TRUE,    // explicit reset req
                                 FALSE,   // initial event reset
                                 NULL
                    ) ; // no name

   if (NULL == osWrite.hEvent)
   {
      CloseHandle( osRead.hEvent ) ;
      return false ;
   }

   return true;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void comm_Release(void)
{
     if(idComDev != (HANDLE) -1){
        comm_Close();
     }

     CloseHandle( osRead.hEvent ) ;
     CloseHandle( osWrite.hEvent ) ;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool comm_Open( int commNr )
{
   char       szPort[ 15 ];
   bool       fRetVal ;


   sprintf( szPort, "COM%d", commNr) ;

   // open COMM device
   if ((idComDev  =
        CreateFile( szPort, GENERIC_READ | GENERIC_WRITE,
                  0,                    // exclusive access
                  NULL,                 // no security attrs
                  OPEN_EXISTING,
                  FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                  NULL )) == (HANDLE) -1 )
        fRetVal = false ;
   else
   {
      // get any early notifications

      SetCommMask( idComDev , EV_RXCHAR ) ;

      // setup device buffers

      SetupComm( idComDev , 4096, 4096 ) ;

      // purge any information in the buffer

      PurgeComm( idComDev , PURGE_TXABORT | PURGE_RXABORT |
                            PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

      fRetVal = true;
   }

   return ( fRetVal ) ;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool comm_Setup(int baudrate,int parity,int stopbits)
{
   BOOL          fRetVal ;
   DCB           dcb ;


   // set up for overlapped I/O

   CommTimeOuts.ReadIntervalTimeout        = MAXDWORD ;
   CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
   CommTimeOuts.ReadTotalTimeoutConstant   = 0 ;

   // CBR_9600 is approximately 1byte/ms. For our purposes, allow
   // double the expected time per character for a fudge factor.
   CommTimeOuts.WriteTotalTimeoutMultiplier = 2*CBR_9600/baudrate;
   CommTimeOuts.WriteTotalTimeoutConstant = 0 ;

   SetCommTimeouts( idComDev , &CommTimeOuts ) ;

   dcb.DCBlength = sizeof( DCB ) ;

   GetCommState( idComDev, &dcb ) ;

   dcb.BaudRate = baudrate;
   dcb.ByteSize = 8;
   dcb.Parity   = (char)parity;
   dcb.StopBits = (char)stopbits;

   // setup hardware flow control for RS485
   // Note: somehow it does not work, so we will do manually later

   dcb.fOutxCtsFlow = 0;
   dcb.fOutxDsrFlow = 0;
   dcb.fDtrControl  = DTR_CONTROL_DISABLE;
   dcb.fRtsControl  = RTS_CONTROL_DISABLE;

   // setup software flow control

   dcb.XonChar  = 0x11;
   dcb.XoffChar = 0x13;
   dcb.XonLim   = 100 ;
   dcb.XoffLim  = 100 ;

   // other various settings

   dcb.fBinary = TRUE ;
   dcb.fParity = TRUE ;

   fRetVal = SetCommState( idComDev, &dcb ) ;

   return ( fRetVal != 0 ) ;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


bool comm_Close(void)
{
   // disable event notification and wait for thread
   // to halt

   SetCommMask( idComDev, 0 ) ;

   // purge any outstanding reads/writes and close device handle

   PurgeComm( idComDev, PURGE_TXABORT | PURGE_RXABORT |
                        PURGE_TXCLEAR | PURGE_RXCLEAR
   ) ;

   CloseHandle( idComDev ) ;
   idComDev = (HANDLE) -1;

   return ( TRUE ) ;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

int comm_Read(char * lpBlock, int nMaxLength, int timeout )
{
   BOOL       fReadStat ;
   COMSTAT    ComStat ;
   DWORD      dwErrorFlags;
   DWORD      dwLength;
   DWORD      dwError;


   // Setup reading timeout

   if(CommTimeOuts.ReadTotalTimeoutConstant != (unsigned int)timeout){
      CommTimeOuts.ReadTotalTimeoutConstant = timeout;
      SetCommTimeouts( idComDev , &CommTimeOuts ) ;
   }

   fReadStat = ReadFile( idComDev, lpBlock,nMaxLength, &dwLength, &osRead ) ;

   if (!fReadStat)
   {
     if (GetLastError() == ERROR_IO_PENDING)
     {
         while(!GetOverlappedResult( idComDev,&osRead, &dwLength, TRUE ))
         {
               dwError = GetLastError();
               if(dwError == ERROR_IO_INCOMPLETE)
                  // normal result if not finished
                  continue;
               else
               {
                  ClearCommError( idComDev, &dwErrorFlags, &ComStat ) ;
                  break;
               }
         }
	 }else
     {
         dwLength = 0 ;
         ClearCommError( idComDev, &dwErrorFlags, &ComStat ) ;
     }
   }


   return ( dwLength );
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool comm_Write(char * lpBlock , int dwBytesToWrite)
{

   BOOL        fWriteStat ;
   DWORD       dwBytesWritten ;
   DWORD       dwErrorFlags;
   DWORD       dwError;
   DWORD       dwBytesSent=0;
   COMSTAT     ComStat;

   fWriteStat = WriteFile( idComDev, lpBlock, dwBytesToWrite,&dwBytesWritten, &osWrite ) ;

   if (!fWriteStat)
   {
      if(GetLastError() == ERROR_IO_PENDING)
      {

         while(!GetOverlappedResult( idComDev,&osWrite, &dwBytesWritten, TRUE ))
         {
            dwError = GetLastError();
            if(dwError == ERROR_IO_INCOMPLETE)
            {
               // normal result if not finished
               dwBytesSent += dwBytesWritten;
               continue;
            }
            else
            {
               ClearCommError( idComDev, &dwErrorFlags, &ComStat ) ;
               break;
            }
         }

         dwBytesSent += dwBytesWritten;


      }
      else
      {
         // some other error occurred
         ClearCommError( idComDev, &dwErrorFlags, &ComStat ) ;
         return ( FALSE );
      }
   }

   return ( TRUE ) ;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void comm_Purge()
{
  PurgeComm( idComDev, PURGE_TXABORT | PURGE_RXABORT |
                       PURGE_TXCLEAR | PURGE_RXCLEAR
  ) ;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool  comm_WaitForChar(int timeout)
{
   DWORD       dwEvtMask;
   DWORD       dwLength;

   WaitCommEvent( idComDev, &dwEvtMask, &osRead );
   WaitForSingleObject(osRead.hEvent,timeout);

   if(GetOverlappedResult( idComDev,&osRead , &dwLength, FALSE ) == TRUE){
      if ((dwEvtMask & EV_RXCHAR) == EV_RXCHAR){
               return  1;
      }
   }

   return 0;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool  comm_CtrlPins(bool rts,bool dtr)
{
    bool result = true;

    if(rts == true){
       result &= (EscapeCommFunction( idComDev, SETRTS ) == TRUE );
    }else
    {
       result &= (EscapeCommFunction( idComDev, CLRRTS ) == TRUE );
    }

    if(dtr == true){
       result &= (EscapeCommFunction( idComDev, SETDTR ) == TRUE );
    }else
    {
       result &= (EscapeCommFunction( idComDev, CLRDTR ) == TRUE );
    }

    return result;
}
