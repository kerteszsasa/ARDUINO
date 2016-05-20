#define SHIFTREG_dataPin          8        //Pin connected to Data  pin (DS)    of 74HC595
#define SHIFTREG_latchPin         9        //Pin connected to latch pin (ST_CP) of 74HC595
#define SHIFTREG_clockPin         10       //Pin connected to clock pin (SH_CP) of 74HC595
#define BUTTON                    12
#define BUZZER                    13

int reg1 =1;
int reg2 =0;
int reg3 =0;
uint32_t wheel = 1;
long randNumber;  
   


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(SHIFTREG_latchPin, OUTPUT);
  pinMode(SHIFTREG_dataPin, OUTPUT);  
  pinMode(SHIFTREG_clockPin, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT);
  digitalWrite(BUTTON, HIGH);
  digitalWrite(BUZZER, LOW);
  registerWrite(0,0,0);
  randomSeed(analogRead(0));
}

void loop() {
  // put your main code here, to run repeatedly:

 if(digitalRead(BUTTON) == 0){
    randNumber = random(20, 255); 
  Serial.println(randNumber);
  return;
 }
 if(randNumber >0){
  countDown(randNumber);
  randNumber--;
 }

   
}

void countDown(int num){
  if(num < 255 && 101 <= num) step(50);
  if(num < 100 && 46 <= num) step(75);
  if(num < 45 && 26 <= num) step(100);
  if(num < 25 && 16 <= num) step(200);
  if(num < 15 && 9 <= num) step(300);
  if(num < 8 && 4 <= num) step(400);
  if(num < 3 && 1 <= num) step(500);
}

void step(int wait){
  digitalWrite(BUZZER, HIGH);
  delay(3);
  digitalWrite(BUZZER, LOW);
  delay(wait);
  reg1 = wheel & 0xFF;
  reg2 = (wheel>>8) & 0xFF;
  reg3 = (wheel>>16) & 0xFF;
  registerWrite(reg3,reg2,reg1);
  wheel *= 2;
  if( wheel == 65536) wheel =1;  // 16777216 if 24bit  // 65536 if 16bit
}

// This method sends bits to the shift register:
void registerWrite(int data1, int data2, int data3) {
  digitalWrite(SHIFTREG_latchPin, LOW);
  shiftOut(SHIFTREG_dataPin, SHIFTREG_clockPin, MSBFIRST, data1);
  shiftOut(SHIFTREG_dataPin, SHIFTREG_clockPin, MSBFIRST, data2);
  shiftOut(SHIFTREG_dataPin, SHIFTREG_clockPin, MSBFIRST, data3);
  digitalWrite(SHIFTREG_latchPin, HIGH);
}
