/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : adc.c                                                  *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

/*****************************************************************
* read ADC values
* mux = ADC Channel
*****************************************************************/
#include <global.h>

void init_adc(void){
/********************************************
 * Configure the Analog to Digital converter 0
*********************************************/
  AD0_CR |= ((PCLK / 4500000) << 8);
  AD0_CR |= BURST;         // put A/D into continuous convert mode
  AD0_CR &= ~(CLKS1 | CLKS2 | CLKS3);       //11 clocks/10 bit accuracy
  AD0_CR |= PDN;      //power up the unit
  PINSEL1 |= ((BIT26) & ~(BIT27) | (BIT24) & ~(BIT25))  ;  // AD0.2 Ilumination Sensor and AD0.1 Internal Temperature Sensor
  AD0_CR |= 0x01000000; // Start A/D Conversion
/********************************************
 * Configure the Analog to Digital converter 1
*********************************************/
  AD1_CR |= ((PCLK / 4500000) << 8);
  AD1_CR |= BURST;         // put A/D into continuous convert mode
  AD1_CR &= ~(CLKS1 | CLKS2 | CLKS3);       //11 clocks/10 bit accuracy
  AD1_CR |= PDN;      //power up the unit
  PINSEL0 |= (U32)(0x03<<20); // AD1.2 external Analog input
  AD1_CR |= 0x01000000; // Start A/D Conversion
}

 U16 ReadChannel0(U8 mux){
   AD0_CR = (AD0_CR & ~(0xff)) | (1<<mux);      //enable AD0.x only
   U32 ADCresult = 0;
   U16 ADCresmV = 0;
  AD0_CR |= 0x01000000; // Start A/D Conversion
  U32 ADC_CALC_RESULT = 0;
  for(U8 i=0;i<20;i++)
  {
    do{
      ADCresult = AD0_GDR; // Read A/D Data add values 
      } while ((ADCresult & 0x80000000) == 0); // Wait for end of A/D Conversion
      ADC_CALC_RESULT += ((ADCresult >> 6) & 0x03FF); 
  }
      ADC_CALC_RESULT /= 20;                         
      ADCresmV = (U16)(ADC_CALC_RESULT)*(2048/1024);         // 10 bit resolution, 2,048V max
//      debug_printf("potentiometer now reads %4.2f\n",ADCresmV);
  return ADCresmV;
  } 
  
  U16 ReadChannel1(U8 mux){
   AD1_CR = (AD1_CR & ~(0xff)) | (1<<mux);      //enable AD1.x only
   U32 ADCresult = 0;
   U16 ADCresmV = 0;
   AD1_CR |= 0x01000000; // Start A/D Conversion
  U32 ADC_CALC_RESULT = 0;
  for(U8 i=0;i<4;i++){
    do{
      ADCresult = AD1_GDR; // Read A/D Data Register Wandlungsergebnisse aufaddieren
    } while ((ADCresult & 0x80000000) == 0); // Wait for end of A/D Conversion
      ADC_CALC_RESULT += ((ADCresult >> 6) & 0x03FF); // Read A/D Data Register Wandlungsergebnisse aufaddieren
  }
      ADC_CALC_RESULT /= 4; 
      ADCresmV = (U16)(ADC_CALC_RESULT)*(2048/1024);         // 10 bit resolution, 2,048V max
//      debug_printf("potentiometer now reads  %4.2f\n",ADCresmV);
  return ADCresmV;
  }

