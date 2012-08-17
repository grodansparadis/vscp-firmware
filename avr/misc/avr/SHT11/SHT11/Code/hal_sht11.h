/**
 * @file   
 * @author Alex Raimondi 
 * @date   8. 6. 2002
 * 
 * @brief  Low level function to interface an SHT11.
 * 
 * The SHT11 is a digital humidity & temperature sensmitter 
 * from Sensirion (www.sensirion.com).
 */
#ifndef SHT11_H
#define SHT11_H

#include <io.h>
#include <progmem.h>

#include "typedefs.h"
#include "mirror.h"

/** 
 * Port definitions for SHT11 SCK pin
 */
/*@{*/
#define SHT11_SCK_PORT	PORTB			///< Port register
#define SHT11_SCK_DDR	DDRB			///< Port direction register
#define SHT11_SCK_PIN	PINB			///< Port input register
#define SHT11_SCK_BIT	PB7				///< Bit in Port
/*@}*/

/** 
 * Port definitions for SHT11 SCK pin
 */
/*@{*/
#define SHT11_DATA_PORT	PORTB			///< Port register
#define SHT11_DATA_DDR	DDRB			///< Port direction register
#define SHT11_DATA_PIN	PINB			///< Port input register
#define SHT11_DATA_BIT	PB6				///< Bit in Port
/*@}*/

/** 
 * Macros to toggle port state of SCK line.
 */
/*@{*/
#define SHT11_SCK_LO()	cbi( SHT11_SCK_PORT, SHT11_SCK_BIT ) 
#define SHT11_SCK_HI()	sbi( SHT11_SCK_PORT, SHT11_SCK_BIT ) 
/*@}*/

/** 
 * Macros to toggle port state of DATA line.
 * 
 * Since DATA line has to be externaly pulled up by a resistor on initialisation
 * we have to write a 0 into port bit and toggle line by switching between 
 * input and output.
 */
/*@{*/
#define SHT11_DATA_LO()	sbi( SHT11_DATA_DDR, SHT11_DATA_BIT ) 
#define SHT11_DATA_HI()	cbi( SHT11_DATA_DDR, SHT11_DATA_BIT )
/*@}*/

/**
 * Macro to read DATA bit from port.
 */
#define SHT11_GET_BIT() (bit_is_set(SHT11_DATA_PIN, SHT11_DATA_BIT) > 0 ? 1 : 0) 

/*#define mirrorByte(val) ({				\
	uint8 __t;							\
	uint8 __c  = 4;						\
	__asm__ __volatile__ (				\
		"L_%=:\n\t"						\
		"rol %1" "\n\t"					\
		"ror %0" "\n\t"					\
		"rol %1" "\n\t"					\
		"ror %0" "\n\t"					\
		"dec %2" "\n\t"					\
		"breq L_%=" "\n\t"				\
		: "=r" (__t)					\
		: "r" ((uint8)(val)),			\
		  "r" ((uint8)(__c))			\
	);									\
 })*/

/**
 * Variable to hold crc value continously updated while communicating.
 */
extern uint8 crcValue;

/**
 * Variable to hold current content of sht11 status register.
 * This value is needed as start value of crc calculation.
 * The initial value (after power on reset) is 0. 
 */
extern uint8 sht11StatusReg;

/** 
 * Definitions of all known sht11 commands.
 */
/*@{*/
#define SHT11_MEAS_T	0x03		///< Start measuring of temperature.
#define SHT11_MEAS_RH	0x05		///< Start measuring of humidity.
#define SHT11_STATUS_R	0x07		///< Read status register.
#define SHT11_STATUS_W	0x06		///< Write status register.
#define SHT11_RESET		0x1E		///< Perform a sensor soft reset.
/*@}*/

/**
 * Enum to select between temperature and humidity measuring.
 */
typedef enum _sht11MeasureType
{
	sht11MeaT		= SHT11_MEAS_T,	///< Temperature		
	sht11MeaRh		= SHT11_MEAS_RH	///< Humidity
} sht11MeasureType;

/**
 * @brief	Start new transmission
 *
 * Performs a transmission start sequence and resets crc 
 * calculation.
 *
 * @note	Do not use this function directly. 
  */
void halSht11TransmissionStart( void );

/**
 * @brief	Read one byte from sensor
 *
 * Reads one byte form sensor and sends or skips ACK
 * according to sendAck.
 * This function does not perform any crc calculation!
 *
 * @note	Do not use this function directly. 
 *
 * @param	sendAck	if TRUE an ack will be sent to sensor
 *					if FALSE no ack will be sent. This will terminate the communication.
 *
 * @return	the byte read form sensor
 */
uint8 halSht11ReadByte( bool sendAck );

/**
 * @brief	Send one byte to sensor
 *
 * Sends a byte to the sensor and updates the crc calculation.
 *
 * @note	Do not use this function directly. 
 *
 * @param	value	The value to be sent.
 *
 * @return	TRUE if ACK form sensor received. FALSE otherwise.
 */
bool halSht11SendByte( uint8 value );

/**
 * @brief	Sets up sht11 
 *
 * Ports are set up to enable communication.
 */
void halSht11Init( void );

/** 
 * @brief	Start measuring
 *
 * Starts a temperature or humidity measurement according
 * to the value in type.
 * This function does not wait for the measurement to complete.
 *
 * @param	type	The type of value that should be measured.
 *
 * @return	TRUE if ACK form sensor received. FALSE otherwise.
 */
bool halSht11StartMeasure( sht11MeasureType type );

/**
 * @brief	Wait for the sensor to complete measurment.
 *
 * This function blocks until the sensor sings completion of current 
 * measurement. 
 *
 * @warning	By now there is no timeout implemented. We have to take care 
 *			the sensor is realy measuring. Otherwise we are trapped in an 
 *			endless loop.
 */
 #define halSht11ValueReady()	(SHT11_GET_BIT() == 0)

/**
 * @brief	Gets the result of measurement.
 *
 * First this function checks for the sensor to be ready. If not it blocks until
 * value is ready.
 * The value is read form sensor, the crc value is updated and compared with crc
 * value supplied by the sensor.
 *
 * @param	value	Measured value is returned in here.
 *
 * @return	TRUE if no crc error, FALSE otherwise.
 */
bool halSht11GetMeaValue( uint16 * value );

/**
 * @brief	Write status register
 *
 * The supplied value is written into status register. 
 * sht11StatusReg (needed for crc calculation) is updated, too.
 *
 * @param	value	new status register value.
 *
 * @return	TRUE if ACK form sensor received. FALSE otherwise.
 */
bool halSht11WriteStatus( uint8 value );

/**
 * @brief	Read status register
 *
 * Reads out the sensor's status register. Crc is updated and compared 
 * to the crc value supplied by the sensor.
 * sht11StatusReg (needed for crc calculation) is updated, too.
 *
 * @param	value	Current status register value is returned in here.
 *
 * @return	TRUE if no crc error, FALSE otherwise.
 */
bool halSht11ReadStatus( uint8 *value );


#endif
