/*
	Decoder.h - A library for use with the HTCL-2032 quadrature decoder
	Created by Andrew Micklich, August 2016
*/
#ifndef Decoder_h
#define Decoder_h

//pin defines
#define D0 0x16 //arduino pin 22
#define D1 0x17 //arduino pin 23
#define D2 0x18 //arduino pin 24
#define D3 0x19 //arduino pin 25
#define D4 0x1A //arduino pin 26
#define D5 0x1B //arduino pin 27
#define D6 0x1C //arduino pin 28
#define D7 0x1D //arduino pin 29

#define RST_Y 0x1E //arduino pin 30
#define X_Y 0x1F //arduino pin 31
#define EN_2 0x20 //arduino pin 32
#define EN_1 0x21 //arduino pin 33
#define SEL_2 0x22 //arduino pin 34
#define RST_X 0x23 //arduino pin 35
#define SEL_1 0x24 //arduino pin 36
#define OE 0x25 //arduino pin 37
#define CLK 0x26 //arduino pin 38

#include "Arduino.h"

class Decoder 
{
	public:
		Decoder(); // Constructor: Instantiates class variables
		void start(); // Performs setup on Arduino to interface with the decoder
		void reset(); //Sets everything back to position 0
		/*
			These three functions set the count mode of the decoder appropriately.
		*/
		void setCount4();
		void setCount2();
		void setCount1();
		/*
			ReadLSB: Takes boolean argument: 0 for x-axis, 1 for y-axis. Returns int
			containing the current LSB for the absolute position.
		*/
		int readLSB(bool axis);
		/*
			Read3SB: Same as readLSB, except that it returns an int containing the
			third MSB of the absolute position.
		*/
		int read3SB(bool axis);
		/*
			setPoint: Allows user to set a reference point using int. With no args,
			this defaults to the currently stored point.
		*/
		void setPoint(bool axis);
		void setPoint(int point, bool axis);
		// getAbsPos: Returns currently stored absolute position of the encoder
		int getAbsPos(bool axis);
		/* 
			getRelPos: Subtracts reference point from currently stored absolute
			position, then returns result as an int. With no point arg, uses currently
			stored reference point.
		*/
		int getRelPos(bool axis);
		int getRelPos(bool axis, int point);
	private:
		int _resultx;
		int _resulty;
		int _result_lox;
		int _result_loy;
		int _result_3rdx;
		int _result_3rdy;
		int _refpointx;
		int _refpointy;
};

#endif
