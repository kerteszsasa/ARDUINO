#include  <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
int msg[1];
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
int SW1 = 7;

void setup(void){
 Serial.begin(9600);
 // attachInterrupt(0, messageReceived, FALLING);  // look for rising edges on digital pin 2
 radio.begin();
 radio.openWritingPipe(pipe);
 Serial.println("Program started: TRANSMITTER");
 }



/*void messageReceived(void){
  Serial.println("INTERRUPT");
  
   if (radio.available()){
   bool done = false;    
   while (!done){
     done = radio.read(msg, 1);      
     Serial.println(msg[0]);
     analogWrite(5, msg[0]);
   }
 } 
  
}*/



void loop(void){
  delay(10);
/* if (digitalRead(SW1) == HIGH){
    msg[0] = 111;
    radio.write(msg, 1);
  }
*/
    Serial.println("SEND");
    msg[0] = analogRead(2)/4;
    radio.write(msg, 1);

    
  
  /*  delay(100);
    msg[0] = analogRead(2) / 2;
    radio.write(msg, 1);
 */
}
