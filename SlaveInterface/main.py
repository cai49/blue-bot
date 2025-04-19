import serial.tools.list_ports

import sys
import time
import random
import serial as sr

# Constants
TEENSY_SERIAL_NUMBER = "MYSN00001"
RPPICO_SERIAL_NUMBER = "E6605838835BA82C"

ARG_USE_RPICO	= '--use-rpico'
ARG_NOSERIAL= '--no-serial'

ARG_PSET = '-pset:'
ARG_PGET = '-pget:'

ARG_CSET = '-cset:'
ARG_CGET = '-cget'


# Flags
__NO_SERIAL = False
__COORD_MODE_ABS = True
__SERIAL_NUMBER = TEENSY_SERIAL_NUMBER

# Robot Coordinate Variables
X = 0.
Y = 0.
Z = 0.

# CLI arguments
args = sys.argv

for k, arg in enumerate(args, start=1):
	if ARG_NOSERIAL in arg:
		__NO_SERIAL = True
		continue

	if ARG_USE_RPICO in arg:
		__SERIAL_NUMBER = RPPICO_SERIAL_NUMBER
		continue
		
	if ARG_PSET in arg:
		arg_data = arg.removeprefix(ARG_PSET)

		if '[' in arg_data:
			if arg_data[0] == '[':
				_data = arg_data.strip("[]")
				_datums = _data.split(',')

				match len(_datums):
					case 3:
						X, Y, Z = [float(datum) for datum in _datums]
						print(f"XYZ changed: X={X}, Y={Y}, Z={Z}")
					case 2:
						X, Y = [float(datum) for datum in _datums]
						print(f"XY changed: X={X}, Y={Y}")
					case 1:
						X = float(_datums[0])
						print(f"X changed: X={X}")
		else:
			match arg_data[0]:
				case 'X':
					X = float(arg_data[1:])
					print(f'X changed: {X}')
				case 'Y':
					Y = float(arg_data[1:])
					print(f'Y changed: {Y}')
				case 'Z':
					Z = float(arg_data[1:])
					print(f'Z changed: {Z}')
		
		continue

	if ARG_PGET in arg:
		arg_data = arg.removeprefix(ARG_PGET)
		continue

	if ARG_CSET in arg:
		arg_data = arg.removeprefix(ARG_CSET)
		continue

	if ARG_CGET in arg:
		arg_data = arg.removeprefix(ARG_CGET)
		continue



# Serial port identification
if not __NO_SERIAL:
	ports = serial.tools.list_ports.comports()

	serial = None
	for port in ports:
		device_sn = port.serial_number
		device_name = port.name

		if device_sn == __SERIAL_NUMBER:
			serial = sr.Serial(device_name, baudrate=115200, timeout=1.0)

	if serial == None:
		sys.exit("Serial device not found")

	serial.reset_input_buffer()
	print("Serial OK")

serial.write(str("MAKE RECEIVE").encode('utf-8'))

while serial.in_waiting <= 0:
	time.sleep(1)

while serial.in_waiting > 0:
	line = serial.readline().decode('utf-8').rstrip()
	print(line)

