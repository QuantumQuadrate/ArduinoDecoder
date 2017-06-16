import csv
import serial
ser = serial.Serial('/dev/tty.usbmodem1411', 115200, timeout = 1)

class ArduinoSerial():


    def START(self):
	try:
        	print ser.readline()
		return
  
	except serial.serialutil.SerialException:
		pass
		

    def READ(self, motor):
	try:
		#serial write to the arduino
		ser.write(chr(2))
		n = None
		if motor=="x":
			wx = open('EncoderPositionX.csv', 'a')
			writerx = csv.writer(wx)
			ser.write(chr(0))
			#Read the newest output from the Arduino
			n = ser.readline()
			if n:
				m = int(n,16)
               			if m > 0x7FFFFFFF:
                			m -= 0x100000000
				writerx.writerow(['Position', m])
				return m
			else:
				return n
		elif motor=="y":
			wy =open('EncoderPositionY.csv', 'a')
			writery = csv.writer(wy)
			ser.write(chr(1))
			#Read the newest output from the Arduino
			n = ser.readline()
			if n:
				m = int(n,16)
        			if m > 0x7FFFFFFF:
                			m -= 0x100000000
				writery.writerow(['Position', m])
				wy.close
				return m
			else:
				return n
		else:
			#TODO: need a break here
			print "error, expecting x or y"
			return n
	except serial.serialutil.SerialException:
		#TODO: Stuff.
		pass

    def STATUS(self):
	try:
        	#get the serial number from the EEPROM of the arduino
		ser.write(chr(3))
		n = ser.readline()
		return n
	except serial.serialutil.SerialException:
		pass


