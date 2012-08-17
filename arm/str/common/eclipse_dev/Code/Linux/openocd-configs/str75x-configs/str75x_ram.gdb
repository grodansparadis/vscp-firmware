target remote localhost:3333
monitor reset
monitor sleep 500
monitor poll
monitor soft_reset_halt
monitor arm7_9 sw_bkpts enable
monitor mww 0x60000010 0x7270E2
monitor mdw 0x60000010
