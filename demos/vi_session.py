import serial.tools.list_ports

import sys
import serial as sr

ports = serial.tools.list_ports.comports()


serial = None
for port in ports:
	device_name = port.name
		
	serial = sr.Serial(device_name, baudrate=115200, timeout=1.0)

if serial == None:
	sys.exit("Serial device not found")

serial.reset_input_buffer()
print("Serial OK")
