# ArduinoDecoder

Arduino-based quadrature decoder for use with picomotors

Authors : Sydney Lybert, Andrew Micklich and Minho Kwon

Thanks to Sydney Lybert and Andrew Micklich for their early contribution. 

Latest stable version is made for Teensy 3.5, where build-in counters provide more accessible way to decode quadruture signal.

## Dependencies

  * pySerial (for serial communication)

## Serial Communication Files

EncoderPosition.ino - This is an Arduino program that works with the Decoder library.
Its purpose is serial communication between the pypico server and the Arduino managing the quadrature decoder.
The pypico server is found here: https://github.com/QuantumQuadrate/pypico.
The serial communication class is found here: https://github.com/QuantumQuadrate/pypico/tree/master/pypico/arduinoComm.

## Use

* The ReadReply() function responds to a request from the pypico server (the READ() function in the serial communication class) and returns the current postion of an encoder on either the x or y axis, as determined by the request.

* The StatusReply() function returns the serial number of the Arduino at the request of the server (the STATUS() function in the serial communication class), which is stored in the EEPROM at location 0

* The ResetEEPROM() function set the current positions of both encoders to zero at the reqest of the server (the RESET() funciton in the serial communication class) and clears the memory of the most recent position.

The arduino obtains the current encoder position from the decoder chip and stores it in non-volatile memory every 10 seconds. 


## Decoder Library

Decoder/Decoder.cpp - Library C++ file that defines several useful commands for use with the decoder IC.
Commands assume the user has connected the IC to the Arduino as specified in the .ino file.
Current Work: Matt has said he will look for ways to optimize the library code itself.

Decoder/Decoder.h - Header file for the decoder library.

Decoder/Keywords.txt - Keywords file so library classes and functions look nice in the Arduino IDE.

## Library Test Code

DecoderTest.ino - Arduino code that runs with the library in the Decoder folder.
Instructions on how to connect the decoder IC to the Arduino are in the .ino file.
Current Work: Will optimize speed by using a register command rather than DigitalWrite for the clock signal.

## Encoder Hardware

Encoder (HEDR-55L2-BY07) : Datasheet can be found [here](http://www.mouser.com/ds/2/678/V02-3823EN_DS_HEDR-5xxx_2014-10-300-909317.pdf)

Decoder (HCTL-2032-SC) :  Datasheet can be found [here](https://media.digikey.com/pdf/Data%20Sheets/Avago%20PDFs/HCTL-2032,2022.pdf)
