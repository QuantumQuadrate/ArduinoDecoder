// Decoder TEST v0.21 -- Andrew Micklich -- September 2016
// ------------------------------------------
// SETUP:
// Connect Arduino MEGA digital pins 22-29 to D0-D7 on the HTCL
// Connect digital pin 31 to X/Y
// Connect digital pins 33 and 32 to EN1, EN2 respectively
// Connect digital pin 34 to SEL2
// Connect digital pin 36 to SEL1
// Connect digital pin 35 to RSTX
// Connect digital pin 30 to RSTY
// Connect digital pin 36 to OE
// Connect digital pin 38 to CLK
// Connect 5V Arduino pin to 5V input on optical encoder and V_DD on HTCL
// Connect GND Arduino pin to GND on optical encoder and V_SS on HTCL
// Connect Channel A on encoder to CHA_X on HTCL
// Connect Channel B on encoder to CHB_X on HTCL
// NOTE: Index Channel not yet supported in code
// TEST TEXT
// --------------------------------------------
// Known Bugs:
// None
// --------------------------------------------
// Known Limitations:
// If encoder is turned too rapidly, will not record (Hardware limits? Clock limits?)
// Only 16-bit count currently, eventually can support 32-bit if necessary
#include <Decoder.h>

Decoder decoder;

void setup() {
  // put your setup code here, to run once:
  decoder.start();
  Serial.println("Start");
}

int16_t Result;
int16_t Result_old;
int Result_lo;
int Result_3rd;

void loop() {
  digitalWrite(38, HIGH);
  Result_lo = decoder.readLSB(0);
  Result_3rd = decoder.read3SB(0);
  Result = Result_lo + (256*Result_3rd);
  if (Result != Result_old) {
    Serial.println(Result);
  }
  digitalWrite(38,LOW);
  Result_old = Result;

}
