// Neopixel hw control using 2 pots and 2 mode momentaries
// This code is messy and ugly... but it works. :)
// I modified the Adafruit sample code to not use delay() which enables
// the hw controller to be much more response

#include <Adafruit_NeoPixel.h>

#define BUTTON1_PIN   3    // Digital IO pin connected to the button.  This will be
                          // driven with a pull-up resistor so the switch should
                          // pull the pin to ground momentarily.  On a high -> low
                          // transition the button press logic will execute.

                          
#define BUTTON2_PIN   4 

#define POT1    1
#define POT2    2

#define PIXEL_PIN    6    // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 24

unsigned long interval=50;  // the time we need to wait
unsigned long previousMillis=0;

 int neoPixelToChange = 0; //track which neoPixel to change
int neoPixel_j = 0; //stores values for program cycles
int rainbow_color = 0; // stores rainbow color

uint32_t currentColor;// current Color in case we need it
uint16_t currentPixel = 0;// what pixel are we operating on


// Read Brightness POT
int brightVal = 0;
int brightness = 0;
int oldBright = 0;


int wait = 15;
int rainbow_wait = 5;

// Allow button presses while in rainbow
int lightCycle = 0;


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
 
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
 // pinMode(BUTTON2_PIN, INPUT_PULLUP);

  currentColor = strip.Color(255,0,0);
  currentPixel = 0;
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  Serial.begin(9600);
}

void loop() {
  // Get current button state.
  bool newState = digitalRead(BUTTON1_PIN);
  //Serial.print("button1 state = ");
  Serial.println(newState);
  // Check if state changed from high to low (button press).
  if (newState == LOW && oldState == HIGH) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    newState = digitalRead(BUTTON1_PIN);
    if (newState == LOW) {
      showType++;
      if (showType > 4)
        showType=0;
      neoPixelToChange = 0;
      startShow(showType);
    }
  } else {
      startShow(showType);
  
  }
  

  // Set the last button state to the old state.
  oldState = newState;

  brightVal  = analogRead(POT1);  // range is 0 - 523
  int pot2 = analogRead(POT2);    // range is 0 - 525
//  Serial.print("brightval = ");
//  Serial.print(brightVal);
//  Serial.print("   |    pot2 = ");
//  Serial.println(pot2);

//  // if brightVal has changed by +/- 10 then update (else stay dim)
//  if (brightVal >= (oldBright+10) || brightVal <= (oldBright-10)) { 
//    brightness = map(brightVal, 0, 1023, 0, 255);
//    strip.setBrightness(brightness);
//    oldBright = brightVal;
//  }

    brightness = map(brightVal, 0, 523, 1, 250);
   //strip.setBrightness(brightness);
   // changing color instead
   neoPixel_j = brightness;
}


void colorSet(uint8_t color, uint8_t wait) {
    
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > wait) {
    
    // save the last time you changed a NeoPixel 
    previousMillis = currentMillis; 

    //change the colors of the pixels
    uint16_t i;
    //Serial.print("In ColorSet: neoPixel_j = ");
    //Serial.println(neoPixel_j);
    //Serial.println();

    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(color));
    }
    strip.show();
   // neoPixel_j = (neoPixel_j + 1) % 255; //increment j until all colors are used, then start over
  }
}

// Fill all the dots with one color
void allColor(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
  }
} // note for a random color, use:
  // allColor((unsigned long)random(0x01000000)); // random color
  
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {

  int debug = 0;
  wait = 1;
  unsigned long currentMillis = millis();
  if(debug) {
  Serial.print("in colorWipe. NeoPixelToChange = ");
  Serial.println(neoPixelToChange);
  Serial.println(" ");
  }
  //only do this if some of the pixels still need to be lit
  if (neoPixelToChange <= strip.numPixels()){
     if(debug) {
       Serial.print("in neoPixelToChange > numPixels. neoPixelToChange = ");
       Serial.println(neoPixelToChange);
       Serial.println(" ");
     }
   
    int num = currentMillis - previousMillis;
//    Serial.print("current millis (");
//    Serial.print(currentMillis);
//    Serial.print(") - previous millis (");
//    Serial.print(previousMillis);
//    Serial.print(" = ");
//    Serial.println(num);
//    int new_wait = wait * neoPixelToChange ;
//    Serial.println("new_wait (");
//    Serial.print(new_wait);
//    Serial.println(")");
//     Serial.println("");
    
   if(currentMillis - previousMillis > wait * neoPixelToChange) { //appears to be an exponential growth delay but works

      
      // save the last time you changed a NeoPixel 
      previousMillis = currentMillis;  
    
      //change a pixel
      // strip.setPixelColor(neoPixelToChange, c);
      strip.setPixelColor(neoPixelToChange,Wheel(neoPixel_j));
      strip.show();
      neoPixelToChange++;
//     Serial.print("in time > wait :  neoPixelToChange = ");
//      Serial.print(" neoPixelToChange = ");
//     Serial.println(neoPixelToChange);
//     Serial.println();
    }
  }
 
}



void rainbow(uint8_t wait) {
    
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > neoPixel_j) { // re-use pot value in time context
    
    // save the last time you changed a NeoPixel 
    previousMillis = currentMillis; 

    //change the colors of the pixels
    uint16_t i;

    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+rainbow_color) & 255));
    }
    strip.show();
    rainbow_color = (rainbow_color + 1) % 255; //increment j until all colors are used, then start over
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > neoPixel_j) {
    
    // save the last time you changed a NeoPixel 
    previousMillis = currentMillis; 

    //change the colors of the pixels
    uint16_t i;

    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + neoPixel_j) & 255));
    }
    strip.show();
    neoPixel_j = (neoPixel_j + 1) % 1279; // 5 cycles of all colors on wheel, then start over
  }
}

  //Theatre-style crawling lights.
void theaterChase(uint8_t wait) {

  for (int j=0; j<2; j++) {  //do 1 cycles of chasing

    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel(neoPixel_j));   //turn every third pixel on
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
  // for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
 
    lightCycle++;
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+lightCycle) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  //}
  if(lightCycle == 256){
    lightCycle = 0;}
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}




void startShow(int i) {
  switch(i){
   // case 0: colorSet(strip.Color(0, 0, 0), wait);    // Black/off
   //         break;
    case 0: rainbowCycle(rainbow_wait);
            break;
    case 1: colorSet(neoPixel_j, wait);
            //colorWipe(strip.Color(255, 0, 0), wait);  
            break;
 //   case 1: theaterChase(wait);
 //           break;
    case 2: rainbow(rainbow_wait);
            break;
  
 //   case 4: theaterChaseRainbow(wait);
 //           break;
  }
}
