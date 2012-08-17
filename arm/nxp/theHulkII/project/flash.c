/*

//
// Convert 'addr' to sector number  - Flash Programming
//
unsigned int get_secnum (void *addr)  {
  unsigned int n;

  n = ((unsigned int) addr >> 13) & 0x1F;        // pseudo sector number

  if (n >= (0x30000 >> 13))  {
    n -= 14;                                    // high small 8kB Sectors (
  }
  else if (n >= (0x10000 >> 13))  {
    n  = 7 + (n >> 3);                          // large 64kB Sectors
  }
  return (n);                                   // sector number
}


//
// Erase Sector between 'start' and 'end'
// Return:  IAP error code (0 when OK)
// NOTES:  start needs to be a 256 byte boundary
//         size should be 256, 512, 1024 or 4089
//
unsigned int erase (void* start, void* end)  {
  struct iap_in  iap;                      // IAP input parameters
  unsigned int result[16];                 // IAP results
  unsigned int save_VicInt;                // for saving of interrupt enable register

  save_VicInt = VICIntEnable;              // save interrupt enable status
  VICIntEnClr = 0xFFFFFFFF;                // disable all interrupts

#ifdef BYPASS_IAP
  stop_pll();                              // IAP requires to run without PLL
#endif

  iap.cmd = 50;                            // IAP Command: Prepare Sectors for Write
  iap.par[0] = get_secnum (start);         // start sector
  iap.par[1] = get_secnum (end);           // end sector
  iap_entry (&iap, result);                // call IAP function
  if (result[0])  goto exit;               // an error occured?

  iap.cmd = 52;                            // IAP command: Erase Flash
  iap.par[0] = get_secnum (start);         // start sector
  iap.par[1] = get_secnum (end);           // end sector
  iap.par[2] = CCLK;                       // CPU clock
  iap_entry (&iap, result);                // call IAP function

exit:

#ifdef BYPASS_IAP
  start_pll();                             // start PLL
#endif

  VICIntEnable = save_VicInt;              // enable interrupts
  return (result[0]);
}


//
// Program *data to flash_addr. number of bytes specified by size
// Return:  IAP error code (0 when OK)
// Note: 
//
unsigned int program (void *flash_addr, void *data, unsigned int size)  {
  struct iap_in  iap;                      // IAP input parameters
  unsigned int result[16];                 // IAP results
  unsigned int save_VicInt;                // for saving of interrupt enable register

  save_VicInt = VICIntEnable;              // save interrupt enable status
  VICIntEnClr = 0xFFFFFFFF;                // disable all interrupts

#ifdef BYPASS_IAP
  stop_pll();                              // IAP requires to run without PLL
#endif

  iap.cmd = 50;                            // IAP Command: Prepare Sectors for Write
  iap.par[0] = get_secnum (flash_addr);    // start sector
  iap.par[1] = iap.par[0];                 // end Sektor
  iap_entry (&iap, result);                // call IAP function
  if (result[0])  goto exit;               // an error occured?

  iap.cmd = 51;                            // IAP Command: Copy RAM to Flash
  iap.par[0] = (unsigned int) flash_addr;  // destination-addr
  iap.par[1] = (unsigned int) data;        // source-addr
  iap.par[2] = size;                       // number of bytes
  iap.par[3] = CCLK;                       // CPU clock
  iap_entry (&iap, result);                // call IAP function

exit:

#ifdef BYPASS_IAP
  start_pll();                             // start PLL
#endif

  VICIntEnable = save_VicInt;              // enable interrupts
  return (result[0]);
}

///////////////////////////////////////////////////////////////////////

	for (i = 0; i < sizeof (vals); i++)  {
		vals[i] = (unsigned char) i;
	}

	program( 0x30000, vals, sizeof( vals ) );
	program( 0x31000, vals, sizeof( vals ) );
	program( 0x32000, vals, sizeof( vals ) );
	erase( 0x30000, 0x31FFF );
	erase( 0x32000, 0x33FFF );


*/