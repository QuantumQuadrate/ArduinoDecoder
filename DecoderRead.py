from time import sleep
from SerialComm import ArduinoSerial

s = ArduinoSerial(record=True)
s.START()

while(1):
	n = s.READ("x")
	m = s.READ("y")
#	sleep(5)
