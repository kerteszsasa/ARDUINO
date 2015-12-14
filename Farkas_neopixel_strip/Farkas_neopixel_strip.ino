// This is a demonstration on how to use an input device to trigger changes on your neo pixels.
// You should wire a momentary push button to connect from ground to a digital IO pin.  When you
// press the button it will change to a new pixel animation.  Note that you need to press the
// button once to start the first animation!

#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#define BUTTON_PIN   12    // Digital IO pin connected to the button.  This will be
                          // driven with a pull-up resistor so the switch should
                          // pull the pin to ground momentarily.  On a high -> low
                          // transition the button press logic will execute.

#define PIXEL_PIN    6    // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 100

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

bool oldState = HIGH;
int showType = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  showType = EEPROM.read(0);



while(digitalRead(BUTTON_PIN) == HIGH); // remove after development
  
}

void loop() {
 /* // Get current button state.
  bool newState = digitalRead(BUTTON_PIN);

  // Check if state changed from high to low (button press).
  if (newState == LOW && oldState == HIGH) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    newState = digitalRead(BUTTON_PIN);
    if (newState == LOW) {
      showType++;
      if (showType > 9)
        showType=0;
      startShow(showType);
    }
  }

  // Set the last button state to the old state.
  oldState = newState;

*/

  if(digitalRead(BUTTON_PIN) == LOW){
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    delay(1000);
    if(digitalRead(BUTTON_PIN) == LOW){
      showType++;
      if (showType > 10) showType=0;
      EEPROM.write(0, showType);
    }
  }
  startShow(showType);
}

void startShow(int i) {
  i=10;
  switch(i){
    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
            break;
    case 1: colorWipe(strip.Color(255, 0, 0), 50);  // Red
            break;
    case 2: colorWipe(strip.Color(0, 255, 0), 50);  // Green
            break;
    case 3: colorWipe(strip.Color(0, 0, 255), 50);  // Blue
            break;
    case 4: theaterChase(strip.Color(127, 127, 127), 50); // White
            break;
    case 5: theaterChase(strip.Color(127,   0,   0), 50); // Red
            break;
    case 6: theaterChase(strip.Color(  0,   0, 127), 50); // Blue
            break;
    case 7: rainbow(20);
            break;
    case 8: rainbowCycle(20);
            break;
    case 9: theaterChaseRainbow(50);
            break;
    case 10:SawtoothColorChanger(100);
            break;
    default:showType=0;
            break;
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}


void SawtoothColorChanger(uint8_t wait){
int color_temp = 0;
  for(int color=0; color<60; color++){
    if(color == 60)color = 0;
    color_temp = color;
    delay(wait); 
    for(int pixel=0; pixel < strip.numPixels(); pixel++){
      strip.setPixelColor(strip.numPixels() -pixel-1, TriangleColorWheel(color_temp));
      color_temp++;
      if(color_temp == 60)color_temp = 0;
    }
    strip.show();
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

//input: 0-9, output: 0-255
uint8_t Brightness(uint8_t val){

  val = 9-val;
  if(val == 0)return 1;
  if(val == 1)return 5;
  if(val == 2)return 15;
  if(val == 3)return 20;
  if(val == 4)return 20;
  if(val == 5)return 50;
  if(val == 6)return 100;
  if(val == 7)return 150;
  if(val == 8)return 200;
  if(val == 9)return 255;
  return 0;
}

// Input a value 0 to 59 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t TriangleColorWheel(byte WheelPos) {
  //Red
  if(WheelPos < 10) {
    return strip.Color(Brightness(WheelPos), 0, 0);
  }
  //Yellow
  if(WheelPos < 20) {
    WheelPos -= 10;
    return strip.Color(Brightness(WheelPos), Brightness(WheelPos), 0);
  }
    //Green
  if(WheelPos < 30) {
    WheelPos -= 20;
    return strip.Color(0, Brightness(WheelPos), 0);
  }
    //Cyan
  if(WheelPos < 40) {
    WheelPos -= 30;
    return strip.Color(0, Brightness(WheelPos), Brightness(WheelPos));
  }
    //Blue
  if(WheelPos < 50) {
    WheelPos -= 40;
    return strip.Color(0, 0, Brightness(WheelPos));
  }
    //Magenta
  if(WheelPos < 60) {
    WheelPos -= 50;
    return strip.Color(Brightness(WheelPos), 0, Brightness(WheelPos));
  }

}
