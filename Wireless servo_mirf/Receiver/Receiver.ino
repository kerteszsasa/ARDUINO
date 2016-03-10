#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <Servo.h>

int value;
int LED1 = 3;
Servo myservo;  // create servo object to control a servo
int val;    // variable to read the value from the analog pin


void setup()
{
  Serial.begin(9600);
   myservo.attach(5);  // attaches the servo on pin 5 to the servo object
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  // Set own address - unique 5 character string
  Mirf.setRADDR((byte *)"FGHIJ");
  Mirf.payload = sizeof(value);
  Mirf.channel = 90;
  Mirf.config(); 
  Serial.println("Listening...");
  pinMode(LED1, OUTPUT);
}

void loop()
{
  if(Mirf.dataReady()) {
    Mirf.getData((byte *) &value);
    Serial.print("Got data: ");
    Serial.println(value);
    //analogWrite(5, value);
     val = map(value, 0, 255, 45, 165);     // scale it to use it with the servo (value between 0 and 180)
     myservo.write(val); 
  }
}
