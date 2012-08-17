/*********************************************************************
 *
 *                  Stack Manager for Microchip eTCP/IP Stack
 *
 *********************************************************************
 * FileName:        StackMgr.c
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended 
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip's PICmicro Microcontroller products. 
 * The software is owned by the Company and/or its supplier, and is
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
 * $Id: StackMgr.c,v 1.5 2006/09/28 22:10:26 C12770 Exp $
 *
 ********************************************************************/
 
#include "bsd_tcp_ip\pkt_queue.h"
#include "bsd_tcp_ip\ether.h"
#include "bsd_tcp_ip\block_mgr.h"
#include "bsd_tcp_ip\etcpcfg.h"
#include "bsd_tcp_ip\emac.h"
#include "bsd_tcp_ip\route.h"
#include "bsd_tcp_ip\eip.h"
#include "bsd_tcp_ip\gpfunc.h"

IP_ADDR     SysIPAddr;		// big endian
IP_ADDR 	SysIPMask;		// big-endian
IP_ADDR 	SysIPGateway;	// big-endian
MAC_ADDR    SysMACAddr;		// big-endian

extern PktQueue MACTxQueue;	//MAC Transmit Queue

void IP_RxHandler( NET_PKT *sbfr );
void ARP_RxHandler( NET_PKT *sbfr );
void eTCPInit();
void SocketLayerInit();
void TCPTimer();

#ifndef STACK_MGR_RX_PKT_CNT
#error "Please make sure that STACK_MGR_RX_PKT_CNT is defined in eTCP.def"
#endif

#if(STACK_MGR_RX_PKT_CNT == 0)
#error "Please make sure that STACK_MGR_RX_PKT_CNT is greater than zero in eTCP.def"
#endif

#if(STACK_MGR_RX_PKT_CNT > 255)
#error "Please make sure that STACK_MGR_RX_PKT_CNT is less than 256 in eTCP.def"
#endif

/*********************************************************************
 * Function:        void InitStackMgr(void)
 *
 * PreCondition:    None
 * Input:			None
 * Output:          None
 * Side Effects:    None
 * Overview:        Initialize internal stack variables
 * Note:
 ********************************************************************/
void InitStackMgr()
{
	InitBufferManager();
	InitHostRouteTable();
	InitIPLayer();
	eTCPInit();
	SocketLayerInit();
	MACInit();
}

/*********************************************************************
 * Function:        void StackMgrProcess(void)
 *
 * PreCondition:    None
 * Input:			None
 * Output:          None
 * Side Effects:    None
 * Overview:        Executes state machine and processes of stack
 * Note:
 ********************************************************************/
void StackMgrProcess()
{
NET_PKT *sbfr;
WORD protocol;
BYTE RxPktCnt = 0;

	//Process new packets Rx
	while( RxPktCnt++ < STACK_MGR_RX_PKT_CNT && (sbfr = MACGetRxPacket()) )
	{
		protocol = GetEtherFrameInfo( sbfr );

		if( protocol == ETH_PROTO_IP )
			IP_RxHandler( sbfr );
		else if( protocol == ETH_PROTO_ARP )
			ARP_RxHandler( sbfr );
		else
		{
		   	DeAllocateBuffer(sbfr);
		}
	}
	
	//resolve any pending ARPs
	IPProcess();
	
	//TCP Socket Timers
	TCPTimer();
	
	//process outgoing packets sitting in TxQueue
	MACTxProcess();

}
/*********************************************************************
 * Function:        TCPIPSetDefaultAddr       
 *
 * PreCondition:    Call before InitStackMgr   
 *
 * Input:           None          
 *
 * Output:          None
 *
 * Side Effects:    Sets the IP address, IP mask, IP Gateway and
 *                  MAC address to the default values defined in
 *                  etcp.def file.
 ********************************************************************/
void TCPIPSetDefaultAddr(void)
{
	// Initialize default IP and MAC address
	IP_StrToDWord( DEFAULT_IP_ADDR, 	&SysIPAddr.Val );	
	IP_StrToDWord( DEFAULT_IP_MASK, 	&SysIPMask.Val );	
	IP_StrToDWord( DEFAULT_IP_GATEWAY, 	&SysIPGateway.Val );	
	MAC_StrToBin ( DEFAULT_MAC_ADDR,	SysMACAddr.v );    
}
/*********************************************************************
 * Function:            TCPIPSetIPAddr      
 *
 * PreCondition:        Call before InitStackMgr   
 *
 * Input:               addr1 - addr4 Ip address in bytes
 *
 * Output:              None
 *
 * Side Effects:        Sets the IP address
 * 
 * Note:                Set the IP address to 10.10.33.201
 *                      TCPIPSetIPAddr( 10,
 *                                      10,
 *                                      33,
 *                                      201);
 ********************************************************************/
void TCPIPSetIPAddr(BYTE addr1, BYTE addr2, BYTE addr3, BYTE addr4)
{
    SysIPAddr.v[0] = addr1;
    SysIPAddr.v[1] = addr2;
    SysIPAddr.v[2] = addr3;
    SysIPAddr.v[3] = addr4;
    
}
/*********************************************************************
 * Function:            TCPIPSetIPAddrStr      
 *
 * PreCondition:        Call before InitStackMgr   
 *
 * Input:               ip_addr - Ip address in ASCII format
 *
 * Output:              None
 *
 * Side Effects:        Sets the IP address
 * 
 * Note:                Set the IP address to 10.10.33.201
 *                      TCPIPSetIPAddrStr( "10.10.33.201");
 ********************************************************************/
void TCPIPSetIPAddrStr(BYTE *ip_addr)
{
	IP_StrToDWord( ip_addr, 	&SysIPAddr.Val );	
}
/*********************************************************************
 * Function:            TCPIPGetIPAddr     
 *
 * PreCondition:        None
 *
 * Input:               None
 *
 * Output:              IP address as a 32 bit value
 *
 * Side Effects:        None
 * 
 * Note:                DWORD_VAL curr_ip =   TCPIPGetIPAddr();         
 ********************************************************************/
DWORD TCPIPGetIPAddr(void)
{
    return SysIPAddr.Val;
}
/*********************************************************************
 * Function:            TCPIPSetIPMask      
 *
 * PreCondition:        Call before InitStackMgr   
 *
 * Input:               addr1 - addr4 Ip mask in bytes
 *
 * Output:              None
 *
 * Side Effects:        Sets the IP mask
 * 
 * Note:                Set the IP mask to 255.255.255.201
 *                      TCPIPSetIPMask( 255,
 *                                      255,
 *                                      255,
 *                                      201);
 ********************************************************************/
void TCPIPSetIPMask(BYTE addr1, BYTE addr2, BYTE addr3, BYTE addr4)
{
    SysIPMask.v[0] = addr1;
    SysIPMask.v[1] = addr2;
    SysIPMask.v[2] = addr3;
    SysIPMask.v[3] = addr4;
    
}
/*********************************************************************
 * Function:            TCPIPSetIPMaskStr      
 *
 * PreCondition:        Call before InitStackMgr   
 *
 * Input:               ip_addr - Ip mask in ASCII format
 *
 * Output:              None
 *
 * Side Effects:        Sets the IP mask
 * 
 * Note:                Set the IP mask to 255.255.255.201
 *                      TCPIPSetIPMaskStr( "255.255.255.201");
 ********************************************************************/
void TCPIPSetIPMaskStr(BYTE *ip_addr)
{
	IP_StrToDWord( ip_addr, 	&SysIPMask.Val );	
}
/*********************************************************************
 * Function:            TCPIPGetIPMask     
 *
 * PreCondition:        None
 *
 * Input:               None
 *
 * Output:              IP mask as a 32 bit value
 *
 * Side Effects:        None
 * 
 * Note:                DWORD_VAL curr_ip =   TCPIPGetIPMask();         
 ********************************************************************/
DWORD TCPIPGetIPMask(void)
{
    return SysIPMask.Val;
}
/*********************************************************************
 * Function:            TCPIPSetIPGateway      
 *
 * PreCondition:        Call before InitStackMgr   
 *
 * Input:               addr1 - addr4 Ip gateway in bytes
 *
 * Output:              None
 *
 * Side Effects:        Sets the IP gateway
 * 
 * Note:                Set the IP gateway to 10.10.33.201
 *                      TCPIPSetIPGateway(  10,
 *                                          10,
 *                                          33,
 *                                          201);
 ********************************************************************/
void TCPIPSetIPGateway(BYTE addr1, BYTE addr2, BYTE addr3, BYTE addr4)
{
    SysIPGateway.v[0] = addr1;
    SysIPGateway.v[1] = addr2;
    SysIPGateway.v[2] = addr3;
    SysIPGateway.v[3] = addr4;
    
}
/*********************************************************************
 * Function:            TCPIPSetIPGatwayStr      
 *
 * PreCondition:        Call before InitStackMgr   
 *
 * Input:               ip_addr - Ip gateway in ASCII format
 *
 * Output:              None
 *
 * Side Effects:        Sets the IP gateway
 * 
 * Note:                Set the IP gateway to 10.10.33.201
 *                      TCPIPSetIPGatwayStr( "10.10.33.201");
 ********************************************************************/
void TCPIPSetIPGatwayStr(BYTE *ip_addr)
{
	IP_StrToDWord( ip_addr, 	&SysIPGateway.Val );	
}
/*********************************************************************
 * Function:            TCPIPGetIPGateway     
 *
 * PreCondition:        None
 *
 * Input:               None
 *
 * Output:              IP gateway as a 32 bit value
 *
 * Side Effects:        None
 * 
 * Note:                DWORD_VAL curr_ip =   TCPIPGetIPGateway();         
 ********************************************************************/
DWORD TCPIPGetIPGateway(void)
{
    return SysIPGateway.Val;
}
/*********************************************************************
 * Function:       
 *
 * PreCondition:    
 *
 * Input:           
 *
 * Output:          
 *
 * Side Effects:    
 *
 * Overview:        
 *
 * Note:           
 ********************************************************************/
void TCPIPSetMACAddr(BYTE addr1, BYTE addr2, BYTE addr3, BYTE addr4, BYTE addr5, BYTE addr6)
{
    SysMACAddr.v[0] = addr1;
    SysMACAddr.v[1] = addr2;
    SysMACAddr.v[2] = addr3;
    SysMACAddr.v[3] = addr4;
    SysMACAddr.v[4] = addr5;
    SysMACAddr.v[5] = addr6;
    
}
void TCPIPSetMACAddrStr(BYTE *mac_addr)
{
	MAC_StrToBin( mac_addr, 	SysMACAddr.v );	
}
/*********************************************************************
 * Function:       
 *
 * PreCondition:    
 *
 * Input:           
 *
 * Output:          
 *
 * Side Effects:    
 *
 * Overview:        
 *
 * Note:           
 ********************************************************************/
void TCPIPGetMACAddr(MAC_ADDR *mac_addr)
{
     mac_addr->v[0] = SysMACAddr.v[0];
     mac_addr->v[1] = SysMACAddr.v[1];
     mac_addr->v[2] = SysMACAddr.v[2];
     mac_addr->v[3] = SysMACAddr.v[3];
     mac_addr->v[4] = SysMACAddr.v[4];
     mac_addr->v[5] = SysMACAddr.v[5];
    
}
