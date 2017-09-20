import serial


class ArduinoSerial(object):

	def __init__(self, port, baud=115200, record=False):
		self.port = port
		self.ser = serial.Serial(port, baud, timeout=1)
		self.record = record
		self.files = ['EncoderPositionX.dat', 'EncoderPositionY.dat']
		self.channels = 2

	def START(self):
		try:
			print self.ser.readline()
			#TODO: Check for START in buffer
			return 0

		except self.serial.self.serialutil.SerialException:
			print "There was a self.serial/usb error"
			return 1

	def READ(self, motor):
		try:
			# serial write to the arduino
			self.ser.write(chr(2))  # read command
			n = None
			if motor == "x":
				channel = 0
			elif motor == "y":
				channel = 1
			else:
				# TODO: need a break here
				print "error, expecting x or y"
				raise KeyError

			self.ser.write(chr(channel))  # read channel position
			# Read the newest output from the Arduino
			n = self.ser.readline()  # TODO newline
			if n:
				# TODO: implement ASCII
				m = int(n, 16)
				if m > 0x7FFFFFFF:
					m -= 0x100000000
				if self.record:
					with open(self.files[channel], 'a') as f:
						f.write(str(m) + '\n')
				return m
			else:
				return n

		except serial.serialutil.SerialException:
			# TODO: Stuff.
			print "There was a self.serial/usb error"

	def STATUS(self):
		try:
			# get the self.serial number from the EEPROM of the arduino
			self.ser.write(chr(3))
			n = self.ser.readline()
			return n
		except serial.serialutil.SerialException:
			print "There was a self.serial/usb error"
