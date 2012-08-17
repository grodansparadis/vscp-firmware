/*************************************************************
* nRF2401A interface code
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/

#ifndef _NRF2401A_H_
#define _NRF2401A_H_

#include <system.h>

// Define the nRF connections here
#define NRF_CE_PORT			PORTD
#define NRF_CS_PORT			PORTD
#define NRF_PWR_UP_PORT		PORTD
#define NRF_DR1_PORT		PORTB

#define NRF_CE_TRIS			TRISD
#define NRF_CS_TRIS			TRISD
#define NRF_PWR_UP_TRIS		TRISD
#define NRF_DR1_TRIS		TRISB

#define NRF_CE_PIN			4
#define NRF_CS_PIN			5
#define NRF_PWR_UP_PIN		6
#define NRF_DR1_PIN			0

///////////////////////////////////////////////////////////////////
// Don't change below this line
volatile bit nrf_ce			 @ NRF_CE_PORT     . NRF_CE_PIN;
volatile bit nrf_cs			 @ NRF_CS_PORT     . NRF_CS_PIN;
volatile bit nrf_pwr_up 	 @ NRF_PWR_UP_PORT . NRF_PWR_UP_PIN;
volatile bit nrf_dr1 		 @ NRF_DR1_PORT    . NRF_DR1_PIN;

bit 		 nrf_ce_tris	 @ NRF_CE_TRIS     . NRF_CE_PIN;
bit			 nrf_cs_tris	 @ NRF_CS_TRIS	   . NRF_CS_PIN;
bit			 nrf_pwr_up_tris @ NRF_PWR_UP_TRIS . NRF_PWR_UP_PIN; 
bit			 nrf_dr1_tris    @ NRF_DR1_TRIS    . NRF_DR1_PIN;
bit			 spi_dout_tris	 @ TRISC . 5;

// Constants and data types (credit: Andy Dewilde)

/* ===============================================================
					   CONFIGURATION PARAMETERS
   =============================================================== */

//.txrx
#define NRF_TX         0x0         /**< transmit mode */
#define NRF_RX         0x1         /**< receive mode  */

//.power
#define NRF_POWM20dBm  0x00        /**< -20 dBm output power (lowest) */
#define NRF_POWM10dBm  0x01        /**< -10 dBm output power          */
#define NRF_POWM5dBm   0x02        /**< -5  dBm output power          */ 
#define NRF_POW0dBm    0x03        /**<   0 dBm output power (highest)*/ 

//.xtal
#define NRF_XTAL4      0x00        /**< 4  MHz crystal connected */
#define NRF_XTAL8      0x01        /**< 8  MHz crystal connected */
#define NRF_XTAL12     0x02        /**< 12 MHz crystal connected */
#define NRF_XTAL16     0x03        /**< 16 MHz crystal connected <- */
#define NRF_XTAL20     0x04        /**< 20 MHz crystal connected */

//.rate
#define NRF_RATE250k   0x00        /**< 250 kbps, -90 dB recv sensi. */
#define NRF_RATE1M     0x01        /**< 1 Mbps, -80 dB recv sensi.   */

//.mode
#define NRF_DIRECT     0x00        /**< modulate the carrier directly */
#define NRF_SHOCKBURST 0x01        /**< shockburst buffered mode      */

//.rxmode
#define NRF_SINGLERX   0x00        /**< activate single receiver */
#define NRF_DUALRX     0x01        /**< activiate two  receiver  */
 
//.addrwidth
#define NRF_ADDR24     24          /**< 24 bit address  */
#define NRF_ADDR32     32          /**< 32 bit address  */
#define NRF_ADDR40     40          /**< 40 bit address  */

//.crcmode
#define NRF_CRCOFF     0x00        /**< disable CRC calculation */
#define NRF_CRCON      0x01        /**< enable  CRC calculation */

//.crclength
#define NRF_CRC8BIT    0x00         /**< 8  bit CRC checksum */
#define NRF_CRC16BIT   0x01         /**< 16 bit CRC checksum */



// ------------------------------------------------------------------
/** \brief nRF2401 configuration settings
  
    All configuration bits of the Nordic nRF2401 chip are mapped into
    this structure.  
*/// ----------------------------------------------------------------
struct _nrf2401_t{
	char  datawidth_ch2;        /**< payload size ch2 (in bits) */
	char  datawidth_ch1;        /**< payload size ch1 (in bits) */
  
	char  addr_ch2[5];          /**< source address ch2         */
	char  addr_ch1[5];          /**< source address ch1         */
 
	char  crcmode;              /**< CRC on/off                 1 */
	char  crclength;            /**< type of CRC (8, 16 bits)   1 */
	char  addrwidth;            /**< number of address bits     6 */

	char  power;                /**< transmit power             2 */
	char  xtal;	               /**< crystal oscillator         3 */
	char  rate;                 /**< communication speed        1 */
	char  mode;                 /**< transmission mode          1 */       
	char  rxmode;               /**< duoceiver mode             1 */
 
    char  txrx;                 /**< transmit or receive mode   1 */
	char  channel;              /**< communicatione channel     7 */
};


typedef struct _nrf2401_t nrf2401_t;


// Function definitions
void nrf_init();
void nrf_rx_mode();
void nrf_tx_mode();
void nrf_read();
void nrf_write(char* addr, char* data);
char nrf_get_byte(char index);
void nrf_send_config(char nr_of_bytes);
void nrf_basic_config();

#endif // _NRF2401A_H_
