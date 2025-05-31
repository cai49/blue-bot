from machine import Pin
from rp2 import PIO, StateMachine, asm_pio
from time import sleep
import select
import sys

@asm_pio(out_init=(rp2.PIO.OUT_HIGH,), out_shiftdir=PIO.SHIFT_RIGHT, autopull=True, pull_thresh=1)
def paral_prog():
	pull()
	out(pins, 1)

paral_sm = StateMachine(0, paral_prog, freq=10000000, out_base=Pin(25))
paral_sm.active(1)

poll_obj = select.poll()
poll_obj.register(sys.stdin, select.POLLIN)

sys.stdout.write(">")
while True:
    poll_results = False 
    
    while not poll_results:
        poll_results = poll_obj.poll(1)
    
    data = sys.stdin.readline().strip()
    
    if data == "":
       continue
    
    sys.stdout.write(f"Received data: {data}\n\r")

    for i, k in enumerate([1, 0, 0, 0, 1, 0]):
        paral_sm.put(k)
        sys.stdout.write(str(k))
        sleep(.01)
    
    sys.stdout.write("\n\r")
    sys.stdout.write(">")
