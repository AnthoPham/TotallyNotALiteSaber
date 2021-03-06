#include <Adafruit_NeoPixel.h>
#include <EEPROM.h> //This will be used to store the state value to enter after startup
#include <Arduino.h>
#include <Math.h> 
#include "TimerOne.h"

const int N_LEDS = 7;
double exposure = 10;
double wait = 65525/1000;
double hue = 0;
int brightness = 255;
int multiplier = 0;
int newmult = 0;
int pixel = 0;
int boot = 0;

//Encoder pins
const int encoderPinA= 3; 
const int encoderPinB= 4;
boolean encoderA = 0;
boolean encoderB = 0;
const int buttonpin = 5;
int encoder = 0;
int button = 0;
int prevbutton = 0;
int buttoncount = 0;

//BQ charger related pins and reads
const int PGOOD = 2;
boolean PGOOD_read = 0;
const int sys_gpio = 7;


//"sleep timer"
/*
 * If the difference between mill1 and mill2 
 */

unsigned long millis1 = 0; 
unsigned long millis2 = 0;
int count = 0;



int mainstate = 0; //
/*
   0 = startup
   1 = glow with encoder controlled color
   2 = rainbow glow (Raph's code)
   3 = no glow hum with encoder controlled color
   4 = 'fire' flucation with encoder controlled color
   5 = RED
   6 = BLUE
   7 = YELLOW
   8 = GREEN
   9 = BLUE
   10 = INDIGO
   11 = VIOLET
   12 = Charging

   5 - 11 have pot controlled brightness

*/

int timebased = 0; //case 2 and 1

// Fire
unsigned int array[4000];
int bright4 = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, 6, NEO_GRB + NEO_KHZ800);


void setup() {
  
  
  strip.fill(strip.Color(0, 0, 0));
  strip.setBrightness(255);
  strip.show();
  Serial.begin(115200); // initialize serial communication at 115200

  pinMode(PGOOD, INPUT); //set as input just to read when USB is powered
  pinMode(sys_gpio, OUTPUT); //set as output to control the FET that can disconnect the sys_gpio
  digitalWrite(sys_gpio, HIGH);
  PGOOD_read = digitalRead(PGOOD); //0 = Low = USB powered, 1 = High = No USB power
  if(PGOOD_read == LOW) {
    mainstate = 13; //keeps the RGB off when powered off of usb
  }
  else {
    mainstate = 0;
    //mainstate should remain 0 to allow for proper stuff
  }

  
  
  noInterrupts();
  strip.begin();
  strip.fill(strip.Color(0, 0, 0));
  strip.show();
  hue = random(0, 65534);
  schwoom();
  
  // create timer clocks
  // 1. 10ms clock for glows
  // create interrupt based on timer
  Timer1.initialize(14000);
  Timer1.attachInterrupt(background);

  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(buttonpin, INPUT_PULLUP);
  digitalWrite(encoderPinA, HIGH);
  digitalWrite(encoderPinB, HIGH);
  attachInterrupt(1, EncoderAISR, CHANGE);
  
  // create interrupt based on button (from rotatry encoder). I may not need this?

  //read EEPROM for state to enter after startup.
  //Can't build a lightsaber without the "scwhoom" animation.
  //schwoomanimation()
  //read EEPROM
  //
  interrupts();
  millis1 = millis();
}

void loop() {
}


void stateincrement() { //should also blink blink to indicate new state
  
  switch(mainstate) {
    case 11:
      mainstate = 0;
      break;
    case 13:
      mainstate = 13;
      break;
    default:
      mainstate++;
      break;
  }
  Serial.print("New state: ");
  Serial.println(mainstate);
}

void schwoom() {
  for(int i = 0; i<N_LEDS; i++) {
    strip.setPixelColor(i, strip.ColorHSV(hue));
    strip.show();
  }
}

void background() { //10ms timer ISR
//  Serial.println(" Timer ISR begin" ); //useful for seeing Hz of TimerISR
  millis2 = millis();
  if(millis2 - millis1 >= 1000) {
    millis1 = millis2;
    count += 1;
    if(count >= 900) {
      digitalWrite(sys_gpio, LOW);
    }
  }
  switch(mainstate) {
    case 0: //start up
      stateincrement();
//      strip.setBrightness(brightness); //apply brightness
//      strip.fill(strip.ColorHSV(hue));           //apply color
      break;
    case 1: //humming brightness
      if(timebased >= 2){
          switch (multiplier) {
            case 0: //increasing brightness
              brightness += 1;
              if (brightness >= 254) {
                multiplier = 1;
              }
              break;
            case 1: //decreasing brightness
              brightness -= 1;
              if (brightness <= 21) {
                multiplier = 0;
              }
              break;
            }
        timebased = 0;
      }
      else { timebased++;} 
//      Serial.println(brightness); //Debug line
      strip.setBrightness(brightness); //apply brightness
      strip.fill(strip.ColorHSV(hue));           //apply color
      break;
    case 2: //Raph's code 
      hue += wait;
      if(hue > 65535) {hue = 0;}
      strip.fill(strip.ColorHSV(hue));           //apply color
      strip.show();                              // show on RGB strip
      break;
    case 3: //not going to vary with time, only encoder
      strip.setBrightness(brightness); //apply brightness
      strip.fill(strip.ColorHSV(hue));           //apply color
      break;
    case 4: //Brightness controlled randomlly
      if(pixel < N_LEDS) {
        bright4 = min( (int) round(41*sin(2*3.14*2000*pixel/100000)+120-41*cos(3.14*2000*pixel/100000)+41*sin(2*3.14*pixel/14)), 255);
        pixel+=1;
      }
      else {
        pixel = 0;
      }
      // Debug Lines
//      Serial.print("Brightness: ");
//      Serial.print(bright4);
//      Serial.print(" | pixel: ");
//      Serial.println(pixel);
      break;
    case 5:  //R
      hue = 65535*00/360;
      strip.fill(strip.ColorHSV(hue));
      break;
    case 6:  //O
      hue = 1430;
      strip.fill(strip.ColorHSV(hue));
      break;
    case 7:  //Y
      hue = 65535*60/360;
      strip.fill(strip.ColorHSV(hue));
      break;
    case 8:  //G
      hue = 65535*120/360;
      strip.fill(strip.ColorHSV(hue));
      break;
    case 9:  //B
      hue = 65535*240/360;
      strip.fill(strip.ColorHSV(hue));
      break;
    case 10: //I
      hue = 65535*247.6/360;
      strip.fill(strip.ColorHSV(hue));
      break;
    case 11: //V
      hue = 65535*270/360;
      strip.fill(strip.ColorHSV(hue));
      break;
//    case 12:
//      if(pixel < N_LEDS) {
//        bright4 = min( (int) round(41*sin(2*3.14*2000*pixel/100000)+120-41*cos(3.14*2000*pixel/100000)+41*sin(2*3.14*pixel/14)), 255);
//        pixel+=1;
//      }
//      else {
//        pixel = 0;
//      }
//      break;
    case 13:
      strip.fill(strip.Color(0, 0, 0));
      strip.show();
      break;   
  }
    
  button = digitalRead(buttonpin);
  if(button != prevbutton) {
    buttoncount = 0;
  }
  if(!button) { //pressed
    Serial.println(buttoncount);
    switch(buttoncount) {
      case 20:
        Serial.println("Button presse!");
        stateincrement();
        buttoncount = 0;
        break;
      default: 
        buttoncount++;
        break;
    }
  }
  prevbutton = button;

  
   
//  Serial.println(brightness); //debug line
  
  strip.show();                              // show on RGB strip
//  timer1_write(1000); //reset timer
}



void EncoderAISR(void) {
  encoderA = digitalRead(encoderPinA);
  encoderB = digitalRead(encoderPinB);

  encoder = (encoderA << 1 | encoderB);
  switch(mainstate) {
    case 0: //start up
      break;
    case 1: case 3: case 4: //hue controlled by encoder
      switch(encoder) {
        case 0: //counterclockwise
          hue-= wait*4;
          break;
        case 1: //clockwise
          hue += wait*4;
          break;
        
      }
      Serial.println(hue);
      
      break;
    case 2: case 5: case 6: case 7: case 8: case 9: case 10: case 11: //brightness controlled by encoder
      switch(encoder) {
        case 0: //counterclockwise
          brightness-= 10;
          break;
        case 1: //clockwise
          brightness += 10;
          break;
        
      }
      strip.setBrightness(brightness); //apply brightness
      strip.show();
      break;
  }
  if(brightness > 255) {brightness = 255;}
  if(brightness < 0  ) {brightness =   0;}
  
  if(hue > 65535) {hue = 0;}
  if(hue < 0  ) {hue =   65534;}
//  strip.fill(strip.ColorHSV(hue));           //apply color
//  strip.show();                              // show on RGB strip 
  // Debug serial prints
//  Serial.print("Encoder ");
//  Serial.println(encoder);
//
//  Serial.print("| Brightness  ");
//  Serial.print(brightness);
//  
//  Serial.print(" | Encoder A: ");
//  Serial.print(encoderA);
//  
//  Serial.print(" | Encoder B: ");
//  Serial.println(encoderB);
}
