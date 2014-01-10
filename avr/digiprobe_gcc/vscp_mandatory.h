/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*      based on work copyrighted 2006-2009 by Ake Hedman, eurosource          *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	All mandatory functions for the vscp protocol class
*******************************************************************************/

#ifndef _VSCP_MANDATORY_H_
#define _VSCP_MANDATORY_H_

//**************************************************************************
// Extended features in "Drop nickname / Reset" Event, Class 0, Type 7
//**************************************************************************
#ifdef DROP_NICKNAME_EXTENDED_FEATURES
void vscp_hardreset(void);
void vscp_wait_ms(uint16_t tmsec);
void vscp_wait_s(uint16_t tsec);
#endif

	void vscp_protocol_class(void);

	int readEEPROM( uint8_t addr );
	int writeEEPROM( uint8_t addr, uint8_t data );

	int8_t sendVSCPFrame( uint16_t vscpclass, 
	                        uint8_t vscptype, 
	                        uint8_t nodeid,
	                        uint8_t priority,
	                        uint8_t size, 
	                        uint8_t *pData );

	int8_t getVSCPFrame( uint16_t *pvscpclass, 
	                        uint8_t *pvscptype, 
	                        uint8_t *pNodeId,
	                        uint8_t *pPriority, 
	                        uint8_t *pSize, 
	                        uint8_t *pData );

	// Prototypes
	void SendInformationEvent( uint8_t idx, uint8_t eventClass, uint8_t eventTypeId );


	uint8_t vscp_getRegisterPagesUsed( void );

	uint8_t vscp_readAppReg( uint8_t reg );

	uint8_t vscp_writeAppReg( uint8_t reg, uint8_t val );

	uint8_t vscp_readNicknamePermanent( void );

	void vscp_writeNicknamePermanent( uint8_t nickname );

	uint8_t vscp_getZone( void );

	uint8_t vscp_getSubzone( void );

	void vscp_goBootloaderMode( void );

	uint8_t vscp_getMajorVersion();

	uint8_t vscp_getMinorVersion();

	uint8_t vscp_getSubMinorVersion();

	uint8_t vscp_getGUID( uint8_t idx );

	void vscp_setGUID( uint8_t idx, uint8_t data );

	uint8_t vscp_getMDF_URL( uint8_t idx );

	uint8_t vscp_getUserID( uint8_t idx );

	void vscp_setUserID( uint8_t idx, uint8_t data );

	uint8_t vscp_getManufacturerId( uint8_t idx );

	void vscp_setManufacturerId( uint8_t idx, uint8_t data );

	uint8_t vscp_getBootLoaderAlgorithm( void ) ;

	uint8_t vscp_getBufferSize( void ) ;

	uint8_t vscp_getNickname( void );

	void vscp_setNickname( uint8_t nickname );

	uint8_t vscp_getSegmentCRC( void );

	void vscp_setSegmentCRC( uint8_t crc );

	void vscp_setControlByte( uint8_t ctrl );

	uint8_t vscp_getControlByte( void );

	void vscp_getMatrixInfo( char *pData );

	#if DECISION_MATRIX_ELEMENTS > 0
	static void doDM( void );
	#endif

	#ifdef EMBEDDED_MDF
	void vscp_getEmbeddedMdfInfo( void );
	#endif

	///////////////////////////////////////////////////////////////////////////////

	void init_app_eeprom( void );
	
	uint32_t vscp_getFamilyCode(void);
	
	uint32_t vscp_getFamilyType(void);
	
	void vscp_restoreDefaults(void);

#endif
