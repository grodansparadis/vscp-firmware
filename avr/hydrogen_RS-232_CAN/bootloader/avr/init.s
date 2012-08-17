; Copyright (C) 2002 Jaroslaw Karwik, 
;   Mente Marine Oy <jaroslaw.karwik@mente-marine.com>
;
;
; CANDIP
; Changes akhe@eurosource.se http://www.eurosource.se
; 
; This file is free software; you can redistribute it and/or modify it
; under the terms of the GNU General Public License as published by the
; Free Software Foundation; either version 2, or (at your option) any
; later version.

; In addition to the permissions in the GNU General Public License, the
; author gives you unlimited permission to link the
; compiled version of this file into combinations with other programs,
; and to distribute those combinations without any restriction coming
; from the use of this file.  (The General Public License restrictions
; do apply in other respects; for example, they cover modification of
; the file, and distribution when not linked into a combine
; executable.)
;
; This file is distributed in the hope that it will be useful, but
; WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
; General Public License for more details.  
;

    .extern  _main
    .section .init

;
;		Note: GNU C/ASM compiler/assembler specific initialization
;

; 
; Code entered when the device is reseted
;
	
BOOT_PURE_RESET:	

	CLR R25		;  R25:R24 serves as first parameter to main()
	CLR R24		;  set the parameter to 0 (reset)
	RJMP INIT
; 
;  Code entered after 'boot' request from the application
; 
BOOT_APP_CALL:		

	LDI R24,0x01	;  set the main parameter to nonzero (request)

INIT:
	
	CLI             ; No interrupts in the boot code
	WDR		; Clear  watchdog timer
	
      ; Setup  watchdog to 0.5s ( 1<<WDCE )|( 1<<WDE )|( 111b ) 	
	LDI   R18, ( 1<<4 )|( 1<<3 )|( 0x07 )
	
      ; Enable change of watchdog parameters (WDTCR <- R18)	
	OUT   0x21, R18
	
      ; Actual change of watchdog parameters (WDTCR <- R18)	
	OUT   0x21, R18      
                  
      ; Note that the stack is being initialized 
      ; in the 'main' function (GNU specific)
                  
	RJMP  main                        ; Goto main ( C code) 














