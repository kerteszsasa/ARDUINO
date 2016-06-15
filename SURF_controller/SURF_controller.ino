#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <Servo.h>
#include <EEPROM.h>

void(* resetFunc) (void) = 0;//declare reset function at address 0

#define SERVO_PIN                     6
#define RED                           10
#define GREEN                         9
#define BLUE                          5
#define MODE_SELECT_PIN               A6
#define BATTERY_PIN                   A5
#define GAS_PIN                       A2
#define RANDOM_PIN                    A0
#define PAIR_1                        A0
#define PAIR_2                        A1

int MODE_RECEIVER = 0;
int MODE_TRANSMITTER = 0;

int SURF_speed = 0;
int Message_TTL = 0;
int cycle_divider = 0;

#define BUFFER_SIZE 20
char nrf_buffer[BUFFER_SIZE];
String BASE_ADDRESS = "00001";
String TRANSMITTER_ADDRESS = ".....";
String RECEIVER_ADDRESS = "-----";
String WAT_FOR_ADDRESS = "?????";

Servo myservo;  // create servo object to control a servo



// the setup routine runs once when you press reset:
void setup() {
  //RGB init
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);
  digitalWrite(BLUE, HIGH);
  panel_identify();         //Determine whitch panel is it: receiver or transmitter
  
  if( MODE_TRANSMITTER ) Init_transmitter();
  if( MODE_RECEIVER )    Init_receiver();

  
  
   myservo.attach(SERVO_PIN);  // attaches the servo on pin 5 to the servo object

  //Read_From_EEPROM();
  RF_init();
  delay(1000);
  String startup = "Conn: ";
  if( MODE_TRANSMITTER ){
    startup += "TRANS ";
    startup += TRANSMITTER_ADDRESS;
  }
  if( MODE_RECEIVER ){
    startup += "REC ";
    startup += RECEIVER_ADDRESS;
  }
  debug(startup);
  if( MODE_TRANSMITTER ) send_Pair_Address(RECEIVER_ADDRESS);

  Serial.begin(115200);
  Serial.println("Listening...");
}

// the loop routine runs over and over again forever:
void loop() {
  if ( RF_incoming() ) {
    RF_handling();
  }
  cycle_divider++;
  if (cycle_divider == 50) {
    cycle_divider = 0;
    if (MODE_TRANSMITTER) {
      Transmitter_function();
    }
    if (MODE_RECEIVER) {
      Receiever_function();
    }
  }
  delay(1);
}

void Transmitter_function() {

  // RF_sendString(BASE_ADDRESS, read_gas() );
  // Read_From_EEPROM();

 // resetFunc(); //call reset
 String str = "SETSPEED:";
 str += read_gas(); 
 //debug(str);
 //RF_sendString(RECEIVER_ADDRESS, str);
 //debug(RECEIVER_ADDRESS);
 //debug(TRANSMITTER_ADDRESS);
  //debug(BASE_ADDRESS);
 RF_sendString(RECEIVER_ADDRESS, str);
 debug(str);
 RF_sendString(TRANSMITTER_ADDRESS, str);
 digitalWrite(GREEN, LOW); // bekapcsolás jelző
 //analogWrite(GREEN, 250);
  
  
}

void Receiever_function() {
  //resetFunc(); //call reset 
  //debug("REC: running");

  if(Message_TTL > 0) Message_TTL--;
  if(Message_TTL == 0) SURF_speed = 0;
  
  int val = map(SURF_speed, 0, 1024, 45, 165);     // scale it to use it with the servo (value between 0 and 180)
  myservo.write(val);
  digitalWrite(GREEN, LOW); // bekapcsolás jelző
  //analogWrite(GREEN, 1020);

  
 /* digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);
  digitalWrite(BLUE, HIGH);
  if(SURF_speed > 75) digitalWrite(RED, LOW);
  if(SURF_speed > 25 && SURF_speed < 75){
    digitalWrite(GREEN, LOW);
    digitalWrite(RED, LOW);
  }
   if(SURF_speed < 25 ){
    digitalWrite(GREEN, LOW);
  }*/
}

void RF_handling() {
  String str(nrf_buffer);
  if ( str.indexOf("GETBATTERY") == 0 ) {
    read_battery();
  }
  if ( str.indexOf("NEWADDRESS") == 0 ) {
    if (MODE_TRANSMITTER) {
      debug("NEW:xxxxx TODO");
      String addr = "";
      addr = Generate_address();
      Write_To_EEPROM(addr); 
      String response = "NEW: ";
      response += addr;
      debug(response);
      digitalWrite(RED, LOW);
      digitalWrite(GREEN, LOW);
      digitalWrite(BLUE, LOW);
      delay(1000);
      resetFunc(); //call reset 
    }
    else {
      debug("NOT VALID COMMAND");
    }
  }
  if ( str.indexOf("SETSPEED:") == 0 ) {
    if (MODE_RECEIVER) {
      String sp = str.substring(9);
      SURF_speed = sp.toInt();
      Message_TTL = 50;
      digitalWrite(BLUE, LOW);
      delay(2);
      digitalWrite(BLUE, HIGH);
      //debug(SURF_speed);
    }
    else {
      debug("NOT VALID COMMAND");
    }
  }
  if ( str.indexOf("PAIRTO:") == 0 ) {
    if (MODE_RECEIVER) {
      String ad = str.substring(7);
      Write_To_EEPROM(ad);
      delay(1000);
      resetFunc(); //call reset 
    }
    else {
      debug("NOT VALID COMMAND");
    }
  }
  if ( str.indexOf("GETGAS") == 0 ) {
    debug( read_gas() );
  }
  if ( str.indexOf("WHO?") == 0 ) {
    MODE_TRANSMITTER ? debug("TRANSMITTER: "+TRANSMITTER_ADDRESS) : debug("RECEIEVER: "+ RECEIVER_ADDRESS);
  }
  if ( str.indexOf("R1") == 0 ) {
    digitalWrite(RED, LOW);
  }
  if ( str.indexOf("R0") == 0 ) {
    digitalWrite(RED, HIGH);
  }
  if ( str.indexOf("G1") == 0 ) {
    digitalWrite(GREEN, LOW);
  }
  if ( str.indexOf("G0") == 0 ) {
    digitalWrite(GREEN, HIGH);
  }
  if ( str.indexOf("B1") == 0 ) {
    digitalWrite(BLUE, LOW);
  }
  if ( str.indexOf("B0") == 0 ) {
    digitalWrite(BLUE, HIGH);
  }
  //Finally reset the buffer
  memset(nrf_buffer, 0, sizeof(nrf_buffer));
}

void Init_transmitter(){
  String addr = "";
  addr = Read_From_EEPROM();
  if( addr[0] == -1){           // Generate new address if ther is no one
    addr = Generate_address();
    Write_To_EEPROM(addr);
    digitalWrite(BLUE, LOW);
    delay(1000);
    digitalWrite(BLUE, HIGH);
  }
  TRANSMITTER_ADDRESS = addr.substring(0);  // Set this address to transmitter's address
  String  rec_addr = addr.substring(0);
  rec_addr.toLowerCase();
  RECEIVER_ADDRESS = rec_addr;              // Set the address's lower case version to receiver's address (the tarnsmitter send data to it)
}

void send_Pair_Address(String str){
  String message = "PAIRTO:";
  message += str;
  for(int i= 0; i<10; i++){
    RF_sendString("?????", message);
    delay(100);
  }
}

void Init_receiver(){
  String addr = "";
  addr = Read_From_EEPROM();    // Read address form EEPROM
  if(addr[0] == -1){        //If no address change to ????
  addr = "?????";         //And save it
  Write_To_EEPROM(addr);
  }
  RECEIVER_ADDRESS[0] = addr[0];
  RECEIVER_ADDRESS[1] = addr[1];
  RECEIVER_ADDRESS[2] = addr[2];
  RECEIVER_ADDRESS[3] = addr[3];
  RECEIVER_ADDRESS[4] = addr[4];  //Set the address for the RF address
  if(addr[0] == '?'){       // If it is ?????
  digitalWrite(RED, LOW);
  digitalWrite(BLUE, LOW);    //Set LED PINK and return
    return; 
  }
  pinMode(PAIR_1, OUTPUT);
  digitalWrite(PAIR_1, LOW);
  pinMode(PAIR_2, INPUT);
  digitalWrite(PAIR_2, HIGH);
  int pair = digitalRead(PAIR_2);
  if(pair == 0){          //If someone pressed the pair button
    digitalWrite(RED, LOW);
    addr = "?????";
  Write_To_EEPROM(addr);      //Save ????? to EEPROM
  while(digitalRead(PAIR_2) == 0); // Wait for release button
  digitalWrite(RED, LOW);
  digitalWrite(BLUE, LOW);    //Set LED PINK and return
  }
  RECEIVER_ADDRESS[0] = addr[0];
  RECEIVER_ADDRESS[1] = addr[1];
  RECEIVER_ADDRESS[2] = addr[2];
  RECEIVER_ADDRESS[3] = addr[3];
  RECEIVER_ADDRESS[4] = addr[4];  //Set the address for the RF address                 

}

String Read_From_EEPROM() {
  String str = "-----";
  str[0] = EEPROM.read(0);
  str[1] = EEPROM.read(1);
  str[2] = EEPROM.read(2);
  str[3] = EEPROM.read(3);
  str[4] = EEPROM.read(4);
  return str;
}

void Write_To_EEPROM(String str){
  EEPROM.write(0, str[0] );
  EEPROM.write(1, str[1] );
  EEPROM.write(2, str[2] );
  EEPROM.write(3, str[3] );
  EEPROM.write(4, str[4] );
}

String Generate_address() {
  int rnd;
  char c;
  String str = "-----";
   rnd = analogRead(RANDOM_PIN);
  randomSeed( analogRead(RANDOM_PIN) );
  delayMicroseconds( analogRead(RANDOM_PIN) );
  str[0] = random(65, 91);
  delayMicroseconds( analogRead(RANDOM_PIN) );
  str[1] = random(65, 91);
  delayMicroseconds( analogRead(RANDOM_PIN) );
  str[2] = random(65, 91);
  delayMicroseconds( analogRead(RANDOM_PIN) );
  str[3] = random(65, 91);
  delayMicroseconds( analogRead(RANDOM_PIN) );
  str[4] = random(65, 91);
  return str;
}

void RF_init(void) {
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  char* R_addr = "7777777";
  // Set own address - unique 5 character string
 // if( MODE_TRANSMITTER ) TRANSMITTER_ADDRESS.getBytes(buffer, 5);
 // if( MODE_RECEIVER )    RECEIVER_ADDRESS.getBytes(buffer, 5);
  if( MODE_TRANSMITTER ){
   R_addr[0] = TRANSMITTER_ADDRESS [0];
   R_addr[1] = TRANSMITTER_ADDRESS [1];
   R_addr[2] = TRANSMITTER_ADDRESS [2];
   R_addr[3] = TRANSMITTER_ADDRESS [3];
   R_addr[4] = TRANSMITTER_ADDRESS [4];
  }
  if( MODE_RECEIVER ){
   R_addr[0] = RECEIVER_ADDRESS [0];
   R_addr[1] = RECEIVER_ADDRESS [1];
   R_addr[2] = RECEIVER_ADDRESS [2];
   R_addr[3] = RECEIVER_ADDRESS [3];
   R_addr[4] = RECEIVER_ADDRESS [4];
  }
 
  Mirf.setRADDR( (byte *)R_addr );
  Mirf.payload = BUFFER_SIZE;
  Mirf.channel = 90;
  Mirf.config();
}


void RF_sendString(String address, String str) {
 char* T_addr = "55555";
 T_addr[0] = address [0];
 T_addr[1] = address [1];
 T_addr[2] = address [2];
 T_addr[3] = address [3];
 T_addr[4] = address [4];
  if (str.length() > BUFFER_SIZE - 1) str = "TOO LONG STRING";
  char TMP_buffer[BUFFER_SIZE];
  str.toCharArray(TMP_buffer, BUFFER_SIZE);
  Mirf.setTADDR((byte *) T_addr);
  Mirf.send((byte *) TMP_buffer);
  while (Mirf.isSending()) delay(1);
}

int RF_incoming(void) {
  if (Mirf.dataReady()) {
    Mirf.getData((byte *) nrf_buffer);
    return 1;
  } else {
    return 0;
  }

}

void panel_identify() {
  long rx_or_tx = 0;
  analogRead(A6);
  delay(500);
  rx_or_tx += analogRead(A6);
  rx_or_tx += analogRead(A6);
  rx_or_tx += analogRead(A6);
  rx_or_tx += analogRead(A6);
  if (rx_or_tx > 2048) {
    MODE_TRANSMITTER = 1;
  } else {
    MODE_RECEIVER = 1;
  }
}

void read_battery() {
  analogRead(BATTERY_PIN);
  delay(1);
  analogRead(BATTERY_PIN);
  float batt = analogRead(BATTERY_PIN);
  batt += analogRead(BATTERY_PIN);
  batt += analogRead(BATTERY_PIN);
  batt += analogRead(BATTERY_PIN);
  batt /= 4;
  if (MODE_TRANSMITTER) {
    batt *= 66;
    batt /= 10240;
  }
  if (MODE_RECEIVER) {
    batt *= 66;
    batt /= 1024;
  }
  String str(batt);
  str += 'V';
  debug(str);
}

String read_gas() {
  analogRead(GAS_PIN);
  int value = analogRead(GAS_PIN);
  value += analogRead(GAS_PIN);
  value += analogRead(GAS_PIN);
  value += analogRead(GAS_PIN);
  value /= 4;
  // make it between 0 and 1000
  //TODO
  //hack for hall sensor lineariaation
  value = value - 540;
  value = value *2;
  //hack end
  String str(value);
  if (MODE_TRANSMITTER) return str;
  if (MODE_RECEIVER) return "NOT A TRANSMITTER";

}

void set_speed(char sp) {
  analogWrite(SERVO_PIN, sp);
  //int val = map(sp, 0, 255, 45, 165);     // scale it to use it with the servo (value between 0 and 180)
  //myservo.write(val);
}

void debug(int num) {
  String str(num);
  debug(str);
}

void debug(String str) {
  /* if(str.length() > 19) str = "TOO LONG STRING";
    char TMP_buffer[BUFFER_SIZE];
    str.toCharArray(TMP_buffer, BUFFER_SIZE);
    RF_send(BASE_ADDRESS, TMP_buffer);*/
  RF_sendString(BASE_ADDRESS, str);
}

