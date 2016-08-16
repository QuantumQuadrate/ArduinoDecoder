// Decoder TEST v0.2 -- Andrew Micklich -- August 2016
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
// Connect digital pin 13 to CLK
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

void setup() {
  Serial.begin(9600);
  // 4x mode
  // EN1 = 1
  pinMode(33,OUTPUT);
  digitalWrite(33, LOW);
  // EN2 = 0
  pinMode(32, OUTPUT);
  digitalWrite(32,HIGH);
  // X-axis
  pinMode(31, OUTPUT);
  digitalWrite(31, LOW);
  // OE Low
  pinMode(37, OUTPUT);
  digitalWrite(37, LOW);
  // SEL Pins
  pinMode(36, OUTPUT);
  pinMode(34, OUTPUT);
  // Set RSTX High
  pinMode(35, OUTPUT);
  digitalWrite(35, HIGH);
  // Initialize Input Pins
  pinMode(22, INPUT);
  pinMode(23, INPUT);
  pinMode(24, INPUT);
  pinMode(25, INPUT);
  pinMode(26, INPUT);
  pinMode(27, INPUT);
  pinMode(28, INPUT);
  pinMode(29, INPUT);
  // LED + CLK (LED labeled "L" should be on)
  pinMode(13, OUTPUT);

}

int16_t Result;
int16_t Result_old;
int Result_lo;
int Result_3rd;

void loop() {
  // Clock High
  digitalWrite(13, HIGH);
  // Wait
  delay(10);
  // Reading LSB
  digitalWrite(36, HIGH);
  digitalWrite(34, LOW);
  // LSB Input
  Result_lo = PINA;
  // Reading 3rd Byte
   digitalWrite (36, LOW);
  // 3rd Byte Input
  Result_3rd = PINA;
  Result = Result_lo + (256*Result_3rd);
  if (Result != Result_old) {
    Serial.println(Result);
  }
  // Clock Low
  digitalWrite(13, LOW);
  Result_old = Result;
  // Wait
  delay(10);

}
