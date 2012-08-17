target remote localhost:3333
monitor reset
monitor sleep 500
monitor poll
monitor soft_reset_halt
monitor arm7_9 force_hw_bkpts enable
monitor mww 0xA0000050 0x01c0
monitor mdw 0xA0000050
monitor mww 0xE0005000 0x000F
monitor mww 0xE0005004 0x000F
monitor mww 0xE0005008 0x000F
monitor mww 0x6C000000 0x8001
monitor mdw 0x6C000000
monitor mww 0x6C000004 0x8001
monitor mdw 0x6C000004
