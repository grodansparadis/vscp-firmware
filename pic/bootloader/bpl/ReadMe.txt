BOOT18, aka Philpem's PIC18 Bootloader
Version 1.0
Copyright 2004 Philip Pemberton <philpem@dsl.pipex.com>
The latest version will always be available from http://www.philpem.dsl.pipex.com/

#################
## changelog   ##
#################
  Version 1.3 (15 September 2004):
    Added extra error checking. The "loadhex exited with code" error codes are:
      0: No error (you should never get an "exited with code 0" error message)
      1: Not a hex file
      2: Invalid checksum
      3: Reached end-of-file marker (you should never get an "exited with code 3"
         error message)

  Version 1.2 (unreleased):
    Major bugfix in hex-loader's hex-checksum routine. This bug had a 1 in 256
    chance of appearing - if the checksum in the hex file was zero and the checksum
    that was calculated by the loader was zero, the hex file would fail to load,
    with a "Checksum error". This has now been fixed (hopefully).

#################
## info        ##
#################

I wrote BOOT18 because I couldn't find a bootloader that met my needs. Basically,
these were:
  - Must occupy the Boot Block (program FLASH addresses 0x000 through 0x1FF)
    (this allows me to enable the PIC device's Boot Block Protect config flag
     to prevent the bootblock from getting overwritten).
  - Must be reasonably fast (faster than ICSP)
  - Must NOT use an I/O pin to activate the bootloader
  - Must use the onboard USART to transfer data between the chip and the host

BOOT18 was written in PIC18 Assembler using Microchip's MPLAB development tools.
It uses the PIC18Fxx2's onboard USART - this makes RS232 communications much more
stable and much easier than if I'd used "bit banging" techniques. Because I use the
USART anyway (for debuggging), I don't end up sacrificing I/O pins.
The bootloader activates after it sees a particular activation sequence on the serial
lines - briefly, the bootloader sends a "H", then waits 500mS for an "i" response from
the host. If the "i" isn't received within 500mS, the loader jumps to user code. If the
"i" was received, the loader sends a "K" as an acknowledgement and starts processing
commands.
A full protocol reference is included in the file "Protocol Reference.rtf". This
covers most of the nasty details of the loader's communications protocol. I've also
included a small (well, for x86 systems) uploader, written in Borland Delphi. I've
also included the source code. Yes, I know it's not pretty, but it gets the job done.

If you make use of this bootloader, I would appreciate it if you sent me a
short email to tell me where you use it, what you use it for, etc.
If you want a new feature implementing, email me. Even better, if you've modified my
code to improve it in some way, please contact me and I'll consider including your
new feature in the next release of BOOT18.

And now the legalese...
;   Copyright (c) 2004, Philip Pemberton                                      *
;   All rights reserved.                                                      *
;                                                                             *
;   Redistribution and use in source and binary forms, with or without        *
;   modification, are permitted provided that the following conditions are    *
;   met:                                                                      *
;                                                                             *
;     * Redistributions of source code must retain the above copyright        *
;         notice, this list of conditions and the following disclaimer.       *
;     * Redistributions in binary form must reproduce the above copyright     *
;         notice, this list of conditions and the following disclaimer in     *
;         the documentation and/or other materials provided with the          *
;         distribution.                                                       *
;                                                                             *
;   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
;   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         *
;   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR     *
;   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT      *
;   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
;   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED  *
;   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    *
;   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    *
;   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      *
;   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        *
;   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              *
