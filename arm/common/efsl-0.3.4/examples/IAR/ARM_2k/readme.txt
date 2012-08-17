This demo runs on the LPC214x, and will work with any part that has a SSP peripheral.
Whomever is using the SPI port please create two new files: arm2k_spi.c and arm2k_spi.h

The second serial output is used for stdio.   To use serial0 just change line 63 in demo.c

For external crystal of 12MHz the default baud is 115200, any other you will need to adjust arm2k_uart.c

extract.h -
  for big endian use check code from line 64 to 70

time.c/time.h -
  consolidated multiple calls to get time/date parts, only two calls now (should be only 1)

file.c -
  shifted moved date at end of conditional; closer to time

  added the following functions:
  euint8 file_feof(File* file);
  esint16 file_getc(File* file);
  esint16 file_putc(File* file, euint8* byte);


Joel Winarske
joelw@indyelectronics.com
