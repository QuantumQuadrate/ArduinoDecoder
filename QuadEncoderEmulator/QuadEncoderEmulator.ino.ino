// This code is derived from arduino forum user: cattledog's code posted at the following url:
// https://forum.arduino.cc/index.php?topic=383189.0

//Quadrature signal generator two outputs offset 90 degrees
//emulate rotary encoder
//Timer 1 CTC with ICR1 TOP
//Switch direction with Serial input F/R/S Enter Capital F S or R with not line ending


const char encTable[16] = {0, 1, -1, 0, -1, 0, -0, 1, 1, 0, 0, -1, 0, -1, 1, 0}; //gives -1, 0 or 1 depending on encoder movement
volatile long encoderCount;
volatile long errorCount;
volatile byte encState;
unsigned long prevDisplay;
unsigned long interval = 1000;

char Command = 'F'; //default state is Forward

void setup() {

  Serial.begin(115200);
  attachInterrupt(0, readEnc, CHANGE);
  attachInterrupt(1, readEnc, CHANGE);
 
  pinMode(9, OUTPUT); //output A
  pinMode(10, OUTPUT); //output B

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  TCCR1A = 0; //clear timer registers
  TCCR1B = 0;
  TCNT1 = 0;
  GTCCR |= 1 << PSRASY; //reset prescaler

  //ICR1 and Prescaler sets frequency
  //no prescaler .0625 us per count @ 16mhz
  //prescaler 8 .5 us per count

  TCCR1B |=  _BV(CS11); // prescaler 8
  //TCCR1B |= _BV(CS10); //no prescaler

  //counts are zero indexed 2edges per ICR1 period
  //numerical values for prescaler 8.
  //e.g. 10k period give 20k encoder counts

  ICR1 = 199;//10k ICR1 period  20k encoder counts
  //ICR1 = 99; //20k ICR1 period 40k encoder counts
  //ICR1 = 49; //40K ICR1 period 80k encoder counts
  //ICR1 = 46; //42.5K ICR1 period 85k encoder counts
  //ICR1 = 41; //47.5k ICR1 period 95K encoder counts
  //ICR1 = 39; //50k ICR1 period 100k encoder counts
  //ICR1 = 29; //66.6K ICR1 period 133k encoder counts
  //ICR1 = 19; //100k ICR1 period 200k encoder counts
  //ICR1 = 17;
  //ICR1 = 14;
  //ICR1 = 9;

  OCR1A = ICR1 - 1; //two different pulse widths almost 100% duty cycle
  OCR1B = OCR1A / 2; //offset by half period

  TCCR1B |= _BV(WGM13) | _BV(WGM12); //CTC mode with ICR1
  TCCR1A = _BV(COM1A0) | _BV(COM1B0); //Toggle OC1A/OC1B on compare match
}
void loop () {

  readCommand();

  if (Command == 'F')
  {
    OCR1A = ICR1 - 1; //two different pulse widths almost 100% duty cycle
    OCR1B = OCR1A / 2; //offset by half period
    TCCR1B |= _BV(CS11);//start timer
  }

  if (Command == 'R')
  {
    OCR1B = ICR1 - 1; //two different pulse widths almost 100% duty cycle
    OCR1A = OCR1B / 2; //offset by half period
    TCCR1B |=  _BV(CS11);//start timer
  }

  if (Command == 'S')
  {
    TCCR1B &= ~(_BV(CS11));//stop timer
  }

  if (millis() - prevDisplay >= interval)
  {
    prevDisplay += interval;
    noInterrupts();
    long copyEncoderCount = encoderCount;
    encoderCount = 0;
    long copyErrorCount = errorCount;
    errorCount = 0;
    interrupts();

    Serial.print(copyEncoderCount);
    Serial.print('\t');
    Serial.println(copyErrorCount);
  }
}

void readEnc() {
  encState = ((encState << 2) | ((PIND >> 2) & 3)) & 15; //use encoder bits and last state to form index
  encoderCount += encTable[encState];//update actual position on encoder movement
  if (encTable[encState] == 0)
    errorCount++;
}

void readCommand() {

  if (Serial.available() > 0)
  {
    Command = Serial.read();
    Serial.println(Command);
  }
}

