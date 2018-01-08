// Arduino-based rotary decoder
// Supporting devices : Arduino Mega
// Will be tested on Teensy as they are capable of detecting faster
// Previous contributors -- Syd Lybert, Andrew Micklich -- June 2017
// Also based on rotary encoder codes found from https://playground.arduino.cc/Main/RotaryEncoders by rafbuff
#include <Decoder.h>
#include <EEPROM.h>

Decoder decoder;

// Encoder signals should be connected to pins capable of interrupts.
// Arduino Mega: 	2, 3, 18, 19, 20, 21
// Teensy 3.5 : All digital pins (double check)
enum PinAssignments {
  // Channel 1
  encoder1_PinA = 2,   // right
  encoder1_PinB = 3,   // left
  // Channel 1
  encoder2_PinA = 18,   // right
  encoder2_PinB = 19,   // left

//  clearButton = 8    // another two pins
};

//TODO: Clean up global variables, probably don't need this many
int ResultX;
int ResultY;
int Result_oldY;
int Result_oldX;
int offsetX = 0;
int offsetY= 0;
char inByte;
char motorAxis;

unsigned long timerX = millis();
unsigned long timerY = millis();

volatile unsigned int encoderPos = 0;  // a counter for the dial
unsigned int lastReportedPos = 1;   // change management
static boolean rotating = false;    // debounce management

// interrupt service routine vars
boolean A_set = false;
boolean B_set = false;


//setup code
void setup() {
  Serial.begin(115200);
  //decoder.start();
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  pinMode(clearButton, INPUT);
  // turn on pullup resistors
  digitalWrite(encoderPinA, HIGH);
  digitalWrite(encoderPinB, HIGH);
  digitalWrite(clearButton, HIGH);

  // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(0, doEncoderA, CHANGE);
  // encoder pin on interrupt 1 (pin 3)
  attachInterrupt(1, doEncoderB, CHANGE);
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
  rotating = true;  // reset the debouncer

  DecoderWrite();
  EepromWrite();
}

//Saves the current position of the encoders to global variables
void DecoderWrite(){
  if (lastReportedPos != encoderPos) {
    //Serial.print("Index:");
    //Serial.println(encoderPos, DEC);
    lastReportedPos = encoderPos;
    Result_oldX=lastReportedPos;
    ResultX=Result_oldX;
  }
  if (digitalRead(clearButton) == LOW )  {
    encoderPos = 0;
  }
  // Result_lo = decoder.readLSB(1);
  // Result_3rd = decoder.read3SB(1);
  // ResultY = Result_lo + (256*Result_3rd) + offsetY;
  //
  // Result_lo = decoder.readLSB(0);
  // Result_3rd = decoder.read3SB(0);
  // ResultX = Result_lo + (256*Result_3rd) + offsetX;

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

void doEncoderA() {
  // debounce
  if ( rotating ) delay (1);  // wait a little until the bouncing is done

  // Test transition, did things really change?
  if ( digitalRead(encoderPinA) != A_set ) { // debounce once more
    A_set = !A_set;

    // adjust counter + if A leads B
    if ( A_set && !B_set )
      encoderPos += 1;

    rotating = false;  // no more debouncing until loop() hits again
  }

}

// Interrupt on B changing state, same as A above
void doEncoderB() {
  if ( rotating ) delay (1);
  if ( digitalRead(encoderPinB) != B_set ) {
    B_set = !B_set;
    //  adjust counter - 1 if B leads A
    if ( B_set && !A_set )
      encoderPos -= 1;

    rotating = false;
  }

}
