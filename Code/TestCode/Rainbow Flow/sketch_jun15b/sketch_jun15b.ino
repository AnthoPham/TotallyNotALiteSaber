#include <Adafruit_NeoPixel.h>
// #include <FastLED.h>
const int N_LEDS = 7;
double exposure = 10;
double wait = 0;
unsigned long previousmillis = 0;
double hue = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, 6, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.fill(strip.Color(0, 0, 0));
  strip.setBrightness(60);
  strip.show();
  wait = 65535/(exposure*100);
}

void loop() {
  if(hue > 65535) hue = 0;
  unsigned long currentmillis = millis();
  if(currentmillis - previousmillis >= 10){
    hue+=wait;
    strip.fill(strip.ColorHSV(hue));
    strip.show();
    previousmillis = currentmillis;
  }
}
