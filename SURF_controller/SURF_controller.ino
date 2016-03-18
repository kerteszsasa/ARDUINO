#define RED                           10
#define GREEN                         9
#define BLUE                          5
#define MODE_SELECT_PIN               A6
int MODE_RECEIEVER = 0;
int MODE_TRANSMITTER = 0;


 


// the setup routine runs once when you press reset:
void setup() {                
  //RGB init
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);
  digitalWrite(BLUE, HIGH);    
}

// the loop routine runs over and over again forever:
void loop() {

  delay(1000);               // wait for a second

  if(analogRead(A6) > 512){
    digitalWrite(RED, LOW);
  }
  else{
    digitalWrite(BLUE, LOW);
  }

delay(1000); 
  digitalWrite(RED, HIGH);
  digitalWrite(BLUE, HIGH);



  
}
