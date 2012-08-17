cd "C:\development\hydrogen - RS-232 to CAN\bootloader_isp\"
C:
del bootloader.map
del bootloader.lst
"C:\Program Files\Atmel\AVR Tools\AvrAssembler\avrasm32.exe" -fI "C:\development\hydrogen - RS-232 to CAN\bootloader_isp\cstartup.asm" -o "bootloader.hex" -d "bootloader.obj" -e "bootloader.eep" -I "C:\development\hydrogen - RS-232 to CAN\bootloader_isp" -I "C:\Program Files\Atmel\AVR Tools\AvrAssembler\AppNotes" -w  -m "bootloader.map" -l "bootloader.lst"
