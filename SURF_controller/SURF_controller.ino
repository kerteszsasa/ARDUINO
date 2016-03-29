#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <Servo.h>

#define SERVO_PIN                     6
#define RED                           10
#define GREEN                         9
#define BLUE                          5
#define MODE_SELECT_PIN               A6
#define BATTERY_PIN                   A5
#define GAS_PIN                       A2
#define RANDOM_PIN                    A0

int MODE_RECEIEVER = 0;
int MODE_TRANSMITTER = 0;

char battery_response[5];

int SURF_speed = 0;

int cycle_divider = 0;





#define BUFFER_SIZE 20
char nrf_buffer[BUFFER_SIZE];
char* BASE_ADDRESS ="00001";
char* TRANSMIT_ADDRESS ="00001";  
char* RECEIVE_ADDRESS ="TEST1"; 


//Servo myservo;  // create servo object to control a servo



// the setup routine runs once when you press reset:
void setup() {                
  //RGB init
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);
  digitalWrite(BLUE, HIGH);

  Serial.begin(115200);
  // myservo.attach(SERVO_PIN);  // attaches the servo on pin 5 to the servo object
  RF_init();
  //Determine whitch panel is it: receiver or transmitter
  panel_identify();
  Serial.println("Listening...");
  delay(1000);
  RF_send(BASE_ADDRESS, "HELLO");
}

// the loop routine runs over and over again forever:
void loop() {
  if( RF_incoming() ){
    RF_handling();
  }

  cycle_divider++;
  if(cycle_divider == 500){
    cycle_divider = 0;
    if(MODE_TRANSMITTER){
      Transmitter_function();
    }
    if(MODE_RECEIEVER){
      Receiever_function();
    }
  }
  delay(1);  
}














void Transmitter_function(){

    RF_sendString(BASE_ADDRESS, read_gas() );
}

void Receiever_function(){
  
}
void RF_handling(){
    String str(nrf_buffer);
    if( str.indexOf("GETBATTERY")==0 ){
      read_battery();
      RF_send(BASE_ADDRESS, battery_response);
    }
    if( str.indexOf("NEWADDRESS")==0 ){
       if(MODE_TRANSMITTER){
        RF_send(BASE_ADDRESS, "NEW:xxxxx TODO");
      }
      else{
        RF_send(BASE_ADDRESS, "NOT VALID COMMAND");
      }
    }
    if( str.indexOf("SETSPEED:")==0 ){
      if(MODE_RECEIEVER){
        String sp = str.substring(9);
        SURF_speed = sp.toInt();
        analogWrite(RED, 1024 - SURF_speed*10);
      }
      else{
        RF_send(BASE_ADDRESS, "NOT VALID COMMAND");
      }
    }
    if( str.indexOf("PAIRTO:")==0 ){
      
      if(MODE_RECEIEVER){
        RF_send(BASE_ADDRESS, "TODO");
      }
      else{
        RF_send(BASE_ADDRESS, "NOT VALID COMMAND");
      }
    }
    if( str.indexOf("GETGAS")==0 ){
      debug( read_gas() );
    }
    if( str.indexOf("WHO?")==0 ){
      MODE_TRANSMITTER ? debug("TRANSMITTER") : debug("RECEIEVER");
      debug("MY ADDRESS: ?????");
    }
    if( str.indexOf("R1")==0 ){
      digitalWrite(RED, LOW);
    }
    if( str.indexOf("R0")==0 ){
      digitalWrite(RED, HIGH);
    }
     if( str.indexOf("G1")==0 ){
      digitalWrite(GREEN, LOW);
    }
    if( str.indexOf("G0")==0 ){
      digitalWrite(GREEN, HIGH);
    }
     if( str.indexOf("B1")==0 ){
      digitalWrite(BLUE, LOW);
    }
    if( str.indexOf("B0")==0 ){
      digitalWrite(BLUE, HIGH);
    }
    if( str.indexOf("TEST")==0 ){
      debug("this is a message");
      RF_sendString(BASE_ADDRESS,"this is a string");
    }
    //Finally reset the buffer
    memset(nrf_buffer, 0, sizeof(nrf_buffer));
}



void RF_init(void){
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  // Set own address - unique 5 character string
  Mirf.setRADDR((byte *)RECEIVE_ADDRESS);
  Mirf.payload = BUFFER_SIZE;
  Mirf.channel = 90;
  Mirf.config(); 
}

void RF_send(char* address, char* data){
    Mirf.setTADDR((byte *) address);
    Mirf.send((byte *) data);
    while(Mirf.isSending()) delay(1);  
}
void RF_sendString(char* address, String str){

   if(str.length() > BUFFER_SIZE-1) str = "TOO LONG STRING";
  char TMP_buffer[BUFFER_SIZE];
  str.toCharArray(TMP_buffer, BUFFER_SIZE);

    Mirf.setTADDR((byte *) address);
    Mirf.send((byte *) TMP_buffer);
    while(Mirf.isSending()) delay(1);  
}

int RF_incoming(void){
  if(Mirf.dataReady()) {
    Mirf.getData((byte *) nrf_buffer);
    return 1;
  }else{
    return 0;
  }
  
}



void panel_identify(){
    long rx_or_tx =0;
    analogRead(A6);
    delay(500);
    rx_or_tx += analogRead(A6);
    rx_or_tx += analogRead(A6);
    rx_or_tx += analogRead(A6);
    rx_or_tx += analogRead(A6);
    if(rx_or_tx > 2048){
      MODE_TRANSMITTER = 1;
    }else{
      MODE_RECEIEVER = 1;
    }
}

void read_battery(){
 analogRead(BATTERY_PIN);
 delay(1);
 analogRead(BATTERY_PIN);
 float batt = analogRead(BATTERY_PIN);
 batt += analogRead(BATTERY_PIN);
 batt += analogRead(BATTERY_PIN);
 batt += analogRead(BATTERY_PIN);
 batt /=4;
 if(MODE_TRANSMITTER){
  batt*=66;
  batt/=10240;
 }
  if(MODE_RECEIEVER){
  batt*=66;
  batt/=1024;
 }
String str(batt);
 battery_response[0] = str[0];
 battery_response[1] = str[1];
 battery_response[2] = str[2];
 battery_response[3] = str[3];
 battery_response[4] = 'V';
}

String read_gas(){
   analogRead(GAS_PIN);
   int value = analogRead(GAS_PIN);
   value += analogRead(GAS_PIN);
   value += analogRead(GAS_PIN);
   value += analogRead(GAS_PIN);
   value /=4;
   // make it between 0 and 100
   //TODO
   String str(value);
   if(MODE_TRANSMITTER) return str;
   if (MODE_RECEIEVER) return "NOT A TRANSMITTER";
   
}

void read_EEprom_address(){
  
}
void write_EEprom_address(){
  
}
void generate_address(){
  
}
void set_speed(char sp){
    analogWrite(SERVO_PIN, sp);
    //int val = map(sp, 0, 255, 45, 165);     // scale it to use it with the servo (value between 0 and 180)
    //myservo.write(val); 
}

void debug(int num){
  
}

void debug(String str){
 /* if(str.length() > 19) str = "TOO LONG STRING";
  char TMP_buffer[BUFFER_SIZE];
  str.toCharArray(TMP_buffer, BUFFER_SIZE);
  RF_send(BASE_ADDRESS, TMP_buffer);*/
  RF_sendString(BASE_ADDRESS,str);
}

