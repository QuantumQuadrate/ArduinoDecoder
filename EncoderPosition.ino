// Arduino Serial Communication -- Syd Lybert, Andrew Micklich -- June 2017
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

short int ResultX;
short int ResultY;
short int Result_oldY;
short int Result_oldX;
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
  DecoderWrite();
  EepromWrite();
}


void DecoderWrite(){
  Result_lo = decoder.readLSB(1);
  Result_3rd = decoder.read3SB(1);
  ResultY = Result_lo + (256*Result_3rd);
  
  Result_lo = decoder.readLSB(0);
  Result_3rd = decoder.read3SB(0);
  ResultX = Result_lo + (256*Result_3rd);
  
}

void EepromWrite(){

  if(Result_oldY != ResultY){
    //Y value is stored at eeprom(1)
    EEPROM.write(1, ResultY);
    Result_oldY = ResultY;
  }
  if(Result_oldX != ResultX){
    //X value is stored at eeprom(2)
    EEPROM.write(2, ResultX);
    Result_oldX = ResultX;
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
  if(int(motorAxis) == 1){
    Serial.println(ResultY, HEX);
  }
  if(int(motorAxis) == 0){
    Serial.println(ResultX, HEX);
  }

}
