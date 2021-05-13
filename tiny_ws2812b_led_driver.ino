////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Fast Adressable Bitbang LED Library
/// Copyright (c)2015, 2017 Dan Truong
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <FAB_LED.h>
#include <EEPROM.h>

#define PIN_LED 0
#define PIN_TOUCH 2

#define NUM_LEDS 60


#define MODE_BK 0
#define MODE_NW 1
#define MODE_WW 2
#define MODE_CL 3
#define MODE_RB 4
#define MODE_RV 5


#define SPEED 15
#define LONG_PRESS 500
#define VERY_LONG_PRESS 2000

#define NUM_BRIGHT 4
const uint8_t tab_bright [NUM_BRIGHT] = {30, 60, 120, 255};


// Declare the LED protocol and the port
ws2812b<D,PIN_LED>  strip;

// The pixel array to display
grb  pixels[NUM_LEDS] = {};

uint8_t r;
uint8_t g;
uint8_t b;

uint8_t mode = EEPROM.read(0);
uint8_t bright = EEPROM.read(1);

bool state = 0;
bool last_state = 0;

unsigned long duration = 0;
unsigned long last_millis = 0;

unsigned long elapsed = 0;



void setup()
{
  // Turn off the LEDs
  strip.clear(2 * NUM_LEDS);

   pinMode(PIN_TOUCH, INPUT_PULLUP);
}


void loop()
{

  last_state = state;
  state = !digitalRead(PIN_TOUCH);
  
  elapsed = millis();

  if (state == false && last_state == true) {
    if (duration > LONG_PRESS && duration < VERY_LONG_PRESS) {
      bright += 1;
      if (bright >= NUM_BRIGHT) {bright = 0;}
    }
    else if (duration > VERY_LONG_PRESS) {
      EEPROM.write(0, mode);
      EEPROM.write(1, bright);
      flash_();
      }
    else {
      mode += 1;
      if (mode > MODE_RV) {mode = 0;}
    }
  }

  if (state == true){
    duration = millis() - last_millis;
  }

  if (state == false) {
    duration = 0;
    last_millis = millis();
  }
  

  
  Serial.println(mode);
  if (mode == MODE_BK) {
    r = 0;
    g = 0;
    b = 0;
    applyBright();
    fill();
  }
    
  if (mode == MODE_NW) {
    r = 255;
    g = 130;
    b = 41;
    applyBright();
    fill();
  }
    
  if (mode == MODE_WW) {
    r = 255;
    g = 235;
    b = 240;
    applyBright();
    fill();
  }
    
  if (mode == MODE_CL) {
    setLedColorHSV((256*elapsed)/(1000*SPEED) % 256, 255, 255);
    applyBright();
    fill();
  }
    
  if (mode == MODE_RB) { 

    for (int i=0 ; i<NUM_LEDS ; i++) {
      setLedColorHSV( ((256*elapsed)/(1000*SPEED) + (256*i/NUM_LEDS)) % 256 , 255, 255 );
      applyBright();
      setColors(i);
    }
  }

  if (mode == MODE_RV) {
    for (int i=0 ; i<NUM_LEDS ; i++) {
      setLedColorHSV( ((256*elapsed)/(1000*SPEED) + (256*i/NUM_LEDS)) % 256 , 255, 255 );
      applySingleBright( abs(256 - 2*(((256*elapsed)/(1000*SPEED) + (150*i/NUM_LEDS)) % 256)) );
      applyBright();
      setColors(i);
    }
    
    
  }

  // Display the pixels on the LED strip
  strip.sendPixels(NUM_LEDS, pixels);

}



void setColors(int i) {
  pixels[i].r = r;
  pixels[i].g = g;
  pixels[i].b = b;
}

void fill() {
  for(int i = 0; i < NUM_LEDS; i++)
  {
    pixels[i].r = r;
    pixels[i].g = g;
    pixels[i].b = b;
  }
}

void applySingleBright(byte bright_) {
  uint8_t rb = r*bright_/255.0;
  r = rb;

  uint8_t gb = g*bright_/255.0;
  g = gb;

  uint8_t bb = b*bright_/255.0;
  b = bb;
}

void applyBright() {

  float bright_value = tab_bright[bright]/255.0;
  
  uint8_t rb = r*bright_value;
  r = rb;

  uint8_t gb = g*bright_value;
  g = gb;

  uint8_t bb = b*bright_value;
  b = bb;
}

void flash_() {
  strip.clear(NUM_LEDS);
  strip.sendPixels(NUM_LEDS, pixels);
  delay(100);
  
  r = 60;
  g = 60;
  b = 60;
  fill();
  strip.sendPixels(NUM_LEDS, pixels);
  delay(300);
  
}

void setLedColorHSV(byte h, byte s, byte v) {
  // this is the algorithm to convert from RGB to HSV
  h = (h * 192) / 256;  // 0..191
  unsigned int i = h / 32;   // We want a value of 0 thru 5
  unsigned int f = (h % 32) * 8;   // 'fractional' part of 'i' 0..248 in jumps

  unsigned int sInv = 255 - s;  // 0 -> 0xff, 0xff -> 0
  unsigned int fInv = 255 - f;  // 0 -> 0xff, 0xff -> 0
  byte pv = v * sInv / 256;  // pv will be in range 0 - 255
  byte qv = v * (256 - s * f / 256) / 256;
  byte tv = v * (256 - s * fInv / 256) / 256;

  switch (i) {
  case 0:
    r = v;
    g = tv;
    b = pv;
    break;
  case 1:
    r = qv;
    g = v;
    b = pv;
    break;
  case 2:
    r = pv;
    g = v;
    b = tv;
    break;
  case 3:
    r = pv;
    g = qv;
    b = v;
    break;
  case 4:
    r = tv;
    g = pv;
    b = v;
    break;
  case 5:
    r = v;
    g = pv;
    b = qv;
    break;
  }

}
