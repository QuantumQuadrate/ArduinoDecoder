# ArduinoDecoder
Arduino-based quadrature decoder for use with picomotors

DecoderTest.ino - Arduino code that runs with the library in the Decoder folder.
Instructions on how to connect the decoder IC to the Arduino are in the .ino file.
Current Work: Will optimize speed by using a register command rather than DigitalWrite for the clock signal.

Decoder/Decoder.cpp - Library C++ file that defines several useful commands for use with the decoder IC.
Commands assume the user has connected the IC to the Arduino as specified in the .ino file.
Current Work: Matt has said he will look for ways to optimize the library code itself.

Decoder/Decoder.h - Header file for the decoder library.

Decoer/Keywords.txt - Keywords file so library classes and functions look nice in the Arduino IDE.
