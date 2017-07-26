/*
	Decoder.cpp - A library for use with the HTCL-2032 quadrature decoder
	Created by Andrew Micklich, September 2016
*/

#include "Arduino.h"
#include "Decoder.h"

Decoder::Decoder() {
  	// Private variables
	_resultx = 0;
	_resulty = 0;
	_result_lox = 0;
	_result_loy = 0;
	_result_3rdx = 0;
	_result_3rdy = 0;
	_refpointx = 0;
	_refpointy = 0;
	
}

void Decoder::start() {
	// Default to 4x count
	pinMode(EN_1,OUTPUT);
	digitalWrite(EN_1,HIGH);
	pinMode(EN_2, OUTPUT);
	digitalWrite(EN_2,LOW);
	// Default to X-axis readout
	pinMode(X_Y,OUTPUT);
	digitalWrite(X_Y,LOW);
	// Default to OE on
	pinMode(OE,OUTPUT);
	digitalWrite(OE,LOW);
	// Initialize SEL pins
	pinMode(SEL_1,OUTPUT);
	pinMode(SEL_2,OUTPUT);
	// RSTX high
	pinMode(RST_X,OUTPUT);
	digitalWrite(RST_X,HIGH);
	// RSTY high
	pinMode(RST_Y,OUTPUT);
	digitalWrite(RST_Y,HIGH);
	// Input Pins
	pinMode(D0, INPUT);
  	pinMode(D1, INPUT);
  	pinMode(D2, INPUT);
  	pinMode(D3, INPUT);
  	pinMode(D4, INPUT);
  	pinMode(D5, INPUT);
  	pinMode(D6, INPUT);
  	pinMode(D7, INPUT);
  	// CLK
  	pinMode(CLK, OUTPUT);
}

void Decoder::setCount4() {
	digitalWrite(EN_1,LOW);
	digitalWrite(EN_2,HIGH);
}

void Decoder::setCount2() {
	digitalWrite(EN_1,HIGH);
	digitalWrite(EN_2,LOW);
}

void Decoder::setCount1() {
	digitalWrite(EN_1,HIGH);
	digitalWrite(EN_2,HIGH);
}

int Decoder::readLSB(bool axis) {
	digitalWrite(SEL_1, HIGH);
	digitalWrite(SEL_2, LOW);
	if (axis == true) {
		digitalWrite(X_Y, HIGH);
		_result_loy = PINA;
		return _result_loy;
	}
	else if (axis == false) {
		digitalWrite(X_Y, LOW);
		_result_lox = PINA;
		return _result_lox;
	}
}

int Decoder::read3SB(bool axis) {
	digitalWrite(SEL_1,LOW);
	digitalWrite(SEL_2,LOW);
	if (axis == true) {
		digitalWrite(X_Y, HIGH);
		_result_3rdy = PINA;
		return _result_3rdy;
	}
	else if (axis == false) {
		digitalWrite(X_Y, LOW);
		_result_3rdx = PINA;
		return _result_3rdx;
	}
}

void Decoder::setPoint(bool axis) {
	if (axis == true){
		_resulty = _result_loy + 256*_result_3rdy;
		setPoint(_resulty, axis);
	}

	else if (axis == false) {
		_resultx = _result_lox + 256*_result_3rdx;
		setPoint(_resultx, axis);
	}
	

}

void Decoder::setPoint(int point, bool axis) {
	if (axis == true) {
		_refpointy = point;
	}
	else if (axis == false) {
		_refpointx = point;
	}
}

int Decoder::getAbsPos(bool axis) {
	if (axis == true) {
		_resulty = _result_loy + 256*_result_3rdy;
		return _resulty;
	}

	else if (axis == false) {
		_resultx = _result_lox + 256*_result_3rdx;
		return _resultx;
	}
}

int Decoder::getRelPos(bool axis) {
	if (axis == true) {
		return getRelPos(axis, _refpointy);
	}
	else if (axis == false) {
		return getRelPos(axis, _refpointx);
	}

}

int Decoder::getRelPos(bool axis, int point) {
	if (axis == true) {
		_resulty = _result_loy + 256*_result_3rdy - point;
		return _resulty;
	}
	else if (axis == false) {
		_resultx = _result_lox + 256*_result_3rdx - point;
		return _resultx;
	}
}
