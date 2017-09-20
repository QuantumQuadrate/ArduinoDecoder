from time import sleep
from SerialComm import ArduinoSerial

s = ArduinoSerial('COM5', record=True)
s.START()

nold = 0
mold = 0

while(1):
	n = s.READ("x")
	m = s.READ("y")
	sleep(1)
	print("n: {}, delta_n: {}".format(n, n - nold))
	print("m: {}, delta_m: {}".format(m, m - mold))
	nold = n
	mold = m
