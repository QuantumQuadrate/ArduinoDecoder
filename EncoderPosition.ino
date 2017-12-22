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

//TODO: Clean up global variables, probably don't need this many
int ResultX;
int ResultY;
int Result_oldY;
int Result_oldX;
int Result_lo;
int Result_3rd;
int offsetX = 0;
int offsetY= 0;
char inByte;
char motorAxis;

unsigned long timerX = millis();
unsigned long timerY = millis();

//setup code
void setup() {
  Serial.begin(115200);
  decoder.start();

  //if the eeprom is already storing postions
  //this will store them  into the global variables
  if(EEPROM.read(1) == 100){
    offsetY = EEPROMRead16(2);
    offsetX = EEPROMRead16(4);
    ResultX = offsetX;
    ResultY = offsetY;
    Result_oldX = ResultX;
    Result_oldY = ResultY;
  }
  Serial.println("Start");
}

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
    if(int(inByte) == 4){
      ResetEEPROM();
    }
  }
  DecoderWrite();
  EepromWrite();
}

//Saves the current position of the encoders to global variables
void DecoderWrite(){
  Result_lo = decoder.readLSB(1);
  Result_3rd = decoder.read3SB(1);
  ResultY = Result_lo + (256*Result_3rd) + offsetY;

  Result_lo = decoder.readLSB(0);
  Result_3rd = decoder.read3SB(0);
  ResultX = Result_lo + (256*Result_3rd) + offsetX;

}

//writes a position value to the eeprom
void EepromWrite(){
  if((millis()-timerY) >= 10000UL){
    if(Result_oldY != ResultY){
      EEPROMWrite16(2, ResultY);
      Result_oldY = ResultY;
      EEPROMCheck();
    }
    timerY = millis();
  }
  if((millis()-timerX) >= 10000UL){
    if(Result_oldX != ResultX){
      EEPROMWrite16(4, ResultX);
      Result_oldX = ResultX;
      EEPROMCheck();
    }
    timerX = millis();
  }
}

//writes the arduino's serial number to serial
void StatusReply(){
    Serial.println(EEPROM.read(0));
}

//writes the encoder position to serial
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

//Functions that writes sixteen bit integers to the eeprom
void EEPROMWrite16(int address, int value){
  //Decomposition from a int to bytes by using bitshift.
  //One = Most significant, Two = Least significant byte
  byte two = (value & 0xFF);
  byte one = ((value >> 8) & 0xFF);

  //Write the 2 bytes into the eeprom memory.
  EEPROM.write(address, two);
  EEPROM.write(address + 1, one);
}

//Reads in 16 bit integers from the eeprom
int EEPROMRead16(int address){
  //Read the 2 bytes from the eeprom memory.
  int two = EEPROM.read(address);
  int one = EEPROM.read(address + 1);

  //Return the recomposed int by using bitshift.
  return ((two << 0) & 0xFF) + ((one << 8) & 0xFFFF);
}

//Function to check if the Arduino is storing old positions
void EEPROMCheck(){
  if(EEPROM.read(1) == 255){
      EEPROM.write(1, 100);
  }
}

//Function to tell the arduino to forget previous positions
//This writes to serial and returns the reset condition
void ResetEEPROM(){
  EEPROM.write(1, 255);
  EEPROMWrite16(2, 0);
  EEPROMWrite16(4, 0);
  Serial.println(EEPROM.read(1));
  offsetX=0;
  offsetY=0;
}
