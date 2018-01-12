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
  encoder2_PinA = 25,   // right
  encoder2_PinB = 26,   // left
  // Channel 3
  encoder3_PinA = 22,   // right
  encoder3_PinB = 23,   // left
  // Channel 4
  encoder4_PinA = 38,   // right
  encoder4_PinB = 39,   // left
};

int32_t encoder1_Result=0;
int32_t encoder2_Result=0;
int32_t encoder3_Result=0;
int32_t encoder4_Result=0; // Result is position reading plus given offset.
int32_t encoder1_Result_old=0;
int32_t encoder2_Result_old=0;
int32_t encoder3_Result_old=0;
int32_t encoder4_Result_old=0;
int32_t encoder1_offset= 0;
int32_t encoder2_offset= 0;
int32_t encoder3_offset= 0;
int32_t encoder4_offset= 0;
int32_t encoder1_Pos, encoder2_Pos, encoder3_Pos, encoder4_Pos;
int eeprom_address1= 2;
int eeprom_address2= 6;
int eeprom_address3= 10;
int eeprom_address4= 14;

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
    encoder1_offset = EEPROMRead32(eeprom_address1);
    encoder2_offset = EEPROMRead32(eeprom_address2);
    encoder3_offset = EEPROMRead32(eeprom_address3);
    encoder4_offset = EEPROMRead32(eeprom_address4);
    encoder1_Result = encoder1_offset;
    encoder2_Result = encoder2_offset;
    encoder3_Result = encoder3_offset;
    encoder4_Result = encoder4_offset;
    encoder1_Result_old = encoder1_Result;
    encoder2_Result_old = encoder2_Result;
    encoder3_Result_old = encoder3_Result;
    encoder4_Result_old = encoder4_Result;
  }
  // Initializes encoder position to zero or to the stored values.
  encoder1.write(encoder1_Result);
  encoder2.write(encoder2_Result);
  encoder3.write(encoder3_Result);
  encoder4.write(encoder4_Result);

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
  encoder1_Pos=encoder1.read();
  if (encoder1_Result_old != encoder1_Pos) {
    encoder1_Result_old = encoder1_Pos;
    encoder1_Result=encoder1_Result_old;
  }
  encoder2_Pos=encoder2.read();
  if (encoder2_Result_old != encoder2_Pos) {
    encoder2_Result_old = encoder2_Pos;
    encoder2_Result=encoder2_Result_old;
  }
  encoder3_Pos=encoder3.read();
  if (encoder3_Result_old != encoder3_Pos) {
    encoder3_Result_old = encoder3_Pos;
    encoder3_Result=encoder3_Result_old;
  }
  encoder4_Pos=encoder4.read();
  if (encoder4_Result_old != encoder4_Pos) {
    encoder4_Result_old = encoder4_Pos;
    encoder4_Result=encoder4_Result_old;
  }
}

//writes a position value to the eeprom
void EepromWrite(){
  if((millis()-timer4) >= 10000UL){
    if(encoder4_Result_old != encoder4_Result){
      EEPROMWrite32(eeprom_address4, encoder4_Result);
      encoder4_Result_old = encoder4_Result;
      EEPROMCheck();
    }
    timer4 = millis();
  }
    if((millis()-timer3) >= 10000UL){
      if(encoder3_Result_old != encoder3_Result){
        EEPROMWrite32(eeprom_address3, encoder3_Result);
        encoder3_Result_old = encoder3_Result;
        EEPROMCheck();
      }
      timer2 = millis();
    }
  if((millis()-timer2) >= 10000UL){
    if(encoder2_Result_old != encoder2_Result){
      EEPROMWrite32(eeprom_address2, encoder2_Result);
      encoder2_Result_old = encoder2_Result;
      EEPROMCheck();
    }
    timer2 = millis();
  }
  if((millis()-timer1) >= 10000UL){
    if(encoder1_Result_old != encoder1_Result){
      EEPROMWrite32(eeprom_address1, encoder1_Result);
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
void EEPROMWrite32(int address, int32_t value){
  //Decomposition from a 32bit signed int to bytes by using bitshift.
  //One = Most significant, Two = 2nd significant byte
  //Three = 3rd significant byte, Four=Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);
  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

//Reads in 32 bit integers from the eeprom
int32_t EEPROMRead32(int address){
  //Read the 2 bytes from the eeprom memory.
  int32_t four = EEPROM.read(address);
  int32_t three = EEPROM.read(address + 1);
  int32_t two = EEPROM.read(address + 2);
  int32_t one = EEPROM.read(address + 3);
  //Return the recomposed int by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
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
  EEPROMWrite32(eeprom_address1, 0);
  EEPROMWrite32(eeprom_address2, 0);
  EEPROMWrite32(eeprom_address3, 0);
  EEPROMWrite32(eeprom_address4, 0);
  Serial.println(EEPROM.read(1));
  encoder1_offset=0;
  encoder2_offset=0;
  encoder3_offset=0;
  encoder4_offset=0;
}

