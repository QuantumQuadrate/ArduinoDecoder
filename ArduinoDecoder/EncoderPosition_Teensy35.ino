// Arduino-based rotary decoder
// Supporting devices : Arduino Mega
// Will be tested on Teensy as they are capable of detecting faster
// Previous contributors -- Syd Lybert, Andrew Micklich -- June 2017
// Also based on rotary encoder codes found from https://playground.arduino.cc/Main/RotaryEncoders by rafbuff
#include <EEPROM.h>

// Encoder signals should be connected to pins capable of interrupts.
// Arduino Mega: 	2, 3, 18, 19, 20, 21
// Teensy 3.5 : All digital pins (double check)
enum PinAssignments {
  // Channel 1
  encoder1_PinA = 2,   // right
  encoder1_PinB = 3,   // left
  // Channel 2
  encoder2_PinA = 18,   // right
  encoder2_PinB = 19,   // left
};


int encoder1_Result; // Result is pos plus offset.
int encoder2_Result;
int encoder1_Result_old;
int encoder2_Result_old;
int encoder1_offset = 0;
int encoder2_offset= 0;
char inByte;
char motorAxis;

unsigned long timerX = millis();
unsigned long timerY = millis();

volatile unsigned int encoder1_Pos = 0;  // a counter for the dial
unsigned int encoder1_lastReportedPos = 1;   // change management
static boolean encoder1_rotating = false;    // debounce management

volatile unsigned int encoder2_Pos = 0;  // a counter for the dial
unsigned int encoder2_lastReportedPos = 1;   // change management
static boolean encoder2_rotating = false;    // debounce management


// interrupt service routine vars
boolean encoder1_A_set = false;
boolean encoder1_B_set = false;
boolean encoder2_A_set = false;
boolean encoder2_B_set = false;


//setup code
void setup() {
  Serial.begin(115200);
  pinMode(encoder1_PinA, INPUT);
  pinMode(encoder1_PinB, INPUT);
  pinMode(encoder2_PinA, INPUT);
  pinMode(encoder2_PinB, INPUT);
  // turn on pullup resistors
  digitalWrite(encoder1_PinA, HIGH);
  digitalWrite(encoder1_PinB, HIGH);
  digitalWrite(encoder2_PinA, HIGH);
  digitalWrite(encoder2_PinB, HIGH);

  // encoder pin on interrupt 0 (pin 2 for Mega)
  attachInterrupt(digitalPinToInterrupt(2), doEncoder1_A, CHANGE);
  // encoder pin on interrupt 1 (pin 3 for Mega)
  attachInterrupt(digitalPinToInterrupt(3), doEncoder1_B, CHANGE);
  // encoder pin on interrupt 5 (pin 18 for Mega)
  attachInterrupt(5, doEncoder2_A, CHANGE);
  // encoder pin on interrupt 4 (pin 19 for Mega)
  attachInterrupt(4, doEncoder2_B, CHANGE);
  //if the eeprom is already storing postions
  //this will store them  into the global variables
  if(EEPROM.read(1) == 100){
    encoder1_offset = EEPROMRead16(4);
    encoder2_offset = EEPROMRead16(2);
    encoder1_Result = encoder1_offset;
    encoder2_Result = encoder2_offset;
    encoder1_Result_old = encoder1_Result;
    encoder2_Result_old = encoder2_Result;
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
  encoder1_rotating = true;  // reset the debouncer
  encoder2_rotating = true;  // reset the debouncer

  DecoderWrite();
  EepromWrite();
}

//Saves the current position of the encoders to global variables
void DecoderWrite(){
  if (encoder1_lastReportedPos != encoder1_Pos) {
    encoder1_lastReportedPos = encoder1_Pos;
    encoder1_Result_old=encoder1_lastReportedPos+encoder1_offset;
    encoder1_Result=encoder1_Result_old;
  }
  if (encoder2_lastReportedPos != encoder2_Pos) {
    encoder2_lastReportedPos = encoder2_Pos;
    encoder2_Result_old=encoder2_lastReportedPos+encoder2_offset;
    encoder2_Result=encoder2_Result_old;
  }

}

//writes a position value to the eeprom
void EepromWrite(){
  if((millis()-timerY) >= 10000UL){
    if(encoder2_Result_old != encoder2_Result){
      EEPROMWrite16(2, encoder2_Result);
      encoder2_Result_old = encoder2_Result;
      EEPROMCheck();
    }
    timerY = millis();
  }
  if((millis()-timerX) >= 10000UL){
    if(encoder1_Result_old != encoder1_Result){
      EEPROMWrite16(4, encoder1_Result);
      encoder1_Result_old = encoder1_Result;
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
    Serial.println(encoder2_Result, HEX);
  }
  if(int(motorAxis) == 0){
    Serial.println(encoder1_Result, HEX);
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

void doEncoder1_A() {
  // debounce
  if (encoder1_rotating ) delay (1);  // wait a little until the bouncing is done

  // Test transition, did things really change?
  if ( digitalRead(encoder1_PinA) != encoder1_A_set ) { // debounce once more
    encoder1_A_set = !encoder1_A_set;

    // adjust counter + if A leads B
    if ( encoder1_A_set && !encoder1_B_set )
      encoder1_Pos += 1;

    encoder1_rotating = false;  // no more debouncing until loop() hits again
  }

}

// Interrupt on B changing state, same as A above
void doEncoder1_B() {
  if ( encoder1_rotating ) delay (1);
  if ( digitalRead(encoder1_PinB) != encoder1_B_set ) {
    encoder1_B_set = !encoder1_B_set;
    //  adjust counter - 1 if B leads A
    if ( encoder1_B_set && !encoder1_A_set )
      encoder1_Pos -= 1;

    encoder1_rotating = false;
  }

}

void doEncoder2_A() {
  // debounce
  if (encoder2_rotating ) delay (1);  // wait a little until the bouncing is done

  // Test transition, did things really change?
  if ( digitalRead(encoder2_PinA) != encoder2_A_set ) { // debounce once more
    encoder2_A_set = !encoder2_A_set;

    // adjust counter + if A leads B
    if ( encoder2_A_set && !encoder2_B_set )
      encoder2_Pos += 1;

    encoder2_rotating = false;  // no more debouncing until loop() hits again
  }

}

// Interrupt on B changing state, same as A above
void doEncoder2_B() {
  if ( encoder2_rotating ) delay (1);
  if ( digitalRead(encoder2_PinB) != encoder2_B_set ) {
    encoder2_B_set = !encoder2_B_set;
    //  adjust counter - 1 if B leads A
    if ( encoder2_B_set && !encoder2_A_set )
      encoder2_Pos -= 1;

    encoder2_rotating = false;
  }

}
