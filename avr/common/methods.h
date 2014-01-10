#ifndef VSCP_METHODS_H
#define VSCP_METHODS_H

// Prototypes
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

int readEEPROM( uint8_t addr );
int writeEEPROM( uint8_t addr, uint8_t data );

uint8_t vscp_readNicknamePermanent( void );
void vscp_writeNicknamePermanent( uint8_t nickname );

uint8_t vscp_getZone( void );
uint8_t vscp_getSubzone( void );

void vscp_getEmbeddedMdfInfo( void );

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

uint8_t vscp_getBufferSize( void );

uint8_t vscp_getRegisterPagesUsed( void );

uint8_t vscp_getNickname( void );
void vscp_setNickname( uint8_t nickname );

uint8_t vscp_getSegmentCRC( void );
void vscp_setSegmentCRC( uint8_t crc );

void vscp_setControlByte( uint8_t ctrl );
uint8_t vscp_getControlByte( void );

void vscp_getMatrixInfo( char *pData );

// prototypes defined but no functions in .c

void SendInformationEvent( uint8_t idx, uint8_t eventClass, uint8_t eventTypeId );

/*
void SendInformationEventExtended
	(uint8_t priority, uint8_t zone, uint8_t subzone, uint8_t idx, 
	uint8_t eventClass, uint8_t eventTypeId );

*/
#endif
