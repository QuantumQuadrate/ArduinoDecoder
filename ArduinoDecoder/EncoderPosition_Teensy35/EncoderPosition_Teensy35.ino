// Teensy quadrature rotary decoder
// Teensy version written by Minho Kwon Jan 2018
// Previous contributors -- Syd Lybert, Andrew Micklich -- June 2017

#include <EEPROM.h>
#include <Encoder.h> // Using encoder library comes with Teensy

// Encoder signals should be connected to pins capable of interrupts.
// Arduino Mega:   2, 3, 18, 19, 20, 21
// Teensy 3.5 : All digital pins. Note that interrupt # may be different from the digital pin #
// Safe way to do is to
enum PinAssignments {
  // Channel 1
  encoder1_PinA = 2,   // right
  encoder1_PinB = 3,   // left
  // Channel 2
  encoder2_PinA = 18,   // right
  encoder2_PinB = 19,   // left
  // Channel 3
  encoder3_PinA = 20,   // right
  encoder3_PinB = 21,   // left
  // Channel 4
  encoder4_PinA = 9,   // right
  encoder4_PinB = 10,   // left
};

int encoder1_Result=0;
int encoder2_Result=0;
int encoder3_Result=0;
int encoder4_Result=0; // Result is position reading plus given offset.
int encoder1_Result_old=0;
int encoder2_Result_old=0;
int encoder3_Result_old=0;
int encoder4_Result_old=0;
int encoder1_offset= 0;
int encoder2_offset= 0;
int encoder3_offset= 0;
int encoder4_offset= 0;
int encoder1_Pos, encoder2_Pos, encoder3_Pos, encoder4_Pos;
char inByte;
char motorAxis;
unsigned long timer1 = millis();
unsigned long timer2 = millis();
unsigned long timer3 = millis();
unsigned long timer4 = millis();

Encoder encoder1(encoder1_PinA, encoder1_PinB);
Encoder encoder2(encoder2_PinA, encoder2_PinB);
Encoder encoder3(encoder3_PinA, encoder3_PinB);
Encoder encoder4(encoder4_PinA, encoder4_PinB);


//setup code
void setup() {
  Serial.begin(115200);
  //if the eeprom is already storing postions
  //this will store them into the global variables
  if(EEPROM.read(1) == 100){
    encoder1_offset = EEPROMRead16(4);
    encoder2_offset = EEPROMRead16(2);
    encoder1_Result = encoder1_offset;
    encoder2_Result = encoder2_offset;
    encoder3_Result = encoder3_offset;
    encoder4_Result = encoder4_offset;
    encoder1_Result_old = encoder1_Result;
    encoder2_Result_old = encoder2_Result;
    encoder3_Result_old = encoder3_Result;
    encoder4_Result_old = encoder4_Result;
  }
  // Initializes encoder position to zero
  encoder1.write(encoder1_Result)
  encoder2.write(encoder2_Result)
  encoder3.write(encoder3_Result)
  encoder4.write(encoder4_Result)

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
  DecoderRead();
  EepromWrite();
}

//Saves the current position of the encoders to global variables
void DecoderRead(){
  encoder1_Pos=encoder1.read()+encoder1_offset;
  if (encoder1_Result_old != encoder1_Pos) {
    encoder1_Result_old = encoder1_Pos;
    encoder1_Result=encoder1_Result_old;
  }
  encoder2_Pos=encoder2.read()+encoder2_offset;
  if (encoder2_Result_old != encoder2_Pos) {
    encoder2_Result_old = encoder2_Pos;
    encoder2_Result=encoder2_Result_old;
  }
  encoder3_Pos=encoder3.read()+encoder3_offset;
  if (encoder3_Result_old != encoder3_Pos) {
    encoder3_Result_old = encoder3_Pos;
    encoder3_Result=encoder3_Result_old;
  }
  encoder4_Pos=encoder4.read()+encoder4_offset;
  if (encoder4_Result_old != encoder4_Pos) {
    encoder4_Result_old = encoder4_Pos;
    encoder4_Result=encoder4_Result_old;
  }
}

//writes a position value to the eeprom
void EepromWrite(){
  if((millis()-timer2) >= 10000UL){
    if(encoder2_Result_old != encoder2_Result){
      EEPROMWrite16(2, encoder2_Result);
      encoder2_Result_old = encoder2_Result;
      EEPROMCheck();
    }
    timer2 = millis();
  }
  if((millis()-timer1) >= 10000UL){
    if(encoder1_Result_old != encoder1_Result){
      EEPROMWrite16(4, encoder1_Result);
      encoder1_Result_old = encoder1_Result;
      EEPROMCheck();
    }
    timer1 = millis();
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
  if(int(motorAxis) == 0){
    Serial.println(encoder1_Result, HEX);
  }
  if(int(motorAxis) == 1){
    Serial.println(encoder2_Result, HEX);
  }
  if(int(motorAxis) == 2){
    Serial.println(encoder3_Result, HEX);
  }
  if(int(motorAxis) == 3){
    Serial.println(encoder4_Result, HEX);
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
  encoder1_offset=0;
  encoder2_offset=0;
}

