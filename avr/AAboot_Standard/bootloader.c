//actual bootloader routines












/*
success nr 1
	uint32_t i=0; 
    



//999999999999999999999999999999999   
   uint8_t c[256] = "0123456789"; 

   for(i = 10; i < 256; i++) 
      c[i] = 0xAA; 

   boot_program_page(0x02, c); 

   return (0); 

while(1)
{}
succes nr 2
uint8_t testpage[BTL_BLOCKSIZE+1] = "1234567890123456";
								boot_program_page (1,testpage);
								boot_program_page (0xF0,testpage); //messes up previous write, minimum pagesize is then 0x100

succes 3 , complete blocks
//test page writing
								uint8_t testpage[BTL_BLOCKSIZE+1] = "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456";
								boot_program_page (0x00,testpage);
								testpage[0] = 'A';
								boot_program_page (0x100,testpage);


*/
