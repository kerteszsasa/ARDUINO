#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

int value;

void setup()
{
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  // Set own address - unique 5 character string
  Mirf.setRADDR((byte *)"ABCDE"); 
  Mirf.payload = sizeof(value);
  Mirf.channel = 90;
  Mirf.config();
}

void loop()
{
  // Set remote address to send to
  Mirf.setTADDR((byte *)"FGHIJ"); 
  //value = random(255);
  value = analogRead(2)/4;
  Mirf.send((byte *)&value);
  while(Mirf.isSending()) delay(1);
  delay(100);
}
