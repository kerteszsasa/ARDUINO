
//******DEFINE HW PINS******//
// STEPPER MOTOR PINS: connected to a shift register
#define SHIFTREG_dataPin          10        //Pin connected to Data  pin (DS)    of 74HC595
#define SHIFTREG_latchPin         9        //Pin connected to latch pin (ST_CP) of 74HC595
#define SHIFTREG_clockPin         A0       //Pin connected to clock pin (SH_CP) of 74HC595
// STEPPER SPEEDS
#define SPEED_STRIP_MANUAL_SLOW   100
#define SPEED_STRIP_MANUAL_FAST   50
#define SPPED_STRIP_AUTO          300
#define SPEED_CUTTER              200
#define CUTTER_STEPS              8
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
#define SENSOR_COMPONENT_HOLES    ?
#define SENSOR_COMPONENT_VALID    ?
#define CUTTER_END_POSITION       ?
// BUTTON PINS
#define BUTTON_START              A4
#define BUTTON_BACK               A5

#include <LiquidCrystal.h>
#include <Encoder.h>
LiquidCrystal lcd(LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);
//Encoder encoder(2, 3);

long encoderValue = 0;
long encoderValuePrev = 0;

long component_to_cut = 0;
long component_repeat_times = 0;
long component_rutted = 0;
long component_repeated = 0;

long actual_cut = 0;
long actual_repeat = 0;

int STEPPER_STRIP_STATE = 1;
int STEPPER_BLDE_STATE = 1;

int component_hole_prev_value = 0;


//******STATES******//
int MENU_STATE = 0;
int LCD_REFRESH = 1;
#define MENU_ROOT_AUTO             0
#define MENU_ROOT_MANUAL           1
#define MENU_MANUAL_LEFT           2
#define MENU_MANUAL_LEFT_FAST      3
#define MENU_MANUAL_RIGHT          4
#define MENU_MANUAL_RIGHT_FAST     5
#define MENU_SET_COMPONENT         6
#define MENU_SET_REPEAT            7
#define MENU_WORKING               8
#define MENU_FINISHED              9
#define MENU_PAUSE                 10
#define MENU_EMPTY_COMPONENT     11

#define TODO                       255

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
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), INT_A, RISING); 
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), INT_B, RISING);  

  lcd.begin(16, 2);
  Serial.begin(9600);
  Serial.println("reset");
}

void loop() {
  Serial.println(MENU_STATE);
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
        MENU_STATE = MENU_SET_COMPONENT;
        LCD_REFRESH =1;
        encoderValuePrev = encoderValue = 1;
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
        while( !digitalRead(BUTTON_START) ){
          stepperStripLeft(SPEED_STRIP_MANUAL_SLOW);
        }
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
        while( !digitalRead(BUTTON_START) ){
          stepperStripLeft(SPEED_STRIP_MANUAL_FAST);
        }
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
        while( !digitalRead(BUTTON_START) ){
          stepperStripRight(SPEED_STRIP_MANUAL_SLOW);
        }
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
        while( !digitalRead(BUTTON_START) ){
          stepperStripRight(SPEED_STRIP_MANUAL_FAST);
        }
      }
      if( pressedBackButton() ){
        MENU_STATE = MENU_ROOT_MANUAL;
        LCD_REFRESH =1;
      }
      break;
    }
  
  case MENU_SET_COMPONENT:{
      if(LCD_REFRESH){
        if(encoderValue < 0) encoderValue = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("COMPONET NUMBER:");
        lcd.setCursor(0, 1);
        lcd.print(encoderValue);
        LCD_REFRESH =0;
        encoderValuePrev = encoderValue;
      }
      if( encoderValue > encoderValuePrev)
      {
        MENU_STATE = MENU_SET_COMPONENT;
        LCD_REFRESH =1;
      }
      if( encoderValue < encoderValuePrev)
      {
        MENU_STATE = MENU_SET_COMPONENT;
        LCD_REFRESH =1;
      }
      if( pressedSartButton() ){
        MENU_STATE = MENU_SET_REPEAT;
        LCD_REFRESH =1;
        component_to_cut = encoderValue;
        encoderValuePrev = encoderValue = 1;
    }
      if( pressedBackButton() ){
        MENU_STATE = MENU_ROOT_AUTO;
        LCD_REFRESH =1;
      }
      break;
    }
  
  case MENU_SET_REPEAT:{
      if(LCD_REFRESH){
        if(encoderValue < 0) encoderValue = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("REPEAT TIMES:");
        lcd.setCursor(0, 1);
        lcd.print(encoderValue);
        LCD_REFRESH =0;
        encoderValuePrev = encoderValue;
      }
      if( encoderValue > encoderValuePrev)
      {
        MENU_STATE = MENU_SET_REPEAT;
        LCD_REFRESH =1;
      }
      if( encoderValue < encoderValuePrev)
      {
        MENU_STATE = MENU_SET_REPEAT;
        LCD_REFRESH =1;
      }
      if( pressedSartButton() ){
        MENU_STATE = MENU_WORKING;
        LCD_REFRESH =1;
        component_repeat_times = encoderValue;
        encoderValuePrev = encoderValue = 0;
        actual_cut = actual_repeat = 0;
      }
      if( pressedBackButton() ){
        MENU_STATE = MENU_ROOT_AUTO;
        LCD_REFRESH =1;
      }
      break;
    }
    
    case MENU_WORKING:{
      if(LCD_REFRESH){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("SET: ");
        lcd.print(component_to_cut);
        lcd.print(" ");
        lcd.print(component_repeat_times);
        lcd.setCursor(0, 1);
        lcd.print("NOW: ");
        lcd.print(actual_cut);
        lcd.print(" ");
        lcd.print(actual_repeat);
        LCD_REFRESH =0;
        encoderValuePrev = encoderValue;
      }
      if( pressedSartButton() ){
        MENU_STATE = MENU_PAUSE;
        LCD_REFRESH =1;
      }
      if( pressedBackButton() ){
        MENU_STATE = MENU_PAUSE;
        LCD_REFRESH =1;
      }
      component_move_count_cut();
      break;
    }
  
  case MENU_PAUSE:{
      if(LCD_REFRESH){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("PAUSED");
        lcd.setCursor(0, 1);
        lcd.print("NOW: ");
        lcd.print(actual_cut);
        lcd.print(" ");
        lcd.print(actual_repeat);
        LCD_REFRESH =0;
        encoderValuePrev = encoderValue;
      }
      if( pressedSartButton() ){
        MENU_STATE = MENU_WORKING;
        LCD_REFRESH =1;
      }
      if( pressedBackButton() ){
        MENU_STATE = MENU_ROOT_AUTO;
        LCD_REFRESH =1;
      }
      break;
    }
  
  case MENU_FINISHED:{
      if(LCD_REFRESH){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("FINISHED");
        lcd.setCursor(0, 1);
        lcd.print("NOW: ");
        lcd.print(actual_cut);
        lcd.print(" ");
        lcd.print(actual_repeat);
        LCD_REFRESH =0;
        encoderValuePrev = encoderValue;
      }
      if( pressedSartButton() ){
        MENU_STATE = MENU_ROOT_AUTO;
        LCD_REFRESH =1;
      }
      if( pressedBackButton() ){
        MENU_STATE = MENU_ROOT_AUTO;
        LCD_REFRESH =1;
      }
      break;
    }
  
  case MENU_EMPTY_COMPONENT:{
      if(LCD_REFRESH){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("EMPTY COMPONENT");
        lcd.setCursor(0, 1);
        lcd.print("NOW: ");
        lcd.print(actual_cut);
        lcd.print(" ");
        lcd.print(actual_repeat);
        LCD_REFRESH =0;
        encoderValuePrev = encoderValue;
      }
      if( pressedSartButton() ){
        MENU_STATE = MENU_ROOT_AUTO;
        LCD_REFRESH =1;
      }
      if( pressedBackButton() ){
        MENU_STATE = MENU_ROOT_AUTO;
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
      break;
  }

 
 
}

void INT_A() {
    if(digitalRead(ENCODER_PIN_B))
    encoderValue++;
  else
    encoderValue--;
  delay(1);
}
void INT_B() {
}

void component_move_count_cut(){
  if(component_to_cut == 0 && component_repeat_times == 0){
        MENU_STATE = MENU_FINISHED;
        LCD_REFRESH =1;
        return;
  }
  if( readSensorComponentInPosition() ){
    MENU_STATE = MENU_EMPTY_COMPONENT;
        LCD_REFRESH =1;
    return;
  }else{
    getCountedComponents();
    stepperStripLeft(SPPED_STRIP_AUTO);
    if(actual_cut == component_to_cut){
      cutter_cut();
      actual_cut = 0;
      actual_repeat ++;
    }
    if(actual_repeat == component_repeat_times){
        MENU_STATE = MENU_FINISHED;
        LCD_REFRESH =1;
    }
  }
 
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

void getCountedComponents(){
 /* if( component_hole_prev_value==0 && digitalRead(SENSOR_COMPONENT_HOLES)==1){ TODO
    actual_cut ++;
    LCD_REFRESH =1;
  }
  component_hole_prev_value = digitalRead(SENSOR_COMPONENT_HOLES); */
}

int readSensorComponentInPosition(){
  return 0; //digitalRead(MENU_NO_MORE_COMPONENT); TODO
}

int readSensorCutterEndPosition(){
  return 0;
}

void cutter_cut(){
  for(int i=0; i < CUTTER_STEPS; i++){
    stepperCutterLeft(SPEED_CUTTER);
  }
  while( readSensorCutterEndPosition() ){
    stepperCutterRight(SPEED_CUTTER);
  }
}

void stepperStripLeft(int speed){
  registerWrite(STEPPER_STRIP_STATE);
  delay(speed);
  registerWrite(0);
  STEPPER_STRIP_STATE*=2;
  if(STEPPER_STRIP_STATE >8) STEPPER_STRIP_STATE = 1;
}

void stepperStripRight(int speed){
  registerWrite(STEPPER_STRIP_STATE);
  delay(speed);
  registerWrite(0);
  STEPPER_STRIP_STATE/=2;
  if(STEPPER_STRIP_STATE < 1) STEPPER_STRIP_STATE = 8;
}

void stepperCutterLeft(int speed){
  registerWrite(STEPPER_BLDE_STATE<<4);
  delay(speed);
  registerWrite(0);
  STEPPER_BLDE_STATE*=2;
  if(STEPPER_BLDE_STATE >8) STEPPER_BLDE_STATE = 1;
}

void stepperCutterRight(int speed){
  registerWrite(STEPPER_BLDE_STATE<<4);
  delay(speed);
  registerWrite(0);
  STEPPER_BLDE_STATE/=2;
  if(STEPPER_BLDE_STATE < 1) STEPPER_BLDE_STATE = 8;
}

// This method sends bits to the shift register:
void registerWrite(int data) {
  digitalWrite(SHIFTREG_latchPin, LOW);
  shiftOut(SHIFTREG_dataPin, SHIFTREG_clockPin, MSBFIRST, data);
  digitalWrite(SHIFTREG_latchPin, HIGH);
}









