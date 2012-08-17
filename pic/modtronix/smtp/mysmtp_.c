/*********************************************************************
*
*       SMTP client protocol state machine
*
*********************************************************************
* FileName:        mysmtp.c
* Dependencies:    mysmtp.h
*                  StackTsk.h
*                  Tick.h
*
* Processor:       PIC18
* Complier:        MCC18 v1.00.50 or higher
*                  HITECH PICC-18 V8.10PL1 or higher
* Company:         JM DELPRAT - TOULOUSE - FRANCE
*
***********************************************************************
*
* This module implements an SMTP client state machine
* It will send a mail via a nearby SMTP server
* whose IP adress must be programmed in the mysmtp.h file
* it makes ARP resolving of the SMTP server macadress
* the client mode must be enabled in the stack
* it has been tested only with a LOCAL SMTP server running on a PC
* in the same subnet.
* (the local SMTP server then forwards the emails if needed...)
*
* ADRESS OF SMTP SERVER MUST BE PROGRAMMED IN mysmtp.h
*
* Author 	  Date         Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* JM DELPRAT	15/11/04	start of writing the module on CBuilder
*        	21/11/04 	module working on CBuilder
*		05/12/04	moving to CC18, first testing
*				compiler storage class changed to auto (was overlay)
*				because ARP routines conflicts. (more free ram, less rom)
*	 	13/12/04	first mail sent !
*		22/12/04	added retry feature after timeout to have reliable
*				operation.
*
********************************************************************/
//
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>

/*
 * These headers must be included for required defs.
 */

#include "stacktsk.h"
#include "tick.h"
#include "mac.h"

#if defined(STACK_USE_DHCP)
#include "dhcp.h"
#endif

#if defined(STACK_USE_HTTP_SERVER)
#include "http.h"
#endif

#include "mpfs.h"

#if defined(STACK_USE_FTP_SERVER) && defined(MPFS_USE_EEPROM)
#include "ftp.h"
#endif

#if defined(MPFS_USE_EEPROM)
#include "xeeprom.h"
#endif


// For debug only.

#include "ARPTsk.h"
#include "tcp.h"
#include "icmp.h"
#include "delay.h"


#include "mysmtp.h"

//---------------------------------------------------------------------------
// external Functions prototypes:

 //this function must return the adress of one line of text to be included
//in the email :
extern char* getptrfromrsbuff(BYTE buffer);

//Write a char on serial port (DEBUG ONLY)
extern void USARTPut(BYTE c);

//---------------------------------------------------------------------------
// Local Functions prototypes:


//---------------------------------------------------------------------------

#define SMTP_PORT 25
#define	NRETRY 	3						// number of retries for sending mail


// SMTP commands strings :

//ROM char *SMTPCommandString[] =			// this way uses pointer in RAM !!!!
ROM char SMTPCommandString[][16] =			// this one, no.
{
    "HELO ",
    "MAIL FROM: ",
    "RCPT TO: ",
    "DATA\r\n",
    "From: ",
    "To: ",
    "Subject:",
    "info alarme :",
    ".\r\n",
    "QUIT\r\n"
};

// email adresses / subject variables :

ROM	char   	host_name[]=		"SBC44" ;
ROM	char 	crlf[]=				{0x0D,0x0A,0x00};
ROM	char	from_mail[] = 		"foobar@free.fr";
ROM	char	to_mail[]=			"barfoo@free.fr";
ROM	char	subject[]=			"Alarm information";
//ROM	char	test_message[]=		"hello world";

BYTE	c;
SM_SMTP	smSMTP;  			// SMTP server FSM state

BOOL	fsend_mail ;			// demande d'envoi du mail
TICK	lastActivity ;
TCP_SOCKET SMTPSocket ;
NODE_INFO nodedist ;
BYTE	cptretry ;			// retry counter


//-------------- write a string from ROM to the socket ----------------

void my_socket_write_from_ROM(ROM char* RpMsg) {
   BYTE v;

   while( (v = *RpMsg++) )
   {
//      USARTPut(v);			//debug only
      TCPPut(SMTPSocket, v);		//PIC stack only

   }
}

//----------- write a string from RAM to the socket ------------------

void my_socket_write(char* pMsg) {
   BYTE v;

   while( (v = *pMsg++) )
   {
//      USARTPut(v);			//debug only
      TCPPut(SMTPSocket, v);		//PIC stack only
   }
}

//-------------- SMTP INIT ------------------------------------------

/*********************************************************************
 * Function:        void SMTPInit(void)
 *
 * PreCondition:    TCP module is already initialized.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initializes internal variables of SMTP client
 *		    This function must be called at init time (just once)
 *
 * Note:
 ********************************************************************/
void SMTPInit(void)							// called from Websrvr.c
{
  smSMTP = SM_SMTP_STDBY;					// standby state for SMTP machine

  nodedist.IPAddr.v[0] = SMPT_SRVR1;		// writes the SMTP server IP
  nodedist.IPAddr.v[1] = SMPT_SRVR2;
  nodedist.IPAddr.v[2] = SMPT_SRVR3;
  nodedist.IPAddr.v[3] = SMPT_SRVR4;

}

/*********************************************************************
 * Function:        void SMTPSend(void)
 *
 * PreCondition:    SMTP module is already initialized.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Triggers sending of mail
 *		    This function must be called when the application
 *		    Wants to send a mail.
 * Note:
 ********************************************************************/
void SMTPSend()
{
   if (smSMTP == SM_SMTP_STDBY) {	// check if standby (do not overtrigger)
      lastActivity = TickGet();					// init delay before sending
      cptretry = NRETRY ;							// retry count
      fsend_mail = TRUE ;						// flag for State machine !
      USARTPut(0xAA) ;
   }
}




//-------------- SMTP COMMAND EXECUTE -------------------------------


// various actions called from the state machine
BOOL ExecuteSMTPCommand(SMTP_COMMAND cmd) {
ROM char* RpMsg;	// pointeur de message en ROM
    char* pMsg;		// pointeur de message en RAM
 BYTE i ;

    switch(cmd)
    {
    case SMTP_CMD_HELO:
            RpMsg = SMTPCommandString[cmd] ;
            my_socket_write_from_ROM(RpMsg) ;
            my_socket_write_from_ROM(host_name) ;
            my_socket_write_from_ROM(crlf) ;
            TCPFlush(SMTPSocket);
    break;
    case SMTP_CMD_FROM:
            RpMsg = SMTPCommandString[cmd];
            my_socket_write_from_ROM(RpMsg) ;
            my_socket_write_from_ROM(from_mail) ;
            my_socket_write_from_ROM(crlf) ;
            TCPFlush(SMTPSocket);
    break;
    case SMTP_CMD_TO:
            RpMsg = SMTPCommandString[cmd];
            my_socket_write_from_ROM(RpMsg) ;
            my_socket_write_from_ROM(to_mail) ;
            my_socket_write_from_ROM(crlf) ;
            TCPFlush(SMTPSocket);
    break;
    case SMTP_CMD_DATA:
            RpMsg = SMTPCommandString[cmd];
            my_socket_write_from_ROM(RpMsg) ;
            TCPFlush(SMTPSocket);
    break;
    case SMTP_CMD_DATA_HEADERS:
            RpMsg = SMTPCommandString[SMTP_CMD_DATA_HEADERS];
            my_socket_write_from_ROM(RpMsg) ;
            my_socket_write_from_ROM(from_mail) ;
            my_socket_write_from_ROM(crlf) ;
            RpMsg = SMTPCommandString[SMTP_CMD_DATA_HEADERS2];
            my_socket_write_from_ROM(RpMsg) ;
            my_socket_write_from_ROM(to_mail) ;
            my_socket_write_from_ROM(crlf) ;
            RpMsg = SMTPCommandString[SMTP_CMD_DATA_HEADERS3];
            my_socket_write_from_ROM(RpMsg) ;
            my_socket_write_from_ROM(subject) ;
            my_socket_write_from_ROM(crlf) ;

            TCPFlush(SMTPSocket);
    break;
    case SMTP_CMD_DATA_MESSAGE:
            my_socket_write_from_ROM(crlf) ;
            RpMsg = SMTPCommandString[cmd];
            my_socket_write_from_ROM(RpMsg) ;
            my_socket_write_from_ROM(crlf) ;

//          pMsg = test_message ;		// test message (debug)
//          my_socket_write(pMsg) ;
//          my_socket_write_from_ROM(crlf) ;

             for(i=0;i<5;i++){			// here we send the message text
               pMsg = getptrfromrsbuff(i) ;     // made of 5 lines coming from
               my_socket_write(pMsg) ;		// serial port. (printer log)
               my_socket_write_from_ROM(crlf) ;
            }

            TCPFlush(SMTPSocket);
    break;
    case SMTP_CMD_DATA_END:
            RpMsg = SMTPCommandString[cmd];
            my_socket_write_from_ROM(RpMsg) ;
            TCPFlush(SMTPSocket);
    break;
    case SMTP_CMD_QUIT:
            RpMsg = SMTPCommandString[cmd];
            my_socket_write_from_ROM(RpMsg) ;
            TCPFlush(SMTPSocket);
    break;
    }
    return TRUE;
}



/*********************************************************************
 * Function:        void SMTP Client State machine(void)
 *
 * PreCondition:    FTPInit() must already be called.
 *
 * Input:           None
 *
 * Output:          Ready to send mail.
 *
 * Side Effects:    None
 *
 * Overview:
 *
 * Note:            This function acts as a task (similar to one in
 *                  RTOS).  This function performs its task in
 *                  co-operative manner.  Main application must call
 *                  this function repeatedly to ensure it can send
 *                  mails when requested. (include in the main loop)
 ********************************************************************/
void SMTPClient(void)
{
    BYTE v;
//    TICK currentTick;

    // check if state machine is stuck somewhere and reset the SM after a while if needed :
    if ((smSMTP != SM_SMTP_STDBY) && (TickGetDiff(TickGet(), lastActivity) > (15 * TICK_SECOND))) {
        if (TCPIsConnected(SMTPSocket))
           TCPDisconnect(SMTPSocket) ;
        if(cptretry--) {						// if not all retries done...
           lastActivity = TickGet();			// re-init delay
           smSMTP = SM_SMTP_STDBY ;   			// force standby state
        } else {
           fsend_mail = FALSE ;					// give up !
           smSMTP = SM_SMTP_STDBY ;   			// -> standby
        }
    }

    // work each state :
    switch(smSMTP)
    {
    case SM_SMTP_STDBY:		// standby: idle, waiting for connection request
        if (fsend_mail) {
           if (TickGetDiff(TickGet(), lastActivity) > (10 * TICK_SECOND)) {
      USARTPut(0xBB) ;

              lastActivity = TickGet();
              ARPResolve(&nodedist.IPAddr) ;	// resolve IP adress
              smSMTP = SM_SMTP_ARP ;   			// -> wait ARP answer
           }
        }
        break ;

    case SM_SMTP_ARP:		// wait ARP to be resolved
        if ( ARPIsResolved(&nodedist.IPAddr, &nodedist.MACAddr)) {
           SMTPSocket = TCPConnect(&nodedist, SMTP_PORT) ;
           if (SMTPSocket == INVALID_SOCKET) {
              fsend_mail = FALSE ;	       		// avorte
           } else {
              smSMTP = SM_SMTP_CONNECT ;   		// -> attente ACK
           }
        }
        break ;

    case SM_SMTP_CONNECT:		// standby: attente ack connexion
        if (TCPIsConnected(SMTPSocket)) {
           smSMTP = SM_SMTP_WELCOME ;   		// -> attente WELCOME

        }
        break ;

    case SM_SMTP_WELCOME:		// attente welcome du serveur
        if (TCPIsGetReady(SMTPSocket)) {
           if (TCPGet(SMTPSocket, &v)) {
              if (v == '2') {		// commence par un 2 ? (220..)
                 TCPDiscard(SMTPSocket) ;
                 ExecuteSMTPCommand(SMTP_CMD_HELO) ;
                 smSMTP = SM_SMTP_HELO ;   		// -> attente reponse au HELO
              }else {
              smSMTP = SM_SMTP_DONE ;   		// -> disconnect
              }
           }
        }
        break ;

    case SM_SMTP_HELO:			// attente HELO du serveur
        if (TCPIsGetReady(SMTPSocket)) {
           if (TCPGet(SMTPSocket,&v)) {
              if (v == '2') {					// commence par un 2 ? (220..)
                 TCPDiscard(SMTPSocket) ;
                 ExecuteSMTPCommand(SMTP_CMD_FROM) ;
                 smSMTP = SM_SMTP_FROM ;   		// -> attente reponse au FROM
              }else {
                 smSMTP = SM_SMTP_DONE ;   		// -> disconnect
              }
           }
        }
        break ;

    case SM_SMTP_FROM:			// attente HELO du serveur
        if (TCPIsGetReady(SMTPSocket)) {
           if (TCPGet(SMTPSocket,&v)) {
              if (v == '2') {		// commence par un 2 ? (220..)
                 TCPDiscard(SMTPSocket) ;
                 ExecuteSMTPCommand(SMTP_CMD_TO) ;
                 smSMTP = SM_SMTP_TO ;   		// -> attente reponse au TO
              }else {
                 smSMTP = SM_SMTP_DONE ;   		// -> disconnect
              }
           }
        }
        break ;

    case SM_SMTP_TO:			// attente HELO du serveur
        if (TCPIsGetReady(SMTPSocket)) {
           if (TCPGet(SMTPSocket,&v)) {
              if (v == '2') {					// commence par un 2 ? (220..)
                 TCPDiscard(SMTPSocket) ;
                 ExecuteSMTPCommand(SMTP_CMD_DATA) ;
                 smSMTP = SM_SMTP_DATA1 ;  		// -> attente reponse au DATA
              }else {
                 smSMTP = SM_SMTP_DONE ;   		// -> disconnect
              }
           }
        }
        break ;

    case SM_SMTP_DATA1:			// when OK send message headers
        if (TCPIsGetReady(SMTPSocket)) {
           if (TCPGet(SMTPSocket,&v)) {
              if (v == '3') {		// commence par un 3 ? (220..)
                 TCPDiscard(SMTPSocket) ;
                 ExecuteSMTPCommand(SMTP_CMD_DATA_HEADERS) ;	// send headers
//                 ExecuteSMTPCommand(SMTP_CMD_DATA_MESSAGE) ; // message
//                 ExecuteSMTPCommand(SMTP_CMD_DATA_END) ;     // termine

                 smSMTP = SM_SMTP_DATA2;   		// -> send body
              }else {
                 smSMTP = SM_SMTP_DONE ;   		// -> disconnect
              }
           }
        }
        break ;

    case SM_SMTP_DATA2:			// wait to send message body
        if (TCPIsPutReady(SMTPSocket)) {				// wait for TX buffer free
           ExecuteSMTPCommand(SMTP_CMD_DATA_MESSAGE) ;	// message
           smSMTP = SM_SMTP_DATA3 ;   		// -> attente reponse au TO
        } else {
//           USARTPut(0xCC) ;		vérifié qu'il y avait bien besoin d'une attente ici

        }
        break ;

    case SM_SMTP_DATA3:			// wait to send the final "."
        if (TCPIsPutReady(SMTPSocket)) {				// wait for TX buffer free
           ExecuteSMTPCommand(SMTP_CMD_DATA_END) ; 		// termine
           smSMTP = SM_SMTP_QUIT ;						// -> end
        }  else {
//            USARTPut(0xDD) ;		vérifié qu'il y avait bien besoin d'une attente ici

        }
        break ;

    case SM_SMTP_QUIT:		// wait last message before leaving...
        if (TCPIsGetReady(SMTPSocket)) {
           if (TCPGet(SMTPSocket,&v)) {
              if (v == '2') {		// commence par un 2 ? (220..)

                 TCPDiscard(SMTPSocket) ;
                 smSMTP = SM_SMTP_DONE ;   		// -> deconnecte
              }else {
                 smSMTP = SM_SMTP_DONE ;   		// -> disconnect
              }
           }
        }
        break ;


    case SM_SMTP_DONE:			// disconnect Socket :
        if (TCPIsConnected(SMTPSocket) && (TCPIsPutReady(SMTPSocket))) { // wait for TX buff free
           TCPDisconnect(SMTPSocket) ;
        }
        fsend_mail = FALSE ;		// done !
        smSMTP = SM_SMTP_STDBY ;   // -> standby
        break ;
    }
}





