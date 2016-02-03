/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*      based on work copyrighted 2006-2009 Christian Olesch, Lab-Control      *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	MCP355X driver using portpins
*******************************************************************************/

#include "digiprobe_gcc.inc"

#define byteof(A,byte) (*(uint8_t*)(&A + byte))

void mcp355x_init(uint8_t flags)
{
	//Bits im DDR-Register einstellen
	cbi(MCP355X_DDR,MCP355X_SDO);
	sbi(MCP355X_DDR,MCP355X_CS);
	sbi(MCP355X_DDR,MCP355X_SCK);
	
	//Bits im PORT-Register einstellen
	sbi(MCP355X_PORT,MCP355X_SDO);	
	sbi(MCP355X_PORT,MCP355X_SCK);

	switch (flags) {
	case MCP355X_SINGLE:
		{
		//keep CS high for single conversion mode with explicite trigger
		//we don't trigger a measurement here, use mcp355x_trigger for that
		sbi(MCP355X_PORT,MCP355X_CS);
		}
	case MCP355X_CONTINOUS:
		{
		//keep CS low to enable continous mode and also trigger first 
		//measurement
		cbi(MCP355X_PORT,MCP355X_CS);
		}
	}	
};

// Trigger new measurement (in single conversion mode)
void mcp355x_trigger(void)
{
	cbi(MCP355X_PORT,MCP355X_CS);
	//_delay_us(200);
	NOP;
	sbi(MCP355X_PORT,MCP355X_CS);
}



uint8_t 	mcp355x_read(int32_t *value, uint8_t *adstatus, uint8_t flags)
{
	uint8_t retvalue;
	uint8_t i;
	int32_t dw;
	uint8_t *dw_bytes;
	uint8_t errorbits;
	
	dw_bytes=(uint8_t*) &dw;
	retvalue=0;

	//CS auf low setzen und warten bis SDO auf low geht, das
	//bedeutet dann ready
	cbi(MCP355X_PORT,MCP355X_CS);
	_delay_ms(1);
	//_delay_us(MCP355X_DELAY_US);

	//Prüfen ob Wandlung fertig ist
	//Wenn ja Ergebnis auslesen
	if(bit_is_clear(MCP355X_PIN,MCP355X_SDO))
		{
		retvalue = 1;
		dw=0;
		
		//24 Bits auslesen
		for(i=0;i<24;i++)
			{
				cbi(MCP355X_PORT,MCP355X_SCK);
				_delay_us(MCP355X_DELAY_US);
				//NOP;
				//NOP;
				sbi(MCP355X_PORT,MCP355X_SCK);
				_delay_us(MCP355X_DELAY_US);
				//NOP;
				//NOP;
				dw <<= 1; //Datenwort um ein Bit nach links
				//if(bit_is_clear(MCP355X_PIN,MCP355X_SDO)) dw++;
				if(bit_is_set(MCP355X_PIN,MCP355X_SDO)) dw++;
			}
		
		errorbits = *(dw_bytes+2);
		errorbits >>= 6;
		*adstatus=errorbits;
		retvalue = 1;
		
		//Errorbits aus Ergebnis herausmaskieren
		*(dw_bytes+2) &= 0b00111111;
		
		//Wenn kein OVL+
		if((errorbits&_BV(0))==0)
			{
			//Wenn Wert negativ, dann bits 22-31 negieren
			if(*(dw_bytes+2) & _BV(5))
				{
				*(dw_bytes+3)  = 255;
				*(dw_bytes+2) |= 0b11100000;
				}
			//Wenn OVL-, dann auch Bits 22-31 negieren
			if(errorbits & _BV(1))
				{
				*(dw_bytes+3)  = 255;
				*(dw_bytes+2) |= 0b11100000;
				}
			}
		
		//Wenn OVL+
		else
			{
			//Dann OVL+ -Bit aus dem Ergebnis rausmaskieren
			*(dw_bytes+2) &= ~_BV(6);
			};
			
		*value = dw;

		if (flags == MCP355X_CONTINOUS)
			{
			//Nächste Wandlung starten
			mcp355x_trigger();
			}
	}
	
	//sbi(MCP355X_PORT,MCP355X_SCK);
	sbi(MCP355X_PORT,MCP355X_CS);

	//if((*adstatus) == 1) (*value) += 2097152; //Wenn +Overflow, dann 2^21 Bits zuaddieren
	//if((*adstatus) == 2) (*value) -= 2097152; //Wenn -Overflow, dann -(2^21) Bits zuaddieren

	return retvalue;
};



