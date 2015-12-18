#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

extern "C"
{
  #include "user_interface.h" 
}
int status = WL_IDLE_STATUS;
const char* ssid = "AndroidAP";  //  your network SSID (name)
const char* pass = "12345678";   // your network password
unsigned int localPort = 2390;    // local port to listen for UDP packets
IPAddress ipMulti (192,168,1,255);
byte packetBuffer[512]; //buffer to hold incoming and outgoing packets
WiFiUDP Udp;  // A UDP instance to let us send and receive packets over UDP

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

////////////////////////////////////////////////////////6
// PIN DEFINITIONS
int BUTTON = 16;        //0 if pressed
int RGB_R = 13;
int RGB_G = 12;
int RGB_B = 14;

int Brightness_R = 0;
int Brightness_G = 0;
int Brightness_B = 0;

int button_prev_state = 1;      //0 if pressed
int button_press_counter = 0;
int fade_UP_down = 1;           // 1 when UP


void RGB_put(int r, int g, int b){
  uint32_t red =r;
  uint32_t green =g;
  uint32_t blue =b;

  blue= blue*90;
  blue= blue/100;
  
  analogWrite(RGB_R, red);
  analogWrite(RGB_G, green);
  analogWrite(RGB_B, blue); 
}




void UDP_process()
{
  int noBytes = Udp.parsePacket();
  if ( noBytes ) {
    Udp.read(packetBuffer,noBytes); // read the packet into the buffer
      if(noBytes == 9){
        int red = packetBuffer[4];
        int green = packetBuffer[5];
        int blue = packetBuffer[6];
        Serial.println("RGB: " + String(red) +"  " + String(green) +"  " + String(blue));
        Brightness_R = red* 4;
        Brightness_G = green * 4;
        Brightness_B = blue * 4;
        RGB_put(Brightness_R, Brightness_G, Brightness_B);   
      }
  }

}

void Wifi__setup()
{

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
}




 void brightness_plus(){
    if(Brightness_R < 1018){
      Brightness_R += 5;
    }else{
      Brightness_R = 1023;
      //fade_UP_down = 0;
    }
    if(Brightness_G < 1018){
      Brightness_G += 5;
        }else{
      Brightness_G = 1023;
     // fade_UP_down = 0;
    }
    if(Brightness_B < 1018){
      Brightness_B += 5;
        }else{
      Brightness_B = 1023;
     // fade_UP_down = 0;
    }
    if( (Brightness_R==1023)&&(Brightness_G==1023)&&(Brightness_B==1023) )fade_UP_down = 0;
    /*analogWrite(RGB_R, Brightness_R);
    analogWrite(RGB_G, Brightness_G);
    analogWrite(RGB_B, Brightness_B);*/
    RGB_put(Brightness_R, Brightness_G, Brightness_B);  
  }

  void brightness_minus(){        
    if(Brightness_R > 5){
      Brightness_R -= 5;
    }else{
      Brightness_R = 0;
      //fade_UP_down = 1;
    }
    if(Brightness_G > 5){
      Brightness_G -= 5;
        }else{
      Brightness_G = 0;
     // fade_UP_down = 1;
    }
    if(Brightness_B > 5){
      Brightness_B -= 5;
        }else{
      Brightness_B = 0;
      //fade_UP_down = 1;
    }
    if( Brightness_R + Brightness_G + Brightness_B == 0) fade_UP_down = 1;
    /*analogWrite(RGB_R, Brightness_R);
    analogWrite(RGB_G, Brightness_G);
    analogWrite(RGB_B, Brightness_B);*/
    RGB_put(Brightness_R, Brightness_G, Brightness_B);
  }

  

void button_read(){
  
  if(digitalRead(BUTTON)==1 && button_prev_state == 1){   //not press now , not pressed before      hold release
    return;
  }

  if(digitalRead(BUTTON)==1 && button_prev_state == 0){   //not press now , pressed before          released now
     delay(300);
     button_prev_state = digitalRead(BUTTON);
     fade_UP_down = ! fade_UP_down;
    if(button_press_counter < 100){
      //switch state
      if(Brightness_R + Brightness_G + Brightness_B  == 0){
        Serial.println("switch ON");
        Brightness_R = 1023;
        Brightness_G = 1023;
        Brightness_B = 1023;
        fade_UP_down = 0;
      }else{
        Serial.println("switch OFF");
        Brightness_R = 0;
        Brightness_G = 0;
        Brightness_B = 0;
        fade_UP_down =1;
      }
    }
    button_press_counter = 0;
    /*analogWrite(RGB_R, Brightness_R);
    analogWrite(RGB_G, Brightness_G);
    analogWrite(RGB_B, Brightness_B);*/
    RGB_put(Brightness_R, Brightness_G, Brightness_B);
    return;
  }

  if(digitalRead(BUTTON)==0 && button_prev_state == 1){   // press now , not pressed before         pressed now
     button_prev_state = digitalRead(BUTTON);
    return;
  }

  if(digitalRead(BUTTON)==0 && button_prev_state == 0){   // press now , pressed before           hold perssed
     button_press_counter ++;
     if(button_press_counter > 100){
      if(fade_UP_down == 1){
        brightness_plus();
      }else{
        brightness_minus();
      }
    }
    return;
  }

}


 


  
  // the setup routine runs once when you press reset:
void setup() {   
    // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }           
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);
  pinMode(BUTTON, INPUT); 
  analogWriteFreq(200);
 /* analogWrite(RGB_R, Brightness_R);
  analogWrite(RGB_G, Brightness_G);
  analogWrite(RGB_B, Brightness_B);  */
  RGB_put(Brightness_R, Brightness_G, Brightness_B);

  Serial.println("***");
  Serial.println("***");
  Wifi__setup();
  Serial.println("Program started");

}

// the loop routine runs over and over again forever:
void loop() {
         delay(20);      //delay 20 ms
         button_read();
     UDP_process();
           
}
