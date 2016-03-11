/*
  Shift Register Example
 for 74HC595 shift register

 This sketch turns reads serial input and uses it to set the pins
 of a 74HC595 shift register.

 Hardware:
 * 74HC595 shift register attached to pins 2, 3, and 4 of the Arduino,
 as detailed below.
 * LEDs attached to each of the outputs of the shift register

 Created 22 May 2009
 Created 23 Mar 2010
 by Tom Igoe

 */


 
#define SHIFTREG_dataPin 10        //Pin connected to Data  pin (DS)    of 74HC595
#define SHIFTREG_latchPin 9        //Pin connected to latch pin (ST_CP) of 74HC595
#define SHIFTREG_clockPin A0       //Pin connected to clock pin (SH_CP) of 74HC595



void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(SHIFTREG_latchPin, OUTPUT);
  pinMode(SHIFTREG_dataPin, OUTPUT);  
  pinMode(SHIFTREG_clockPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("reset");
}

void loop() {
  if (Serial.available() > 0) {
    // ASCII '0' through '9' characters are
    // represented by the values 48 through 57.
    // so if the user types a number from 0 through 9 in ASCII, 
    // you can subtract 48 to get the actual value:
    int bitToSet = Serial.read() - 48;

  // write to the shift register with the correct bit set high:
   registerWrite(bitToSet);
  }
}


// This method sends bits to the shift register:
void registerWrite(int data) {
  digitalWrite(SHIFTREG_latchPin, LOW);
  shiftOut(SHIFTREG_dataPin, SHIFTREG_clockPin, MSBFIRST, data);
  digitalWrite(SHIFTREG_latchPin, HIGH);
}
