

// Arduino Ethernet Communication -- Syd Lybert, Andrew Micklich -- July 2017
// ------------------------------------------
// SETUP:
// Connect Arduino MEGA digital pins 22-29 to D0-D7 on the HTCL
// Connect digital pin 31 to X/Y
// Connect digital pins 33 and 32 to EN1, EN2 respectively
// Connect digital pin 34 to SEL2
// Connect digital pin 36 to SEL1
// Connect digital pin 35 to RSTX
// Connect digital pin 30 to RSTY
// Connect digital pin 37 to OE
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
#include <UIPEthernet.h>

EthernetServer server = EthernetServer(1000);
EthernetClient client;
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
char msg[256];
int motorAxis; 


unsigned long timerX = millis();
unsigned long timerY = millis();

//setup code
void setup() {
  Serial.begin(115200);
  decoder.start();
  //assign mac and ip addresses, start the server and ethernet communication
  byte lastByte = EEPROM.read(0);
  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x02};
  IPAddress myIP(192,168,0,2);
  Ethernet.begin(mac,myIP);
  server.begin();
  
}

//main loop
void loop() {
  //Need to read the ethernet port until we get values dictating what happens next
  if(client = server.available())
  {
    int i = 0;
    char c = 0;
    while((client.available() > 0) && (c != '\n')){
      //gather the message from the client
      c = client.read();
      if(c != '\n'){
        msg[i] = c;
      }
      i++; 
  }
    
    if(msg[0] == '5'){
      Start();
    }
    
    if(msg[0] == '2'){
      ReadReply();
    }
      
    if(msg[0] == '3'){
      StatusReply();
    }
    
    if(msg[0] == '4'){
      ResetEEPROM();  
   }

   DecoderWrite();
   EepromWrite();
  }
}

void Start(){
  client.println("Start\n");
}

//reads the arduino's serial number from the EEPROM
void StatusReply(){
    client.println(EEPROM.read(0));
}

//writes the encoder position to the client
void ReadReply(){
    int i = 0;
    char c = 0;
    while((client.available() > 0) && (c != '\n')){
      //gather the message from the client
      c = client.read();
      if(c != '\n'){
        msg[i] = c;
      }
      i++; 
  }
  motorAxis = msg[0] - '0';
  if(int(motorAxis) == 1){
    client.println(ResultY);
  }
  if(int(motorAxis) == 0){
    client.println(ResultX);
  }
  
}

//Saves the current position of the encoders to global variables
void DecoderWrite(){
  //Y position
  Result_lo = decoder.readLSB(1);
  Result_3rd = decoder.read3SB(1);
  ResultY = Result_lo + (256*Result_3rd) - offsetY;
  //X position
  Result_lo = decoder.readLSB(0);
  Result_3rd = decoder.read3SB(0);
  ResultX = Result_lo + (256*Result_3rd) - offsetX;
  
}

//writes a position value to the eeprom (every ten seconds, if it changes)
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
//This writes to the ethernet client and returns the reset condition
void ResetEEPROM(){
  EEPROM.write(1, 255);
  offsetX = offsetX + ResultX;
  offsetY = offsetY + ResultY;
  EEPROMWrite16(2, 0);
  EEPROMWrite16(4, 0);
  client.println(EEPROM.read(1));
}


