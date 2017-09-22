from SerialComm import ArduinoSerial

s = ArduinoSerial('COM5', record=True)
s.START()
print "Attempting to reset saved values"
resp = s.RESET()
print "Received response: {}".format(resp)
