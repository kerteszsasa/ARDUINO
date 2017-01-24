// This example uses an Adafruit Huzzah ESP8266
// to connect to shiftr.io.
//
// You can check on your device after a successful
// connection here: https://shiftr.io/try.
//
// by Joël Gähwiler
// https://github.com/256dpi/arduino-mqtt

#include <ESP8266WiFi.h>
#include <MQTTClient.h>

const char *ssid = "KERTESZ";
const char *pass = "harlekin";

// #define PWMRANGE 255
// #define PWM_VALUE 31
//int gamma_table[PWM_VALUE+1] = {
// 0, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 8, 10, 11, 13, 16, 19, 23,
// 27, 32, 38, 45, 54, 64, 76, 91, 108, 128, 152, 181, 215, 255
//};
 
 #define PWM_VALUE 63
//int gamma_table[PWM_VALUE+1] = {
// 0, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7, 8, 9, 10,
// 11, 12, 13, 15, 17, 19, 21, 23, 26, 29, 32, 36, 40, 44, 49, 55,
// 61, 68, 76, 85, 94, 105, 117, 131, 146, 162, 181, 202, 225, 250,
// 279, 311, 346, 386, 430, 479, 534, 595, 663, 739, 824, 918, 1023
//};

int gamma_table[PWM_VALUE+1] = {
 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10,
 11, 12, 13, 15, 17, 19, 21, 23, 26, 29, 32, 36, 40, 44, 49, 55,
 61, 68, 76, 85, 94, 105, 117, 131, 146, 162, 181, 202, 225, 250,
 279, 311, 346, 386, 430, 479, 534, 595, 663, 739, 824, 918, 1023
};



// RGB FET
#define redPIN 13//12
#define greenPIN 12//14
#define bluePIN 14//16




// W FET
#define w1PIN 13

// note 
// TX GPIO2 @Serial (Serial ONE)
// RX GPIO3 @Serial 

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected to wifi!");


  Serial.print("\nconnecting to mqtt...");
  /*while (!client.connect("arduino", "try", "try")) { //maybe userid username passwd???
    Serial.print(".");
    delay(1000);
  }*/

  
  //client.subscribe("/example");
  // client.unsubscribe("/example");
   if (client.connect("WiFi_RGB_2")) {
   client.subscribe("/openHAB/RGB_2/Color");
  client.subscribe("/openHAB/RGB_2/SW1");
  Serial.println("MQTT connected"); 
 }
}

void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  Serial.print("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(payload);
  Serial.println();
/**********************************************************/
   Serial.print(topic);
 Serial.print(" => ");
 Serial.println(payload);
 
 //String payload = pub.payload_string();

if(String(topic) == "/openHAB/RGB_2/Color"){
 int c1 = payload.indexOf(';');
 int c2 = payload.indexOf(';',c1+1);
 
 int red = map(payload.toInt(),0,100,0,PWM_VALUE);
 red = constrain(red,0,PWM_VALUE);
 int green = map(payload.substring(c1+1,c2).toInt(),0,100,0,PWM_VALUE);
 green = constrain(green, 0, PWM_VALUE);
 int blue = map(payload.substring(c2+1).toInt(),0,100,0,PWM_VALUE);
 blue = constrain(blue,0,PWM_VALUE);
 
 red = gamma_table[red];
 green = gamma_table[green];
 blue = gamma_table[blue];
 
 
 analogWrite(redPIN, red);
 analogWrite(greenPIN, green);
 analogWrite(bluePIN, blue);

 Serial.println(red);
 Serial.println(green);
 Serial.println(blue);
 }
 else if(String(topic) == "/openHAB/RGB_2/SW1"){
 int value = map(payload.toInt(),0,100,0,PWM_VALUE);
 value = constrain(value,0,PWM_VALUE);
 value = gamma_table[value];
 analogWrite(w1PIN, value);
 Serial.println(value);
 }

}

void setup() {
  pinMode(redPIN, OUTPUT);
   pinMode(greenPIN, OUTPUT);
  pinMode(bluePIN, OUTPUT);
  pinMode(w1PIN, OUTPUT);
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  client.begin("iot.eclipse.org", net);

  connect();
}



void loop() {
  client.loop();
  delay(10); // <- fixes some issues with WiFi stability

  if(!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  /*if(millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish("/hello", "world");
  }*/
}



