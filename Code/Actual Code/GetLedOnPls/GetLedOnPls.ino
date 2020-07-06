
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <Math.h> 

#define SYS_GPIO 7   //Digital Pin 7
#define Pixel_Data 6 //Digital Pin 6
#define ENC_Button 5  //Digital Pin 5
#define ENC_PIN_A 3   //Digital Pin 3
#define ENC_PIN_B 4   //Digital Pin 0
#define PGOOD 2      //Digital Pin 2 //Active low

const int N_LEDS = 7; //number of LEDS; 7 = test strip
double exposure = 10;
double wait = 65525/1000;
double hue = 0;
int brightness = 255;
int multiplier = 0;
int newmult = 0;
int pixel = 0;
int boot = 0;
unsigned long previousmillis = 0;


//Encoder pins
const int encoderPinA= 12; 
const int encoderPinB= 13;
boolean encoderA = 0;
boolean encoderB = 0;
const int buttonpin = 14;
int encoder = 0;
int button = 0;
int prevbutton = 0;
int buttoncount = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, Pixel_Data, NEO_GRB + NEO_KHZ800);

void setup() {
  // enable sys_gpio output
  // pull sys_gpio high
  //
  //enable pgood pin
  //configure interrupt

  //enable encoder pin

  strip.fill(strip.Color(0, 0, 0));
  strip.setBrightness(255);
  strip.show();
  
  noInterrupts();
  strip.begin();
  strip.fill(strip.Color(0, 0, 0));
  strip.show();

  hue = random(0, 65534);
  schwoom();
}

void loop() {
  // put your main code here, to run repeatedly:

}

//interrupt for pgood pin (potentially not needed)
//if (pgood is low) -> output data


void schwoom() { //should edit this to be an array fill for frontside = [] and backside = []
  for(int i = 0; i<N_LEDS; i++) {
    strip.setPixelColor(i, strip.ColorHSV(hue));
    strip.show();
  }
}
