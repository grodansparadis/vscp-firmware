
// read serial number from PIC ID memory at address 0x200000 - 0x200007

uint32_t ReadIDMemory( void ) {
	uint32_t IdMemory=0;
	uint32_t Temp;
	uint8_t i;
	TBLPTR = (unsigned short long)0x200000;
	for(i=0; i<8; i++) {
		_asm
			TBLRDPOSTINC
		_endasm
		Temp = TABLAT;
		Temp <<= ((7-i)*4);
		IdMemory += Temp;
	}
	
	return IdMemory;
 }
 