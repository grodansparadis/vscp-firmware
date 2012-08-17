//===========================================================================
// C01: FILE DESCRIPTION
//===========================================================================
// Company.............: Adam Elektronik Ltd.
// Project.............: LCD_DRIVER_ARM
// Target..............: LPC213x
// Description.........: External Interrupt fonksiyonları
// Compiler............: Keil uVision3
// Version.............: 0.01
// Author..............: Mehmet Kurnaz
// Creation-Date.......: 24.12.2006
//
// Modification History:
// ---------------------
//
// You can use ext. int. for rx and any gpio for tx. For first presence pulse you shold use ext int. 
// and than set that pin /as gpio. You can read application note 159 from dallas.
//
// I attached source codes you will need. You can use
// 
// if(iIbutton == TRUE)
// {
//     if(ibuttonReset() == 0)
//    {
//        if(ibuttonReadSerial(ibuttonSerial) == TRUE)
//       {
//            isIbutton = FALSE;
//            return TRUE;
//
//       }
//  }
//}
//===========================================================================

//===========================================================================
// C02: INCLUDE FILES
//===========================================================================
#include <adamtypes.h>
#include <extint.h>
#include <gpio.h>
#include <armgpio.h>
#include <interrupt.h>
#include <LPC213X.h>
#include <serialport.h>
#include <softtimer.h>
#include <ibutton.h>

//===========================================================================
// C03: PRIVATE CONSTANT DEFINITIONS
//===========================================================================


//===========================================================================
// C04: PRIVATE DATA TYPES
//===========================================================================


//===========================================================================
// C05: PRIVATE FUNCTION PROTOTYPES
//===========================================================================
void extInt3ISR (void) __irq;
//===========================================================================
// C06: PRIVATE DATA DECLARATIONS
//===========================================================================

//===========================================================================
// C07: GLOBAL DATA DECLARATIONS
//===========================================================================
volatile bool isIbutton = FALSE;
//===========================================================================
// C08: PRIVATE FUNCTIONS
//===========================================================================
//----------------------------------------------------------
// Function......: extInt3ISR
// Description...: external interrupt ISR
// Return Value..: ...
// Parameters....: ...
//----------------------------------------------------------
void extInt3ISR(void) __irq
{
interruptClose(EXTERNAL3_INTERRUPT);

arm_gpioSetPinAsGpio(P0PIN20);
delayUs(7000);
isIbutton = TRUE;
interruptRelease();
}
//===========================================================================
// C09: GLOBAL FUNCTIONS
//===========================================================================

//----------------------------------------------------------
// Function......: extIntInit
// Description...: external interrupt ilk değerleri
// Return Value..: ...
// Parameters....: ...
//----------------------------------------------------------
void extIntInit(void)
//----------------------------------------------------------
{
arm_gpioSetPinFunction(P0PIN20, PIN_FUNC_THIRD_ALTERNATE);
EXTINT = 0x08; //extint3, clearlenir
EXTPOLAR = 0x00; //düşen kenar
EXTMODE = 0x08; //extint3, kenar duyarlı
interruptOpen(EXTERNAL3_INTERRUPT, extInt3ISR);
}



//===========================================================================
// C10: END OF CODE
//===========================================================================
----------

//===========================================================================
// C01: FILE DESCRIPTION
//===========================================================================
// Company.............: Adam Elektronik Ltd.
// Project.............: ARM_LCD_DRIVER
// Target..............: LPC213x
// Description.........: ibutton kütüphanesi
// Compiler............: Keil uVision3
// Version.............: 0.01
// Author..............: Mehmet Kurnaz
// Creation-Date.......: 25.12.2006
//
// Modification History:
// ---------------------
//
//===========================================================================

//===========================================================================
// C02: INCLUDE FILES
//===========================================================================
#include <ibutton.h>
#include <softtimer.h>
#include <serialport.h>
#include <extint.h>
#include <interrupt.h>
#include <gpio.h>
#include <adamtypes.h>
#include <lcd.h>
#include <lcddataram.h>
//===========================================================================
// C03: PRIVATE CONSTANT DEFINITIONS
//===========================================================================


//===========================================================================
// C04: PRIVATE DATA TYPES
//===========================================================================
u8 A, B, C, D, E, F, G, H, I, J; //okuma yazma için gerekli zamanlamalar
//===========================================================================
// C05: PRIVATE FUNCTION PROTOTYPES
//===========================================================================


//===========================================================================
// C06: PRIVATE DATA DECLARATIONS
//===========================================================================


//===========================================================================
// C07: GLOBAL DATA DECLARATIONS
//===========================================================================
const u8 crcTable[] =
{
0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

//===========================================================================
// C08: PRIVATE FUNCTIONS
//===========================================================================

//===========================================================================
// C09: GLOBAL FUNCTIONS
//===========================================================================

//----------------------------------------------------------
// Function......: setSpeed
// Description...: ibutton haberleşme hızını ayarlar
// standart için (standard=1), overdrive için (standard=0).
// Return Value..: herhangi bir ibutton bulursa 0 bulamazsa 1 döndürür
// Parameters....: ...
//----------------------------------------------------------
void setSpeed(u8 standard)
{
if (standard)
{
// Standard Speed
A = 6;
B = 64;
C = 60;
D = 10;
E = 9;
F = 55;
G = 0;
H = 480;
I = 70;
J = 410;
}
else
{
// Overdrive Speed
A = 2;
B = 8;
C = 8;
D = 8;
E = 1;
F = 7;
G = 3;
H = 70;
I = 9;
J = 40;
}
}


//----------------------------------------------------------
// Function......: ibuttonReset
// Description...: 1-Wire hattı resetler ve ibutton var sinyali bekler
// Return Value..: herhangi bir ibutton bulursa 0 bulamazsa 1 döndürür
// Parameters....: ...
//----------------------------------------------------------
u8 ibuttonReset(void)
{
u8 result = 1;
delayUs(G);
gpioSetPin(IBUTTON_TX);
delayUs(H);
gpioClearPin(IBUTTON_TX);
delayUs(I);

result = gpioReadPin(IBUTTON_RX);
delayUs(J); // Complete the reset sequence recovery

return result;
}

//----------------------------------------------------------
// Function......: ibuttonWriteBit
// Description...: ibutton a 1 bit yazar
// Return Value..:
// Parameters....: ...
//----------------------------------------------------------
void ibuttonWriteBit(u8 bit)
{
if (bit)
{
// Write '1' bit
gpioSetPin(IBUTTON_TX);
delayUs(A);
gpioClearPin(IBUTTON_TX);
delayUs(B); // Complete the time slot and 10us recovery
}
else
{
// Write '0' bit
gpioSetPin(IBUTTON_TX);
delayUs(C);
gpioClearPin(IBUTTON_TX);
delayUs(D);
}
}


//----------------------------------------------------------
// Function......: ibuttonReadBit
// Description...: ibutton dan 1 bit okur
// Return Value..:
// Parameters....: ...
//----------------------------------------------------------
u8 ibuttonReadBit(void)
{
u8 result;

gpioSetPin(IBUTTON_TX);
delayUs(A);

gpioClearPin(IBUTTON_TX);
delayUs(E);

result = gpioReadPin(IBUTTON_RX);
delayUs(F);

return result;
}


//----------------------------------------------------------
// Function......: ibuttonWriteByte
// Description...: ibutton a 1 byte yazar
// Return Value..:
// Parameters....: ...
//----------------------------------------------------------
void ibuttonWriteByte(u8 data)
{
u8 loop;

// Loop to write each bit in the byte, LS-bit first
for (loop = 0; loop < 8; loop++)
{
ibuttonWriteBit(data & 0x01);

// shift the data byte for the next bit
data >>= 1;
}
}



//----------------------------------------------------------
// Function......: ibuttonReadByte
// Description...: ibutton dan 1 byte okur
// Return Value..:
// Parameters....: ...
//----------------------------------------------------------
u8 ibuttonReadByte(void)
{
u8 loop, result=0;

for (loop = 0; loop < 8; loop++)
{
// shift the result to get it ready for the next bit
result >>= 1;

// if result is one, then set MS bit
if (ibuttonReadBit())
result |= 0x80;
}
return result;
}

//----------------------------------------------------------
// Function......: ibuttonTouchByte
// Description...: Eşzamanlı okuma ve yazma(data '1' ise oku '0' yaz)
// Return Value..: 0(yazma durumunda) veya okunan bit değeri
// Parameters....:
//----------------------------------------------------------
//ibuttonTouchByte(0xFF) == ibuttonReadByte()
//ibuttonTouchByte(0xFF) == ibuttonReadByte()

u8 ibuttonTouchByte(u8 data)
{
u8 loop, result=0;

for (loop = 0; loop < 8; loop++)
{
// shift the result to get it ready for the next bit
result >>= 1;

// If sending a '1' then read a bit else write a '0'
if (data & 0x01)
{
if (ibuttonReadBit())
result |= 0x80;
}
else
ibuttonWriteBit(0);

// shift the data byte for the next bit
data >>= 1;
}
return result;
}


//----------------------------------------------------------
// Function......: ibuttonBlock
// Description...: aynı buffer üzerinden eşzamanlı okuma ve yazma
// Return Value..:
// Parameters....:
//----------------------------------------------------------
void ibuttonBlock(u8 *data, u8 data_len)
{
u8 loop;

for (loop = 0; loop < data_len; loop++)
{
data[loop] = ibuttonTouchByte(data[loop]);
}
}


//----------------------------------------------------------
// Function......: ibuttonReadSerial
// Description...: read rom(33h),
// Return Value..:
// Parameters....:
//----------------------------------------------------------
bool ibuttonReadSerial(u8 *serialArray)
{
u8 i, crc = 0;

setSpeed(1); //standart hızda haberleş
ibuttonWriteByte(0x33); //read rom

for(i = 0; i < 8; i++)
{
serialArray[i] = ibuttonReadByte();

crc = crcTable[crc ^ serialArray[i]];
}
if((serialArray[0] > 0) && (crc == 0))
{
return TRUE;
}
else
{
extIntInit();
return FALSE;
}
}



//===========================================================================
// C10: END OF CODE
//===========================================================================
