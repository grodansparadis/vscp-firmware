#include <global.h>

//===========================================================================
//================ Common routines ==========================================
//===========================================================================

void i2c_lpc_init(U8 Mode){
   if(Mode == I2C_SPEED_400)
   {
//--- I2C Timing for 60 MHz  ---
      I2C0_SCLH = 0x4b; 
      I2C0_SCLL = 0x4b; 
   }
   else //Slow
   {
      I2C0_SCLH = 0x4b*4;
      I2C0_SCLL = 0x4b*4;
   }

   I2C0_CONCLR = 0xFF;           //-- Clear all flags
   I2C0_CONSET = 0x40;           //-- Set Master Mode
   I2C0_CONSET |= I2C_FLAG_I2EN; //--- Enable I2C
}

//---------------------------------------------------------------------------
static void i2c_lpc_wr_byte(int byte)
{
   I2C0_DAT = byte;
   I2C0_CONCLR = I2C_FLAG_SI;                //-- Clear SI
   while(!(I2C0_CONSET & I2C_FLAG_SI));      //-- End wr POINT
}

//---------------------------------------------------------------------------
static void i2c_lpc_stop()
{
    //-- Set STOP condition
   I2C0_CONCLR = I2C_FLAG_SI;                  //-- Clear SI
   I2C0_CONSET |=  I2C_FLAG_AA | I2C_FLAG_STO; //-- Clear NO ASK
}

//---------------------------------------------------------------------------
static U8 i2c_lpc_ctrl(U8 ctrl)
{
   U8 chk;
   //-- Set START
   I2C0_CONCLR = 0xFF; // Clear all bits
   I2C0_CONSET |= I2C_FLAG_I2EN | I2C_FLAG_STA;
//timeout missing !!
   while(!(I2C0_CONSET & I2C_FLAG_SI));      //--- End START
   //-- Set ADDRESS
   I2C0_DAT = ctrl;
   I2C0_CONCLR = I2C_FLAG_STA | I2C_FLAG_SI; //-- Clear START & SI
   if(ctrl & 1) //-- RD
      chk = 0x40; //-- 40H - SLA+R has been transmitted; ACK has been received
   else
      chk = 0x18; //-- 18H - SLA+W has been transmitted; ACK has been received
   while(!(I2C0_CONSET & I2C_FLAG_SI));      //-- End CTRL
   if(I2C0_STAT != chk)
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
      I2C0_CONCLR = I2C_FLAG_SI;
      I2C0_CONSET |= I2C_FLAG_AA;
      for(;;)
      {
         while(!(I2C0_CONSET & I2C_FLAG_SI));  //-- End Data from slave;
         *buf++ = (unsigned char)I2C0_DAT;
         rc--;
         if(rc <= 0)
            break;
         else if(rc == 1)
            I2C0_CONCLR = I2C_FLAG_AA | I2C_FLAG_SI;  //-- After next will NO ASK
         else
         {
            I2C0_CONCLR = I2C_FLAG_SI;
            I2C0_CONSET |= I2C_FLAG_AA;
         }
      }
   }
   else if(rc == 1)
   {
      I2C0_CONCLR = I2C_FLAG_AA | I2C_FLAG_SI;  //-- After next will NO ASK
      while(!(I2C0_CONSET & I2C_FLAG_SI));  //-- End Data from slave;
      *buf = (unsigned char)I2C0_DAT;
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

   for(i=0;i < I2C_WR_TIMEOUT; i++) //-- actually wr = ~110 but timeout =10000
   {
      I2C0_CONSET = I2C_FLAG_STA;
      I2C0_CONCLR = I2C_FLAG_SI;  //-- Here - clear only SI (not all rI2C_I2CONCLR)
      while(!(I2C0_CONSET & I2C_FLAG_SI));        //wait the ACK

      I2C0_DAT = ctrl & 0xFE;; // R/WI = 0
      I2C0_CONCLR = I2C_FLAG_SI | I2C_FLAG_STA; //-- Clear START & SI
      while(!(I2C0_CONSET & I2C_FLAG_SI));//wait the ACK
      rc = I2C0_STAT;
      if(rc == 0x18) //-- got ACK after CLA + W
         break;
      else
      {
      }
   }
   if(i == I2C_WR_TIMEOUT)
      return I2C_ERR_WR_TIMEOUT;
   return I2C_NO_ERR;
}

//===========================================================================
//================ I/O Extender PCA9555  ==================================
//===========================================================================

//----------------------------------------------------------------------------
U8 pca9555_write(U8 Mode, U8 addr, U16 val){
   U8 ctrl;
   U8 rc;
   ctrl = 0x40 | (addr<<1); //--- 0100 001 0
   rc = i2c_lpc_ctrl(ctrl);  //-- Now WR (RD/WR = 0)
   if(rc != I2C_NO_ERR)
      return rc;
   i2c_lpc_wr_byte(Mode);
   i2c_lpc_wr_byte(val & 0x000000FF);
   i2c_lpc_wr_byte((val>>8) & 0x000000FF);
   i2c_lpc_stop();
   return I2C_NO_ERR;
}

//----------------------------------------------------------------------------
U8 pca9555_read(U8 Mode,U8 addr , U16 *ret_val){
   U8 ctrl;
   U8 rc;
   U16 rd_val;

   unsigned char buf[4];
   ctrl = 0x40 | (addr<<1); //--- 0100 001 0
   rc = i2c_lpc_ctrl(ctrl); //-- Now WR (RD/WI = 0)
   if(rc != I2C_NO_ERR)
      return rc;
   i2c_lpc_wr_byte(Mode);//Command
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
//================ A/D Converter Max1239   ==================================
//===========================================================================
U8 max1239_write(U8 addr, U8 val){

   U8 ctrl;
   U8 rc;
   ctrl = (addr<<1); 
   rc = i2c_lpc_ctrl(ctrl);  //-- Now WR (RD/WR = 0)
   if(rc != I2C_NO_ERR)
      return rc;
   //--- wr
   i2c_lpc_wr_byte(val & 0x000000FF);
   i2c_lpc_stop();
   return I2C_NO_ERR;
}

U8 max1239_read(U8 addr , U16 *ret_val){
   U8 ctrl;
   U8 rc;
   U16 rd_val;

   unsigned char buf[4];
   ctrl = (addr<<1);
   rc = i2c_lpc_ctrl(ctrl | 0x01); //-- Now RD (RD/WI = 1)
   if(rc != I2C_NO_ERR)
      return rc;

   rc = i2c_lpc_rx_to_buf(&buf[0],2);
   if(rc != I2C_NO_ERR)
      return rc;

   i2c_lpc_stop();                 //-- Set STOP condition

   rd_val = (buf[0]<<8)& 0x0000FF00; //-- Port1
   rd_val |= buf[1];  //-- Port 0

   *ret_val = rd_val;

   return I2C_NO_ERR;
}
