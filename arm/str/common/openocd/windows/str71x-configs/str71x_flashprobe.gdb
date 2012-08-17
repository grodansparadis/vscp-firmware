target remote localhost:3333
monitor reset
monitor sleep 500
monitor poll
monitor soft_reset_halt
monitor flash banks
monitor flash probe 0
monitor flash info 0
monitor flash probe 1
monitor flash info 1
monitor reset run
