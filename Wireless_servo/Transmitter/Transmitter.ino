#include  <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
int msg[1];
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
int SW1 = 7;

void setup(void){
 Serial.begin(9600);
 radio.begin();
 radio.openWritingPipe(pipe);}

void loop(void){
  delay(1);
/* if (digitalRead(SW1) == HIGH){
    msg[0] = 111;
    radio.write(msg, 1);
  }
*/
   if (analogRead(2) < 2000){
    msg[0] = analogRead(2)/4;
    radio.write(msg, 1);
  }
  /*  delay(100);
    msg[0] = analogRead(2) / 2;
    radio.write(msg, 1);
 */
}
