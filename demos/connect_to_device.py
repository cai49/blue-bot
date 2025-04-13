import serial.tools.list_ports

import sys
import time
import random
import serial as sr

ports = serial.tools.list_ports.comports()

serial = None
for port in ports:
	device_sn = port.serial_number
	device_name = port.name

	if device_sn == "MYSN00001":
		serial = sr.Serial(device_name, baudrate=115200, timeout=1.0)

if serial == None:
	sys.exit("Serial device not found")


serial.reset_input_buffer()
print("Serial OK")

message_ack = False

running = True
while running:
	try:
		if not message_ack:
			package = random.randint(0,100)

			print(f"Send: {package}")
			serial.write(str(package).encode('utf-8'))
			message_ack = True

		while serial.in_waiting <= 0:
			time.sleep(0.01)

		line = serial.readline().decode('utf-8').rstrip()
		
		print(line)

		if line == "ok":
			message_ack = False
			time.sleep(.5)
		if line == "off":
			running = False

	except KeyboardInterrupt:
		serial.reset_input_buffer()
		serial.close()

		running = False
		print("Successfully Closed Serial COM")
	except Exception as e:
		running = False
		print("An Exception occurred when exiting")
		print(e)

