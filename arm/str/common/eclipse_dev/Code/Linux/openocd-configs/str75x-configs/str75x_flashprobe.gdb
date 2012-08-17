target remote localhost:3333
monitor reset
monitor sleep 500
monitor poll
monitor soft_reset_halt
monitor flash protect 0 0 9 off
monitor flash banks
monitor flash info 0
monitor flash probe 0
monitor reset run
monitor sleep 500
