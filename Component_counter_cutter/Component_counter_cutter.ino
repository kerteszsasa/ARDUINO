
//******DEFINE HW PINS******//
// STEPPER MOTOR PINS: connected to a shift register
#define SHIFTREG_dataPin            10        //Pin connected to Data  pin (DS)    of 74HC595
#define SHIFTREG_latchPin           9        //Pin connected to latch pin (ST_CP) of 74HC595
#define SHIFTREG_clockPin           A0       //Pin connected to clock pin (SH_CP) of 74HC595
// STEPPER SPEEDS
#define SPEED_STRIP_MANUAL_SLOW     100
#define SPEED_STRIP_MANUAL_FAST     50
// LCD PINS
#define LCD_RS_PIN                12
#define LCD_EN_PIN                11
#define LCD_D4_PIN                7
#define LCD_D5_PIN                6
#define LCD_D6_PIN                5
#define LCD_D7_PIN                4
// ENCODER PINS (must be connected to hwINT0 and hwINT1)
#define ENCODER_PIN_A             2
#define ENCODER_PIN_B             3
// SENSOR PINS
#define SENSOR_COMPONENT_HOLES
#define SENSOR_COMPONENT_IN_POSITION
#define CUTTER_END_POSITION
// BUTTON PINS
#define BUTTON_START              A4
#define BUTTON_BACK               A5

#include <LiquidCrystal.h>
#include <Encoder.h>
LiquidCrystal lcd(LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);
//Encoder encoder(2, 3);

long encoderValue = 0;
long encoderValuePrev = 0;

int STEPPER_STRIP_STATE = 1;
int STEPPER_BLDE_STATE = 1;


//******STATES******//
int MENU_STATE = 0;
int LCD_REFRESH = 1;
#define MENU_ROOT_AUTO              0
#define MENU_ROOT_MANUAL            1
#define MENU_MANUAL_LEFT            2
#define MENU_MANUAL_LEFT_FAST         3
#define MENU_MANUAL_RIGHT           4
#define MENU_MANUAL_RIGHT_FAST          5
#define MENU_AUTO_????               6
#define MENU_SET_COMPONENT            7
#define MENU_SET_REPEAT             8
#define MENU_WORKING              9

#define TODO                  255

int menu_default_state = 0;
int menu_manual_state = 3;



void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(SHIFTREG_latchPin, OUTPUT);
  pinMode(SHIFTREG_dataPin, OUTPUT);  
  pinMode(SHIFTREG_clockPin, OUTPUT);
  registerWrite(0);

  pinMode(BUTTON_START, INPUT);
  pinMode(BUTTON_BACK, INPUT);
  digitalWrite(BUTTON_START, HIGH);
  digitalWrite(BUTTON_BACK, HIGH);

  pinMode(ENCODER_PIN_A, INPUT);
  pinMode(ENCODER_PIN_B, INPUT);
  digitalWrite(ENCODER_PIN_A, HIGH);
  digitalWrite(ENCODER_PIN_B, HIGH);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), INT_A, FALLING); 
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), INT_B, FALLING);  

  lcd.begin(16, 2);
  Serial.begin(9600);
  Serial.println("reset");
}

void loop() {
  Serial.println(encoderValue);
  switch (MENU_STATE) {
    case MENU_ROOT_AUTO:{
      if(LCD_REFRESH){
    lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("AUTO      MANUAL");
        lcd.setCursor(0, 1);
        lcd.print("****            ");
        LCD_REFRESH =0;
        encoderValuePrev = encoderValue;
      }
      if( encoderValue > encoderValuePrev){
        MENU_STATE = MENU_ROOT_MANUAL;
        LCD_REFRESH =1;
      }
      if( encoderValue < encoderValuePrev){
        MENU_STATE = MENU_ROOT_AUTO;
        LCD_REFRESH =1;
      }
      if( pressedSartButton() ){
        MENU_STATE = TODO;
        LCD_REFRESH =1;
      }
      if( pressedBackButton() ){
        MENU_STATE = MENU_ROOT_AUTO;
        LCD_REFRESH =1;
      }
      break;
    }
  
    case MENU_ROOT_MANUAL:{
      if(LCD_REFRESH){
    lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("AUTO      MANUAL");
        lcd.setCursor(0, 1);
        lcd.print("          ******");
        LCD_REFRESH =0;
        encoderValuePrev = encoderValue;
      }
      if( encoderValue > encoderValuePrev)
      {
        MENU_STATE = MENU_ROOT_MANUAL;
        LCD_REFRESH =1;
      }
      if( encoderValue < encoderValuePrev)
      {
        MENU_STATE = MENU_ROOT_AUTO;
        LCD_REFRESH =1;
      }
      if( pressedSartButton() ){
        MENU_STATE = MENU_MANUAL_LEFT;
        LCD_REFRESH =1;
      }
      if( pressedBackButton() ){
        MENU_STATE = MENU_ROOT_MANUAL;
        LCD_REFRESH =1;
      }
      break;
    }
    
    case MENU_MANUAL_LEFT:{
      if(LCD_REFRESH){
        lcd.clear();
    lcd.setCursor(0, 0);
        lcd.print("<<<  <<  >>  >>>");
        lcd.setCursor(0, 1);
        lcd.print("     **         ");
        LCD_REFRESH =0;
        encoderValuePrev = encoderValue;
      }
      if( encoderValue > encoderValuePrev)
      {
        MENU_STATE = MENU_MANUAL_RIGHT;
        LCD_REFRESH =1;
      }
      if( encoderValue < encoderValuePrev)
      {
        MENU_STATE = MENU_MANUAL_LEFT_FAST;
        LCD_REFRESH =1;
      }
      if( pressedSartButton() ){
        MENU_STATE = MENU_MANUAL_LEFT;
        LCD_REFRESH =1;
    stepperStripLeft(SPEED_STRIP_MANUAL_SLOW);
      }
      if( pressedBackButton() ){
        MENU_STATE = MENU_ROOT_MANUAL;
        LCD_REFRESH =1;
      }
      break;
    }
  
      case MENU_MANUAL_LEFT_FAST:{
      if(LCD_REFRESH){
        lcd.clear();
    lcd.setCursor(0, 0);
        lcd.print("<<<  <<  >>  >>>");
        lcd.setCursor(0, 1);
        lcd.print("***             ");
        LCD_REFRESH =0;
        encoderValuePrev = encoderValue;
      }
      if( encoderValue > encoderValuePrev)
      {
        MENU_STATE = MENU_MANUAL_LEFT;
        LCD_REFRESH =1;
      }
      if( encoderValue < encoderValuePrev)
      {
        MENU_STATE = MENU_MANUAL_LEFT_FAST;
        LCD_REFRESH =1;
      }
      if( pressedSartButton() ){
        MENU_STATE = MENU_MANUAL_LEFT_FAST;
        LCD_REFRESH =1;
    stepperStripLeft(SPEED_STRIP_MANUAL_FAST);
      }
      if( pressedBackButton() ){
        MENU_STATE = MENU_ROOT_MANUAL;
        LCD_REFRESH =1;
      }
      break;
    }
  
  case MENU_MANUAL_RIGHT:{
      if(LCD_REFRESH){
        lcd.clear();
    lcd.setCursor(0, 0);
        lcd.print("<<<  <<  >>  >>>");
        lcd.setCursor(0, 1);
        lcd.print("         **     ");
        LCD_REFRESH =0;
        encoderValuePrev = encoderValue;
      }
      if( encoderValue > encoderValuePrev)
      {
        MENU_STATE = MENU_MANUAL_RIGHT_FAST;
        LCD_REFRESH =1;
      }
      if( encoderValue < encoderValuePrev)
      {
        MENU_STATE = MENU_MANUAL_LEFT;
        LCD_REFRESH =1;
      }
      if( pressedSartButton() ){
        MENU_STATE = MENU_MANUAL_RIGHT;
        LCD_REFRESH =1;
    stepperStripRight(SPEED_STRIP_MANUAL_SLOW);
      }
      if( pressedBackButton() ){
        MENU_STATE = MENU_ROOT_MANUAL;
        LCD_REFRESH =1;
      }
      break;
    }
  
  case MENU_MANUAL_RIGHT_FAST:{
      if(LCD_REFRESH){
        lcd.clear();
    lcd.setCursor(0, 0);
        lcd.print("<<<  <<  >>  >>>");
        lcd.setCursor(0, 1);
        lcd.print("             ***");
        LCD_REFRESH =0;
        encoderValuePrev = encoderValue;
      }
      if( encoderValue > encoderValuePrev)
      {
        MENU_STATE = MENU_MANUAL_RIGHT_FAST;
        LCD_REFRESH =1;
      }
      if( encoderValue < encoderValuePrev)
      {
        MENU_STATE = MENU_MANUAL_RIGHT;
        LCD_REFRESH =1;
      }
      if( pressedSartButton() ){
        MENU_STATE = MENU_MANUAL_RIGHT_FAST;
        LCD_REFRESH =1;
    stepperStripRight(SPEED_STRIP_MANUAL_FAST);
      }
      if( pressedBackButton() ){
        MENU_STATE = MENU_ROOT_MANUAL;
        LCD_REFRESH =1;
      }
      break;
    }
    
    
    
    
    case TODO:{
      if(LCD_REFRESH){
        lcd.clear();
    lcd.setCursor(0, 0);
        lcd.print("TODO");
        lcd.setCursor(0, 1);
        lcd.print("TODO");
        LCD_REFRESH =0;
        encoderValuePrev = encoderValue;
      }
      if( encoderValue > encoderValuePrev)
      {
        MENU_STATE = TODO;
        LCD_REFRESH =1;
      }
      if( encoderValue < encoderValuePrev)
      {
        MENU_STATE = TODO;
        LCD_REFRESH =1;
      }
      if( pressedSartButton() ){
        MENU_STATE = TODO;
        LCD_REFRESH =1;
      }
      if( pressedBackButton() ){
        MENU_STATE = MENU_ROOT_AUTO;
        LCD_REFRESH =1;
      }
      break;
    }
      

    default:
      Serial.println("bright");
      break;
  }

 
 
}

void INT_A() {
    encoderValue++;
  delay(1);
}
void INT_B() {
  encoderValue--;
  delay(1);
}


int pressedSartButton(){
  int value = !digitalRead(BUTTON_START);
  if(value)
    delay(200);
  return value;  
}

int pressedBackButton(){
  int value = !digitalRead(BUTTON_BACK);
  if(value)
    delay(200);
  return value;
}

int getCountedComponents(){
  return 0;
}

int readSensorComponentHoles(){
  return 0;
}

int readSensorComponentInPosition(){
  return 0;
}

int readSensorCutterEndPosition(){
  return 0;
}

void stepperStripLeft(int speed){
  while( !digitalRead(BUTTON_START) ){
  registerWrite(STEPPER_STRIP_STATE);
  delay(speed);
  registerWrite(0);
  STEPPER_STRIP_STATE*=2;
  if(STEPPER_STRIP_STATE >8) STEPPER_STRIP_STATE = 1;
  }
}

void stepperStripRight(int speed){
  while( !digitalRead(BUTTON_START) ){
  registerWrite(STEPPER_STRIP_STATE);
  delay(speed);
  registerWrite(0);
  STEPPER_STRIP_STATE/=2;
  if(STEPPER_STRIP_STATE < 1) STEPPER_STRIP_STATE = 8;
  }
}

// This method sends bits to the shift register:
void registerWrite(int data) {
  digitalWrite(SHIFTREG_latchPin, LOW);
  shiftOut(SHIFTREG_dataPin, SHIFTREG_clockPin, MSBFIRST, data);
  digitalWrite(SHIFTREG_latchPin, HIGH);
}


