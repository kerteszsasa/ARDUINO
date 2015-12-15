/*
 * 31 mar 2015
 * This sketch display UDP packets coming from an UDP client.
 * On a Mac the NC command can be used to send UDP. (nc -u 192.168.1.101 2390).
 *
 * Configuration : Enter the ssid and password of your Wifi AP. Enter the port number your server is listening on.
 *
 */

#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

extern "C"
{
  #include "user_interface.h" 
}

int status = WL_IDLE_STATUS;
const char* ssid = "HEGEHOG";  //  your network SSID (name)
const char* pass = "xxx";   // your network password

unsigned int localPort = 2390;    // local port to listen for UDP packets

IPAddress ipMulti (192,168,1,255);

byte packetBuffer[512]; //buffer to hold incoming and outgoing packets

WiFiUDP Udp;  // A UDP instance to let us send and receive packets over UDP



void multicas_at_start_esp8266() //When ESP8266 starts, Broadcast presence of ESP8255
{
  Udp.beginPacket(ipMulti, 2390);   
  Udp.write("ESP8266 ID#");
  Udp.print(system_get_chip_id());
  Udp.write("#ESP8266 IP#");
  Udp.println(WiFi.localIP());
  //Udp.writeln("answer");
  Udp.endPacket();
 
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // setting up Station AP
  WiFi.begin(ssid, pass);
 
  // Wait for connect to AP
  Serial.print("[Connecting]");
  Serial.print(ssid);
  int tries=0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tries++;
    if (tries > 30){
      break;
    }
  }
  Serial.println();

  printWifiStatus();

  Serial.println("Connected to wifi");
  Serial.print("Udp server started at port ");
  Serial.println(localPort);
  Udp.begin(localPort);
  multicas_at_start_esp8266();
}

void loop()
{
  int noBytes = Udp.parsePacket();
  if ( noBytes ) {
    Serial.print(millis() / 1000);
    Serial.print(":Packet of ");
    Serial.print(noBytes);
    Serial.print(" received from ");
    Serial.print(Udp.remoteIP());
    Serial.print(":");
    Serial.println(Udp.remotePort());
    // We've received a packet, read the data from it
    Udp.read(packetBuffer,noBytes); // read the packet into the buffer

    // display the packet contents in HEX
    for (int i=1;i<=noBytes;i++){
      Serial.print(packetBuffer[i-1],HEX);
      if (i % 32 == 0){
        Serial.println();
      }
      else Serial.print(' ');
    } // end for
    Serial.println();
//    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); //Send answer to Packet-Sender
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); //Send Answer to Network (Broadcast)   
    Udp.write("answer");
    Udp.endPacket();
  } // end if
}

