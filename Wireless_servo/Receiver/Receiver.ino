#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Servo.h>
int msg[1];
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
int LED1 = 3;
Servo myservo;  // create servo object to control a servo
int val;    // variable to read the value from the analog pin

void setup(void){
 Serial.begin(9600);
 myservo.attach(6);  // attaches the servo on pin 9 to the servo object
 radio.begin();
 radio.openReadingPipe(1,pipe);
 radio.startListening();
 pinMode(LED1, OUTPUT);}

void loop(void){
 if (radio.available()){
   bool done = false;    
   while (!done){
     done = radio.read(msg, 1);      
     Serial.println(msg[0]);
     //val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
     val = map(msg[0], 0, 255, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
     myservo.write(val); 
     
     if (msg[0] == 111){
       // delay(10);
        digitalWrite(LED1, HIGH);
     }
     else {
        digitalWrite(LED1, LOW);
     }
       // delay(50);
   }
 }   
 else{
    Serial.println("No radio available");
     radio.begin();
 radio.openReadingPipe(1,pipe);
 radio.startListening();
 delay(50);
 }
}
