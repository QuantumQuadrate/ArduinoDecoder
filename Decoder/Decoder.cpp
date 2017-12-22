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
	Serial.begin(115200);
	pinMode(33,OUTPUT);
	digitalWrite(33,HIGH);
	pinMode(32, OUTPUT);
	digitalWrite(32,LOW);
	// Default to X-axis readout
	pinMode(31,OUTPUT);
	digitalWrite(31,LOW);
	// Default to OE on
	pinMode(37,OUTPUT);
	digitalWrite(37,LOW);
	// Initialize SEL pins
	pinMode(36,OUTPUT);
	pinMode(34,OUTPUT);
	// RSTX high
	pinMode(35,OUTPUT);
	digitalWrite(35,HIGH);
	// RSTY high
	pinMode(30,OUTPUT);
	digitalWrite(30,HIGH);
	// Input Pins
	pinMode(22, INPUT);
  	pinMode(23, INPUT);
  	pinMode(24, INPUT);
  	pinMode(25, INPUT);
  	pinMode(26, INPUT);
  	pinMode(27, INPUT);
  	pinMode(28, INPUT);
  	pinMode(29, INPUT);
  	// CLK
  	pinMode(38, OUTPUT);
}

void Decoder::setCount4() {
	digitalWrite(33,LOW);
	digitalWrite(32,HIGH);
}

void Decoder::setCount2() {
	digitalWrite(33,HIGH);
	digitalWrite(32,LOW);
}

void Decoder::setCount1() {
	digitalWrite(33,HIGH);
	digitalWrite(32,HIGH);
}

int Decoder::readLSB(bool axis) {
	digitalWrite(36, HIGH);
	digitalWrite(34, LOW);
	if (axis == true) {
		digitalWrite(31, HIGH);
		_result_loy = PINA;
		return _result_loy;
	}
	else if (axis == false) {
		digitalWrite(31, LOW);
		_result_lox = PINA;
		return _result_lox;
	}
}

int Decoder::read3SB(bool axis) {
	digitalWrite(36,LOW);
	digitalWrite(34,LOW);
	if (axis == true) {
		digitalWrite(31, HIGH);
		_result_3rdy = PINA;
		return _result_3rdy;
	}
	else if (axis == false) {
		digitalWrite(31, LOW);
		_result_3rdx = PINA;
		return _result_3rdx;
	}
}

void Decoder::setPoint(bool axis) {
	if (axis == true){
		_resulty = (int16_t)_result_loy + 256*(int16_t)_result_3rdy;
		setPoint(_resulty, axis);
	}

	else if (axis == false) {
		_resultx = (int16_t)_result_lox + 256*(int16_t)_result_3rdx;
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
		_resulty = (int16_t)_result_loy + 256*(int16_t)_result_3rdy;
		return _resulty;
	}

	else if (axis == false) {
		_resultx = (int16_t)_result_lox + 256*(int16_t)_result_3rdx;
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
		_resulty = (int16_t)_result_loy + 256*(int16_t)_result_3rdy - point;
		return _resulty;
	}
	else if (axis == false) {
		_resultx = (int16_t)_result_lox + 256*(int16_t)_result_3rdx - point;
		return _resultx;
	}
}
