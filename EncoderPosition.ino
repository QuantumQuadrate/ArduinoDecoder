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
// Other Useful Info:
// Toggling between the X and Y channels is explained in the Decoder.h file as follows:
// ReadLSB and READ3SB take boolean arguements, 0 for x axis, and 1 for y axis
#include <Decoder.h>
#include <EEPROM.h>

Decoder decoder;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  decoder.start();
  Serial.println("Start");
}

short int Result;
short int Result_old;
int Result_lo;
int Result_3rd;
char inByte;
char motorAxis;

void loop() {
  //Need to read the serial port until we get values dictating what happens next
  while(Serial.available()>0){
    inByte = Serial.read();
    if(int(inByte) == 2){
          ReadReply();
    }
    if(int(inByte) == 3){
          StatusReply();
    }
  }
}


void StatusReply(){
    Serial.println(EEPROM.read(0));
}

void ReadReply(){

    motorAxis = Serial.read();
    while(motorAxis == -1){
      motorAxis = Serial.read();
    }
    digitalWrite(38, HIGH);
    Result_lo = decoder.readLSB(int(motorAxis));
    Result_3rd = decoder.read3SB(int(motorAxis));
    Result = Result_lo + (256*Result_3rd);

    Serial.println(Result, HEX);
    digitalWrite(38,LOW);
    Result_old = Result;

}
