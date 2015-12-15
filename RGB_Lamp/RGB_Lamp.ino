
// PIN DEFINITIONS
int BUTTON = 16;        //0 if pressed
int RGB_R = 13;
int RGB_G = 14;
int RGB_B = 12;

int Brightness_R = 0;
int Brightness_G = 0;
int Brightness_B = 0;

int button_prev_state = 1;      //0 if pressed
int button_press_counter = 0;
int fade_UP_down = 1;           // i when UP


// the setup routine runs once when you press reset:
void setup() {   
  Serial.begin(9600);             
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);
  pinMode(BUTTON, INPUT); 
 /* analogWrite(RGB_R, Brightness_R);
  analogWrite(RGB_G, Brightness_G);
  analogWrite(RGB_B, Brightness_B);  */
  RGB_put(Brightness_R, Brightness_G, Brightness_B);

  Serial.println("***");
  Serial.println("***");
  Serial.println("Program started");

}

// the loop routine runs over and over again forever:
void loop() {
         delay(20);      //delay 20 ms
         button_read();
           
}

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



void button_read(){
  
  if(digitalRead(BUTTON)==1 && button_prev_state == 1){   //not press now , not pressed before      hold release
    return;
  }

  if(digitalRead(BUTTON)==1 && button_prev_state == 0){   //not press now , pressed before          released now
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
     delay(300);
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


  void brightness_plus(){
    if(Brightness_R < 1018){
      Brightness_R += 5;
    }else{
      Brightness_R = 1023;
      fade_UP_down = 0;
    }
    if(Brightness_G < 1018){
      Brightness_G += 5;
        }else{
      Brightness_G = 1023;
      fade_UP_down = 0;
    }
    if(Brightness_B < 1018){
      Brightness_B += 5;
        }else{
      Brightness_B = 1023;
      fade_UP_down = 0;
    }
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
      fade_UP_down = 1;
    }
    if(Brightness_G > 5){
      Brightness_G -= 5;
        }else{
      Brightness_G = 0;
      fade_UP_down = 1;
    }
    if(Brightness_B > 5){
      Brightness_B -= 5;
        }else{
      Brightness_B = 0;
      fade_UP_down = 1;
    }
    /*analogWrite(RGB_R, Brightness_R);
    analogWrite(RGB_G, Brightness_G);
    analogWrite(RGB_B, Brightness_B);*/
    RGB_put(Brightness_R, Brightness_G, Brightness_B);
  }


