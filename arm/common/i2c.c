/*
  i2c support for LPC21XX

  (c) 2004  Yuri Tiomkin (yuri@tnkernel.com)


  Includes drivers:

   - EEPROM 24XX series (from 24XX04 to 24XX512)
   - Real Time Clock DS1307
   - I/O Extender PCA9555
   - Temperature sensor LM75

  Supports uCOS-II and TNKernel

THIS SOFTWARE IS PROVIDED BY THE YURI TIOMKIN AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL YURI TIOMKIN OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

*/

#define __UCOS_          1  //-- if uCOS-II is used
//#define __TNKERNEL_    1    //-- if TNKernel is used

#include "LPC21XX.h"
#include "i2c.h"

#if defined(__UCOS_)
#include ".\\os\\includes.h"
#elif defined(__TNKERNEL_)
#include "..\\os\\tn.h"
#include "..\\os\\tn_port.h"
#endif

#ifndef NULL
#define NULL 0
#endif

#if defined(__UCOS_)
extern OS_EVENT  * gpSemI2Cop;
#elif defined(__TNKERNEL_)
extern TN_SEM  gSemI2Cop;
#endif


//===========================================================================
//================ Common routines ==========================================
//===========================================================================
void i2c_lpc_init(int Mode)
{
   if(Mode == I2C_SPEED_400)
   {
  //--- I2C Timing for 58 MHz (t = 16.954 ns) ---
      rI2C_I2SCLH = 47;  //-- more then 0.6 us  - 0.8
      rI2C_I2SCLL = 93;  //-- more then 1.3 us  - 1.4
   }
   else //Slow
   {
      rI2C_I2SCLH = 47*4;
      rI2C_I2SCLL = 93*4;
   }

   rI2C_I2CONCLR = 0xFF;           //-- Clear all flags
   rI2C_I2CONSET = 0x40;           //-- Set Master Mode
   rI2C_I2CONSET |= I2C_FLAG_I2EN; //--- Enable I2C
}

//---------------------------------------------------------------------------
static void i2c_lpc_wr_byte(int byte)
{
   rI2C_I2DAT = byte;
   rI2C_I2CONCLR = I2C_FLAG_SI;                //-- Clear SI
   while(!(rI2C_I2CONSET & I2C_FLAG_SI));      //-- End wr POINT
}

//---------------------------------------------------------------------------
static void i2c_lpc_stop()
{
    //-- Set STOP condition
   rI2C_I2CONCLR = I2C_FLAG_SI;                  //-- Clear SI
   rI2C_I2CONSET |=  I2C_FLAG_AA | I2C_FLAG_STO; //-- Clear NO ASK
}

//---------------------------------------------------------------------------
static int i2c_lpc_ctrl(int ctrl)
{
   int chk;
   //-- Set START
   rI2C_I2CONCLR = 0xFF; // Clear all bits
   rI2C_I2CONSET |= I2C_FLAG_I2EN | I2C_FLAG_STA;
   while(!(rI2C_I2CONSET & I2C_FLAG_SI));      //--- End START
   //-- Set ADDRESS
   rI2C_I2DAT = ctrl;
   rI2C_I2CONCLR = I2C_FLAG_STA | I2C_FLAG_SI; //-- Clear START & SI
   if(ctrl & 1) //-- RD
      chk = 0x40; //-- 40H - SLA+R has been transmitted; ACK has been received
   else
      chk = 0x18; //-- 18H - SLA+W has been transmitted; ACK has been received
   while(!(rI2C_I2CONSET & I2C_FLAG_SI));      //-- End CTRL
   if(rI2C_I2STAT != chk)
   {
      i2c_lpc_stop();
      return I2C_ERR_NO_RESPONSE;
   }
   return I2C_NO_ERR;
}

//---------------------------------------------------------------------------
static int i2c_lpc_rx_to_buf(char * buf,int num)
{
   int rc;

   if(buf == NULL)
      return I2C_ERR_WRONG_PARAM;

   rc = num;
   if(rc > 1)
   {
      rI2C_I2CONCLR = I2C_FLAG_SI;
      rI2C_I2CONSET |= I2C_FLAG_AA;
      for(;;)
      {
         while(!(rI2C_I2CONSET & I2C_FLAG_SI));  //-- End Data from slave;
         *buf++ = (unsigned char)rI2C_I2DAT;
         rc--;
         if(rc <= 0)
            break;
         else if(rc == 1)
            rI2C_I2CONCLR = I2C_FLAG_AA | I2C_FLAG_SI;  //-- After next will NO ASK
         else
         {
            rI2C_I2CONCLR = I2C_FLAG_SI;
            rI2C_I2CONSET |= I2C_FLAG_AA;
         }
      }
   }
   else if(rc == 1)
   {
      rI2C_I2CONCLR = I2C_FLAG_AA | I2C_FLAG_SI;  //-- After next will NO ASK
      while(!(rI2C_I2CONSET & I2C_FLAG_SI));  //-- End Data from slave;
      *buf = (unsigned char)rI2C_I2DAT;
   }
   else //err
      return I2C_ERR_WRONG_PARAM;

   return I2C_NO_ERR;
}
//----------------------------------------------------------------------
static int i2c_lpc_ask_polling_op(int ctrl)  //-- wait until write finished
{
   int rc;
   int i;

#if defined(__UCOS_)
   for(i=0;i < 15; i++)
#elif defined(__TNKERNEL_)
   for(i=0;i < 15; i++)
#else
   for(i=0;i < I2C_WR_24XX_TIMEOUT; i++) //-- actually wr = ~110 but timeout =10000
#endif
   {
      rI2C_I2CONSET = I2C_FLAG_STA;
      rI2C_I2CONCLR = I2C_FLAG_SI;  //-- Here - clear only SI (not all rI2C_I2CONCLR)
      while(!(rI2C_I2CONSET & I2C_FLAG_SI));        //wait the ACK

      rI2C_I2DAT = ctrl & 0xFE;; // R/WI = 0
      rI2C_I2CONCLR = I2C_FLAG_SI | I2C_FLAG_STA; //-- Clear START & SI
      while(!(rI2C_I2CONSET & I2C_FLAG_SI));//wait the ACK
      rc = rI2C_I2STAT;
      if(rc == 0x18) //-- got ACK after CLA + W
         break;
      else
      {
#if defined(__UCOS_)
         OSTimeDly(2);      //-- 2 mS
#elif defined(__TNKERNEL_)
         tn_task_sleep(2);  //-- 2 mS
#endif
      }
   }
   if(i == I2C_WR_24XX_TIMEOUT)
      return I2C_ERR_24XX_WR_TIMEOUT;
   return I2C_NO_ERR;
}
//===========================================================================
//================ EEPROM 24XX series ========================================
//===========================================================================

//----------------------------------------------------------------------------
static int m24xx_page_size(int eeprom_type, int eeprom_addr,int * pg_sz)
{
   int page_size,rc;

   page_size = 0;
   rc = I2C_NO_ERR;
   switch(eeprom_type)
   {
      case EEPROM_24XX04:
         if(eeprom_addr > EEPROM_MAX_ADDR_24XX04)
            rc = I2C_ERR_24XX_BAD_ADDR;
         else
            page_size = 16; // 24LC04B have page size = 16,24C04A = 8
         break;
      case EEPROM_24XX08:
         if(eeprom_addr > EEPROM_MAX_ADDR_24XX08)
            rc = I2C_ERR_24XX_BAD_ADDR;
         else
            page_size = 16;
         break;
      case EEPROM_24XX16:
         if(eeprom_addr > EEPROM_MAX_ADDR_24XX16)
            rc = I2C_ERR_24XX_BAD_ADDR;
         else
            page_size = 16;
         break;
      case EEPROM_24XX32:
         if(eeprom_addr > EEPROM_MAX_ADDR_24XX32)
            rc = I2C_ERR_24XX_BAD_ADDR;
         else
            page_size = 32;
         break;
      case EEPROM_24XX64:
         if(eeprom_addr > EEPROM_MAX_ADDR_24XX64)
            rc = I2C_ERR_24XX_BAD_ADDR;
         else
            page_size = 32;
         break;
      case EEPROM_24XX128:
         if(eeprom_addr > EEPROM_MAX_ADDR_24XX128)
            rc = I2C_ERR_24XX_BAD_ADDR;
         else
            page_size = 64;
         break;
      case EEPROM_24XX256:
         if(eeprom_addr > EEPROM_MAX_ADDR_24XX256)
            rc = I2C_ERR_24XX_BAD_ADDR;
         else
            page_size = 64;
         break;
      case EEPROM_24XX512:
         if(eeprom_addr > EEPROM_MAX_ADDR_24XX512)
            rc = I2C_ERR_24XX_BAD_ADDR;
         else
            page_size = 128;
         break;
   }
   if(rc != I2C_NO_ERR)
      return rc;
   if(page_size == 0)  //-- Bad eeprom_type
      return I2C_ERR_24XX_BAD_TYPE;

   if(pg_sz)
      *pg_sz = page_size;
   return I2C_NO_ERR;
}
//----------------------------------------------------------------------------
static int m24xx_set_addr(
    int eeprom_type,     //-- EEPROM type
    int eeprom_addr,     //-- start eeprom addr ( not included Hardware A2,A1,A0)
    int eeprom_cs_val,   //-- Hardware A2,A1,A0 (valid from 24XX32)
    int * ctrl_val,      //-- Value of ctrl(return)
    int * addr_hi_val,   //-- Value of addr_hi(return)
    int * addr_lo_val)   //-- Value of addr_lo(return)
{
   int ctrl;
   int addr_hi;
   int addr_lo;
   int rc;

   rc = I2C_NO_ERR;
   ctrl = 0;
   addr_hi = 0;
   addr_lo = 0;

   switch(eeprom_type)
   {
      case EEPROM_24XX04:   // 24LC04B ignore AO,A1,A2 pins


         ctrl = (eeprom_addr>>7) & 0x02; //-- 00000010
         ctrl |= 0xA0; //-- 1010xxxx
         addr_hi = eeprom_addr & 0xFF;
         addr_lo = -1;
         break;

      case EEPROM_24XX08: // 24LC08B ignore AO,A1,A2 pins

         ctrl = (eeprom_addr>>7) & 0x06; //-- 00000110
         ctrl |= 0xA0; //-- 1010xxxx
         addr_hi = eeprom_addr & 0xFF;
         addr_lo = -1;
         break;

      case EEPROM_24XX16: // 24LC16B ignore AO,A1,A2 pins

         ctrl = (eeprom_addr>>7) & 0x07; //-- 00001110
         ctrl |= 0xA0; //-- 1010xxxx
         addr_hi = eeprom_addr & 0xFF;
         addr_lo = -1;
         break;

      case EEPROM_24XX32:

         ctrl = (eeprom_cs_val<<1) & 0x07; //-- 00001110
         ctrl |= 0xA0; //-- 1010xxxx
         addr_hi = (eeprom_addr>>8) & 0x0F;
         addr_lo = eeprom_addr & 0xFF;
         break;

      case EEPROM_24XX64:

         ctrl = (eeprom_cs_val<<1) & 0x07; //-- 00001110
         ctrl |= 0xA0; //-- 1010xxxx
         addr_hi = (eeprom_addr>>8) & 0x1F;
         addr_lo = eeprom_addr & 0xFF;
         break;

      case EEPROM_24XX128:

         ctrl = (eeprom_cs_val<<1) & 0x07; //-- 00001110
         ctrl |= 0xA0; //-- 1010xxxx
         addr_hi = (eeprom_addr>>8) & 0x3F;
         addr_lo = eeprom_addr & 0xFF;
         break;

      case EEPROM_24XX256:

         ctrl = (eeprom_cs_val<<1) & 0x07; //-- 00001110
         ctrl |= 0xA0; //-- 1010xxxx
         addr_hi = (eeprom_addr>>8) & 0x7F;
         addr_lo = eeprom_addr & 0xFF;
         break;

      case EEPROM_24XX512:

         ctrl = (eeprom_cs_val<<1) & 0x07; //-- 00001110
         ctrl |= 0xA0; //-- 1010xxxx
         addr_hi = (eeprom_addr>>8) & 0xFF;
         addr_lo = eeprom_addr & 0xFF;
         break;
   }
   if(rc != I2C_NO_ERR)
      return rc;
   if(ctrl == 0)
      return I2C_ERR_24XX_BAD_TYPE;

   if(ctrl_val)
      *ctrl_val = ctrl;
   if(addr_hi_val)
      *addr_hi_val = addr_hi;
   if(addr_lo_val)
      *addr_lo_val = addr_lo;

   return I2C_NO_ERR;
}

//----------------------------------------------------------------------------
static int i2c_lpc_m24xx_wr(
     int eeprom_type,    //-- EEPROM type
     int eeprom_addr,    //-- start eeprom addr ( not included Hardware A2,A1,A0)
     int eeprom_cs_val,  //-- Hardware A2,A1,A0 (valid from 24XX32)
     char * buf,         //-- Data srs buf
     int num )            //-- Bytes to write qty
{
   int rc;
   int ctrl;
   int addr_hi;
   int addr_lo;

   rc = m24xx_set_addr(eeprom_type,eeprom_addr,eeprom_cs_val,
                                  &ctrl,&addr_hi,&addr_lo);
   if(rc != I2C_NO_ERR)
      return rc;

   //--- wr START + CONTROL
   rc = i2c_lpc_ctrl(ctrl); //-- Now WR (RD/WI = 0)
   if(rc != I2C_NO_ERR)
      return rc;
   //--- wr ADDRESS
   i2c_lpc_wr_byte(addr_hi);
   if(addr_lo != -1)
      i2c_lpc_wr_byte(addr_lo);
   //---  Write  data
   while(num--)                 //-- transmit data until length>0
   {
      rc = *buf++; //---
      i2c_lpc_wr_byte(rc);
   }
   //-----------------------
   i2c_lpc_stop();

   rc = i2c_lpc_ask_polling_op(ctrl);    //-- wait until write finished
   i2c_lpc_stop();
   return I2C_NO_ERR;
}

//----------------------------------------------------------------------------
int m24xx_write(
    int eeprom_type,    //-- EEPROM type
    int eeprom_addr,    //-- start eeprom addr ( not included Hardware A2,A1,A0)
    int eeprom_cs_val,  //-- Hardware A2,A1,A0 (valid from 24XX32)
    char * buf,         //-- Data src buf
    int num)            //-- Bytes to write qty
{
   int page_size = 0;
   int rc;
   int b_to_wr;

#if defined(__UCOS_)
   INT8U err;

   OSSemPend(gpSemI2Cop,0,&err);  //-- Wait until I2C released(sem on)
   VICIntEnClear = 0x00004000;  //-- Disable Int 0 - from I/O extender PCA9555
#elif defined(__TNKERNEL_)
   tn_sem_acquire(&gSemI2Cop,TN_WAIT_INFINITE); //-- Wait until I2C released(sem on)
   VICIntEnClear |= 0x00004000;  //-- Disable Int 0 - from I/O extender PCA9555
#endif

   rc = I2C_NO_ERR;
   for(;;)
   {
      rc = m24xx_page_size(eeprom_type,eeprom_addr,&page_size);
      if(rc != I2C_NO_ERR)
         break;
      if(page_size == 0)
      {
         rc = I2C_ERR_24XX_BAD_PAGESIZE;
         break;
      }

      rc = eeprom_addr%page_size;
      if(rc != 0) //-- not fit on page alignment
      {
         b_to_wr = page_size - rc;
         if(num < b_to_wr)
            b_to_wr = num;
         if(b_to_wr > 0)
         {
             rc = i2c_lpc_m24xx_wr(eeprom_type,eeprom_addr,eeprom_cs_val, buf,b_to_wr);
             if(rc != I2C_NO_ERR)
                break;
             num -= b_to_wr;
             eeprom_addr += b_to_wr;
             buf += b_to_wr;
         }
      }
       //--- write remainder(by pages,if possible)
      while(num > 0)
      {
          if(num < page_size)
             b_to_wr = num;
          else
             b_to_wr = page_size;

          rc = i2c_lpc_m24xx_wr(eeprom_type,eeprom_addr,eeprom_cs_val, buf,b_to_wr);
          if(rc != I2C_NO_ERR)
             break;
          num -= b_to_wr;
          eeprom_addr += b_to_wr;
          buf += b_to_wr;
      }

      break;
   }

#if defined(__UCOS_)
   VICIntEnable  |= 0x00004000;       //-- Enable Int 0
   OSSemPost(gpSemI2Cop);             //-- Set sem on(I2C is free)
#elif defined(__TNKERNEL_)
   VICIntEnable  |= 0x00004000;       //-- Enable Int 0
   tn_sem_signal(&gSemI2Cop);         //-- Set sem on(I2C is free)
#endif

   return rc;
}

//----------------------------------------------------------------------------
int m24xx_read(
  int eeprom_type,    //-- EEPROM type
  int eeprom_addr,    //-- start eeprom addr ( not included Hardware A2,A1,A0)
  int eeprom_cs_val,  //-- Hardware A2,A1,A0 (valid from 24XX32)
  char * buf,         //-- Data dst buf
  int num)            //-- Bytes to read qty
{
   int page_size;
   int rc;
   int ctrl;
   int addr_hi;
   int addr_lo;

#if defined(__UCOS_)
   INT8U err;

   OSSemPend(gpSemI2Cop,0,&err);  //-- Wait until I2C released(sem on)
   VICIntEnClear = 0x00004000;  //-- Disable Int 0 - from I/O extender PCA9555
#elif defined(__TNKERNEL_)
   tn_sem_acquire(&gSemI2Cop,TN_WAIT_INFINITE); //-- Wait until I2C released(sem on)
   VICIntEnClear |= 0x00004000;  //-- Disable Int 0 - from I/O extender PCA9555
#endif

   rc = I2C_NO_ERR;
   for(;;)
   {
      if(num <=0)
      {
         rc = I2C_ERR_24XX_WRONG_NUM;
         break;
      }
       //--- Here - just for addr checking
      page_size = 0;
      rc = m24xx_page_size(eeprom_type,eeprom_addr,&page_size);
      if(rc != I2C_NO_ERR)
         break;
      if(page_size == 0)
      {
         rc = I2C_ERR_24XX_BAD_PAGESIZE;
         break;
      }
      rc = m24xx_set_addr(eeprom_type,eeprom_addr,eeprom_cs_val,
                                           &ctrl,&addr_hi,&addr_lo);
      if(rc != I2C_NO_ERR)
         break;

       //--- wr START + CONTROL
      rc = i2c_lpc_ctrl(ctrl & 0xFE); //-- Now WR (RD/WI = 0)
      if(rc != I2C_NO_ERR)
         break;
       //--- wr ADDRESS
      i2c_lpc_wr_byte(addr_hi);
      if(addr_lo != -1)
         i2c_lpc_wr_byte(addr_lo);

       //--- wr START + CONTROL again - for read start
      rc = i2c_lpc_ctrl(ctrl | 0x01); //-- Now RD (RD/WI = 1)
      if(rc != I2C_NO_ERR)
         break;

      rc = i2c_lpc_rx_to_buf(buf,num);
      if(rc != I2C_NO_ERR)
         break;

      i2c_lpc_stop();     //---- Set STOP ---

      break;
   }

#if defined(__UCOS_)
   VICIntEnable  |= 0x00004000;       //-- Enable Int 0
   OSSemPost(gpSemI2Cop);             //-- Set sem on(I2C is free)
#elif defined(__TNKERNEL_)
   VICIntEnable  |= 0x00004000;       //-- Enable Int 0
   tn_sem_signal(&gSemI2Cop);         //-- Set sem on(I2C is free)
#endif

   return rc;
}

//===========================================================================
//================ Temperature sensor LM75 ==================================
//===========================================================================

//----------------------------------------------------------------------------
static int lm75_set_addr(int cs_val,      //-- Value of A0,A1,A2 for IC instance
                         int mode,        //-- 8 - 8bit reg, 16 - 16 bit reg
                         int reg,         //-- register that  access
                         int * ctrl_val,  //-- return - ctrl
                         int * point_val) //-- return - pointer reg val
{
   int ctrl;
   int point;

   ctrl  = (cs_val<<1) & 0x0E;
   ctrl |= 0x90; //-- 1001xxxx
   point = 0;
   if(mode == 16)
   {
      if(reg == LM75_REG_TEMP)
         point = 0;
      else if(reg == LM75_REG_THYST)
         point = 2;
      else if(reg == LM75_REG_TSET)
         point = 3;
      else
         return I2C_ERR_LM75_WRONG_REGISTER;
   }
   else if(mode == 8)
   {
      if(reg == LM75_REG_CONFIG)
         point = 1;
      else
         return I2C_ERR_LM75_WRONG_REGISTER;
   }
   else
      return I2C_ERR_WRONG_PARAM;

   if(ctrl_val)
      *ctrl_val  = ctrl;
   if(point_val)
      *point_val = point;

   return I2C_NO_ERR;
}
//----------------------------------------------------------------------------
int lm75_read16(int cs_val,
                int reg,
                int * ret_val)
{
   int point;
   int ctrl;
   int rc,rd_val;
   unsigned char buf[4];

   rc = lm75_set_addr(cs_val,16,reg,&ctrl,&point);
   if(rc != I2C_NO_ERR)
      return rc;

   //--- wr START + CONTROL
   rc = i2c_lpc_ctrl(ctrl & 0xFE); //-- Now WR (RD/WI = 0)
   if(rc != I2C_NO_ERR)
      return rc;
   //--- wr point
   i2c_lpc_wr_byte(point);

   //--- wr START + CONTROL
   rc = i2c_lpc_ctrl(ctrl | 0x01); //-- Now RD (RD/WI = 1)
   if(rc != I2C_NO_ERR)
      return rc;

   rc = i2c_lpc_rx_to_buf(&buf[0],2);
   if(rc != I2C_NO_ERR)
      return rc;

   i2c_lpc_stop();

   rd_val = (buf[0]<<1)& 0x01FE; //-- msb
   if(buf[1] & 0x80)             //-- lsb
      rd_val |= 0x01;

   *ret_val = rd_val;

   return I2C_NO_ERR;
}

//----------------------------------------------------------------------------
int lm75_read_cfg(int cs_val,int * ret_val)
{
   int point;
   int ctrl;
   int rc;
   unsigned char buf[4];

   rc = lm75_set_addr(cs_val,8,LM75_REG_CONFIG,&ctrl,&point);
   if(rc != I2C_NO_ERR)
      return rc;

   //--- wr START + CONTROL
   rc = i2c_lpc_ctrl(ctrl & 0xFE); //-- Now WR (RD/WI = 0)
   if(rc != I2C_NO_ERR)
      return rc;
   //--- wr point
   i2c_lpc_wr_byte(point);

   //--- wr START + CONTROL
   rc = i2c_lpc_ctrl(ctrl | 0x01); //-- Now RD (RD/WI = 1)
   if(rc != I2C_NO_ERR)
      return rc;

   rc = i2c_lpc_rx_to_buf(&buf[0],1);
   if(rc != I2C_NO_ERR)
      return rc;

   i2c_lpc_stop();                 //-- Set STOP condition

   *ret_val = buf[0];

   return I2C_NO_ERR;
}
//----------------------------------------------------------------------------
int lm75_write16(int cs_val,
                int reg,
                int wr_val)
{
   int ctrl;
   int point;
   int rc;

   rc = lm75_set_addr(cs_val,16,reg,&ctrl,&point);
   if(rc != I2C_NO_ERR)
      return rc;

   //--- wr START + CONTROL
   rc = i2c_lpc_ctrl(ctrl & 0xFE); //-- Now WR (RD/WI = 0)
   if(rc != I2C_NO_ERR)
      return rc;
   //--- wr point
   i2c_lpc_wr_byte(point);
   //--- wr msb
   i2c_lpc_wr_byte((wr_val>>8)& 0xFF);
   //--- wr lsb
   i2c_lpc_wr_byte(wr_val & 0xFF);

   i2c_lpc_stop();

   return I2C_NO_ERR;
}
//----------------------------------------------------------------------------
int lm75_write_cfg(int cs_val,int wr_val)
{
   int point;
   int ctrl;
   int rc;

   rc = lm75_set_addr(cs_val,8,LM75_REG_CONFIG,&ctrl,&point);
   if(rc != I2C_NO_ERR)
      return rc;

   //--- wr START + CONTROL
   rc = i2c_lpc_ctrl(ctrl & 0xFE); //-- Now WR (RD/WI = 0)
   if(rc != I2C_NO_ERR)
      return rc;
   //--- wr point
   i2c_lpc_wr_byte(point);
   //--- wr data
   i2c_lpc_wr_byte(wr_val & 0xFF);

   i2c_lpc_stop();

   return I2C_NO_ERR;
}

//---------------------------------------------------------------------------
int lm75_init()
{
   int rc;

#if defined(__UCOS_)
   INT8U err;

   OSSemPend(gpSemI2Cop,0,&err);  //-- Wait until I2C released(sem on)
   VICIntEnClear = 0x00004000;  //-- Disable Int 0 - from I/O extender PCA9555
#elif defined(__TNKERNEL_)
   tn_sem_acquire(&gSemI2Cop,TN_WAIT_INFINITE); //-- Wait until I2C released(sem on)
   VICIntEnClear |= 0x00004000;  //-- Disable Int 0 - from I/O extender PCA9555
#endif

   rc = lm75_write_cfg(0,0);

#if defined(__UCOS_)
   VICIntEnable  |= 0x00004000;       //-- Enable Int 0
   OSSemPost(gpSemI2Cop);             //-- Set sem on(I2C is free)
#elif defined(__TNKERNEL_)
   VICIntEnable  |= 0x00004000;       //-- Enable Int 0
   tn_sem_signal(&gSemI2Cop);         //-- Set sem on(I2C is free)
#endif

   return rc;
}

//---------------------------------------------------------------------------
int lm75_get_temp(int * ret_val)
{
   int rc;

#if defined(__UCOS_)
   INT8U err;

   OSSemPend(gpSemI2Cop,0,&err);  //-- Wait until I2C released(sem on)
   VICIntEnClear = 0x00004000;  //-- Disable Int 0 - from I/O extender PCA9555
#elif defined(__TNKERNEL_)
   tn_sem_acquire(&gSemI2Cop,TN_WAIT_INFINITE); //-- Wait until I2C released(sem on)
   VICIntEnClear |= 0x00004000;  //-- Disable Int 0 - from I/O extender PCA9555
#endif

   rc = lm75_read16(0,LM75_REG_TEMP,ret_val);

#if defined(__UCOS_)
   VICIntEnable  |= 0x00004000;       //-- Enable Int 0
   OSSemPost(gpSemI2Cop);             //-- Set sem on(I2C is free)
#elif defined(__TNKERNEL_)
   VICIntEnable  |= 0x00004000;       //-- Enable Int 0
   tn_sem_signal(&gSemI2Cop);         //-- Set sem on(I2C is free)
#endif

   return rc;
}

//===========================================================================
//================ I/O Extender PCA9555  ==================================
//===========================================================================

//----------------------------------------------------------------------------
int pca9555_write(int Mode, int val)
{

 //  I2C_Write(Mode,val);

   int ctrl;
   int rc;

   ctrl = 0x42; //--- 0100 001 0

   //--- wr START + CONTROL
   rc = i2c_lpc_ctrl(ctrl);  //-- Now WR (RD/WI = 0)
   if(rc != I2C_NO_ERR)
      return rc;
   //--- wr Mode
   i2c_lpc_wr_byte(Mode);
   //--- wr Port 0
   i2c_lpc_wr_byte(val & 0x000000FF);
   //--- wr Port 1
   i2c_lpc_wr_byte((val>>8) & 0x000000FF);

   i2c_lpc_stop();

   return I2C_NO_ERR;
}

//----------------------------------------------------------------------------
int pca9555_read(int Mode, int * ret_val)
{
   int ctrl;
   int rc;
   int rd_val;
   unsigned char buf[4];


   ctrl = 0x42; //--- 0100 001 0

   //--- wr START + CONTROL
   rc = i2c_lpc_ctrl(ctrl); //-- Now WR (RD/WI = 0)
   if(rc != I2C_NO_ERR)
      return rc;
   //--- wr Mode
   i2c_lpc_wr_byte(Mode);

   //--- wr START + CONTROL
   rc = i2c_lpc_ctrl(ctrl | 0x01); //-- Now RD (RD/WI = 1)
   if(rc != I2C_NO_ERR)
      return rc;

   rc = i2c_lpc_rx_to_buf(&buf[0],2);
   if(rc != I2C_NO_ERR)
      return rc;

   i2c_lpc_stop();                 //-- Set STOP condition

   rd_val = (buf[1]<<8)& 0x0000FF00; //-- Port1
   rd_val |= buf[0];  //-- Port 0

   *ret_val = rd_val;

   return I2C_NO_ERR;
}

//===========================================================================
//================ Real Time Clock DS1307  ==================================
//===========================================================================

//----------------------------------------------------------------------------
char ds1307_toBCD(char bin_val)
{
//-- this function converts an 8 bit binary value
//-- to an 8 bit BCD value. Input range from 0 to 99.

   char temp;
   char retval;

   temp = bin_val;
   retval = 0;

   for(;;)
   {  // get tens digit by multiple subtraction of 10 from bin_val
      if(temp >= 10)
      {
         temp -= 10;
         retval += 0x10; // increment tens digit
      }
      else  // get ones digit by adding remainder
      {
         retval += temp; // adjusted result
         break;
      }
   }
   return retval;
}

//----------------------------------------------------------------------------
char ds1307_fromBCD(char bcd_val)
{
   return ((bcd_val >> 4) & 0x0F)*10 + (bcd_val & 0x0F);
}

//----------------------------------------------------------------------------
int ds1307_set(DATETIMEINFO * dti)
{
   int i;
   char buf[8];
#if defined(__UCOS_)
   INT8U err;
#endif

   buf[0] = dti->second;    // Seconds
   buf[1] = dti->minute;    // Minutes
   buf[2] = dti->hour;      // Hours
   buf[3] = 1;             // Day of week not supported! -> Always 1
   buf[4] = dti->day;      // Date
   buf[5] = dti->month;     // Month
   buf[6] = dti->year;      // Year
   buf[7] = 0;             // Control  = 00h (SQWE disable)

  //-- convert to BCD
   for(i=0; i<7; i++)
   {
      buf[i] = ds1307_toBCD(buf[i]);
   }
   //-- Set control bits
   buf[0] &= 0x7f;    //-- Seconds  bit 7(CH)    = 0 (clock enabled)
   buf[2] &= 0xbf;    //-- Hours    bit 6(12/24) = 0(24-hour mode)

#if defined(__UCOS_)
   OSSemPend(gpSemI2Cop,0,&err);  //-- Wait until I2C released(sem on)
   VICIntEnClear = 0x00004000;  //-- Disable Int 0 - from I/O extender PCA9555
#elif defined(__TNKERNEL_)
   tn_sem_acquire(&gSemI2Cop,TN_WAIT_INFINITE); //-- Wait until I2C released(sem on)
   VICIntEnClear |= 0x00004000;  //-- Disable Int 0 - from I/O extender PCA9555
#endif

   i2c_lpc_init(I2C_SPEED_100);
   i = ds1307_wr_regs(buf);
   i2c_lpc_init(I2C_SPEED_400);

#if defined(__UCOS_)
   VICIntEnable  |= 0x00004000;       //-- Enable Int 0
   OSSemPost(gpSemI2Cop);             //-- Set sem on(I2C is free)
#elif defined(__TNKERNEL_)
   VICIntEnable  |= 0x00004000;       //-- Enable Int 0
   tn_sem_signal(&gSemI2Cop);         //-- Set sem on(I2C is free)
#endif

   return i;
}

//----------------------------------------------------------------------------
int ds1307_get(DATETIMEINFO * dti)
{
   int rc;
   char buf[8];

#if defined(__UCOS_)
   INT8U err;

   OSSemPend(gpSemI2Cop,0,&err);  //-- Wait until I2C released(sem on)
   VICIntEnClear = 0x00004000;  //-- Disable Int 0 - from I/O extender PCA9555
#elif defined(__TNKERNEL_)
   tn_sem_acquire(&gSemI2Cop,TN_WAIT_INFINITE); //-- Wait until I2C released(sem on)
   VICIntEnClear |= 0x00004000;  //-- Disable Int 0 - from I/O extender PCA9555
#endif

   i2c_lpc_init(I2C_SPEED_100);
   rc = ds1307_rd_regs(buf);
   i2c_lpc_init(I2C_SPEED_400);

#if defined(__UCOS_)
   VICIntEnable  |= 0x00004000;       //-- Enable Int 0
   OSSemPost(gpSemI2Cop);             //-- Set sem on(I2C is free)
#elif defined(__TNKERNEL_)
   VICIntEnable  |= 0x00004000;       //-- Enable Int 0
   tn_sem_signal(&gSemI2Cop);         //-- Set sem on(I2C is free)
#endif

   if(rc != I2C_NO_ERR)
      return rc;

   //-- Mask off the unused bits
   buf[0] &= 0x7F; // Seconds
   buf[1] &= 0x7F; // Minutes
   buf[2] &= 0x3F; // Hours
   buf[3] &= 0x07; // Day of week not supported! -> Always 1
   buf[4] &= 0x3F; // Date (day of month)
   buf[5] &= 0x1F; // Month
                   // Year - not need

   //-- Convert from BCD

   dti->second  = ds1307_fromBCD(buf[0]); // Seconds
   dti->minute  = ds1307_fromBCD(buf[1]); // Minutes
   dti->hour    = ds1307_fromBCD(buf[2]); // Hours
   dti->day     = ds1307_fromBCD(buf[4]); // Date
   dti->month   = ds1307_fromBCD(buf[5]); // Month
   dti->year    = ds1307_fromBCD(buf[6]); // Year

   return I2C_NO_ERR;
}

//----------------------------------------------------------------------------
int ds1307_wr_regs(char * buf)  //---- Wr 8 control bytes to DS1307
{
   int rc;
   int num;
//---- Wr ----------
   //--- wr START + CONTROL
   rc = i2c_lpc_ctrl(0xd0);     //-- Now WR (RD/WI = 0); 0xd0 - ID of DS1307
   if(rc != I2C_NO_ERR)
      return rc;
   //--- wr ADDRESS
   i2c_lpc_wr_byte(0);
   //---  Write  data
   num = 8;
   while(num--)                 //-- transmit data until length>0
   {
      rc = *buf++; //---
      i2c_lpc_wr_byte(rc);
   }
   i2c_lpc_stop();
 //--------------------
   return I2C_NO_ERR;
}

//----------------------------------------------------------------------------
int ds1307_rd_regs(char * buf)  //---- Rd 8 control bytes from DS1307
{
   int rc;
//---- RD ------
   //--- wr START + CONTROL
   rc = i2c_lpc_ctrl(0xd0); //-- Now WR (RD/WI = 0)
   if(rc != I2C_NO_ERR)
      return rc;
   //--- wr ADDRESS
   i2c_lpc_wr_byte(0);

   //--- wr START + CONTROL again - for read start
   rc = i2c_lpc_ctrl(0xd0 | 0x01); //-- Now RD (RD/WI = 1)
   if(rc != I2C_NO_ERR)
      return rc;

   rc = i2c_lpc_rx_to_buf(buf,8);
   if(rc != I2C_NO_ERR)
      return rc;

   i2c_lpc_stop();     //---- Set STOP ---
//-------------------------------------
   return I2C_NO_ERR;
}

//----------------------------------------------------------------------------
int ds1307_wr_nvram(int addr,int num, char * buf,int * b_wr)
{
   int n_wr;
   int rc;

   addr += 0x08;
   n_wr = 0x3f - addr;
   if(n_wr <= 0)
      return I2C_ERR_WRONG_PARAM;

   n_wr = __min(n_wr,num);
   if(n_wr <= 0)
      return I2C_ERR_WRONG_PARAM;

 //---- Wr ----------
   //--- wr START + CONTROL
   rc = i2c_lpc_ctrl(0xd0); //-- Now WR (RD/WI = 0); 0xd0 - ID of DS1307
   if(rc != I2C_NO_ERR)
      return rc;
   //--- wr ADDRESS
   i2c_lpc_wr_byte(addr);
   //---  Write  data
   num = n_wr;
   while(num--)                 //-- transmit data until length>0
   {
      rc = *buf++; //---
      i2c_lpc_wr_byte(rc);
   }
   i2c_lpc_stop();
 //--------------------
   *b_wr = n_wr;
   return I2C_NO_ERR;
}

//----------------------------------------------------------------------------
int ds1307_rd_nvram(int addr,int num, char * buf,int * b_rd)
{
   int n_rd;
   int rc;

   addr += 0x08;
   n_rd = 0x3f - addr;
   if(n_rd <= 0)
      return I2C_ERR_WRONG_PARAM;

   n_rd = __min(n_rd,num);

   if(n_rd <= 0)
      return I2C_ERR_WRONG_PARAM;

   i2c_lpc_init(I2C_SPEED_100);

//---- RD ------
   //--- wr START + CONTROL
   rc = i2c_lpc_ctrl(0xd0); //-- Now WR (RD/WI = 0)
   if(rc != I2C_NO_ERR)
      return rc;
   //--- wr ADDRESS
   i2c_lpc_wr_byte(addr);

   //--- wr START + CONTROL again - for read start
   rc = i2c_lpc_ctrl(0xd0 | 0x01); //-- Now RD (RD/WI = 1)
   if(rc != I2C_NO_ERR)
      return rc;

   rc = i2c_lpc_rx_to_buf(buf,n_rd);
   if(rc != I2C_NO_ERR)
      return rc;

   i2c_lpc_stop();     //---- Set STOP ---

//--------------

   *b_rd = n_rd;
   return I2C_NO_ERR;
}

//---------------------------------------------------------------------------

