target remote localhost:3333
monitor reset
monitor sleep 500
monitor poll
monitor soft_reset_halt
monitor arm7_9 force_hw_bkpts enable
monitor mww 0x40000000 0x00000200
monitor mdw 0x40000000
