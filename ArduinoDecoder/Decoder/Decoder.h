/*
	Decoder.h - A library for use with the HTCL-2032 quadrature decoder
	Created by Andrew Micklich, August 2016
*/
#ifndef Decoder_h
#define Decoder_h

#include "Arduino.h"

class Decoder 
{
	public:
		Decoder(); // Constructor: Instantiates class variables
		void start(); // Performs setup on Arduino to interface with the decoder
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