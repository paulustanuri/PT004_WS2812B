#include <FastLED.h>
#define INT_PIN     2
#define LED_PIN     8
#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define NUM_ROWS  16
#define NUM_COLS  16
#define NUM_LEDS  (NUM_ROWS * NUM_COLS)
#define BRIGHTNESS  90
#define SPEEDS 5
#define autoShutdown true
#define autoShutdownTime 7200000//miliseconds

CRGB leds[NUM_LEDS];
//CRGBArray<NUM_LEDS> leds;
unsigned long runtime;
struct bubble{
    int POS_X;
    int POS_Y;
    uint8_t HUE;
    bool UP;
    bool RIGHT;
    int SPEED;
    int COUNT;
  };

bubble BUBBLES[3]; 

void setup() {
  //Serial.begin(9600);
  
  LEDS.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);
  randomSeed(analogRead(0));

  BUBBLES[0].POS_X = random(1,8);
  BUBBLES[0].POS_Y = random(1,8);
  BUBBLES[0].HUE = random(0,90);
  BUBBLES[0].UP = true;
  BUBBLES[0].RIGHT = true;
  BUBBLES[0].SPEED = 6;
  BUBBLES[0].COUNT = 0;

  BUBBLES[1].POS_X = random(8,15);
  BUBBLES[1].POS_Y = random(8,15);
  BUBBLES[1].HUE = random(100,180);
  BUBBLES[1].UP = false;
  BUBBLES[1].RIGHT = true;
  BUBBLES[1].SPEED = 5;
  BUBBLES[1].COUNT = 0;

  BUBBLES[2].POS_X = random(15);
  BUBBLES[2].POS_Y = random(8,15);
  BUBBLES[2].HUE = random(190,255);
  BUBBLES[2].UP = true;
  BUBBLES[2].RIGHT = true;
  BUBBLES[2].SPEED = 7;
  BUBBLES[2].COUNT = 0;
  
  //Serial.print("-");
  delay(500);
  setInterrupt();
}

void loop() {
  RunBubble();
  
  runtime = millis();
  if(autoShutdown && runtime > autoShutdownTime){
    shuttingDown();
  }
  
}

void setInterrupt(){
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), shuttingDown, FALLING);
}

void shuttingDown(){
  detachInterrupt(digitalPinToInterrupt(INT_PIN));
  pinMode(INT_PIN, OUTPUT);
  digitalWrite(INT_PIN, LOW);
}

void RunBubble(){
  for(int i=0; i<3;i++){
    if(BUBBLES[i].COUNT >= BUBBLES[i].SPEED){
      if(BUBBLES[i].UP && BUBBLES[i].POS_Y >= 0){
        BUBBLES[i].POS_Y--;
        if(BUBBLES[i].POS_Y == 0){
          if(BUBBLES[i].RIGHT){BUBBLES[i].POS_X++;}else{BUBBLES[i].POS_X--;}
        }
      }else{
        BUBBLES[i].POS_Y == 0;
        BUBBLES[i].UP = false;
      }
      if(!BUBBLES[i].UP && BUBBLES[i].POS_Y <= (NUM_ROWS -1)){
        BUBBLES[i].POS_Y++;
        if(BUBBLES[i].POS_Y == (NUM_ROWS -1)){
          if(BUBBLES[i].RIGHT){BUBBLES[i].POS_X++;}else{BUBBLES[i].POS_X--;}
        }
      }else{
        BUBBLES[i].POS_Y == (NUM_ROWS -1);
        BUBBLES[i].UP=true;
      }
      if(BUBBLES[i].RIGHT && BUBBLES[i].POS_X >= 0){
        BUBBLES[i].POS_X--;
      }else{
        BUBBLES[i].RIGHT=false;
      }
      if(!BUBBLES[i].RIGHT && BUBBLES[i].POS_X <= (NUM_COLS - 1)){
        BUBBLES[i].POS_X++;
      }else{
        BUBBLES[i].RIGHT=true;
      }
    }

    BUBBLES[i].HUE++;
    Set_LED_Hue(BUBBLES[i].HUE, BUBBLES[i].POS_X, BUBBLES[i].POS_Y);
    Set_LED_Hue(BUBBLES[i].HUE, BUBBLES[i].POS_X-1, BUBBLES[i].POS_Y-1);
    Set_LED_Hue(BUBBLES[i].HUE, BUBBLES[i].POS_X-1, BUBBLES[i].POS_Y);
    Set_LED_Hue(BUBBLES[i].HUE, BUBBLES[i].POS_X-1, BUBBLES[i].POS_Y+1);
    Set_LED_Hue(BUBBLES[i].HUE, BUBBLES[i].POS_X, BUBBLES[i].POS_Y-1);
    Set_LED_Hue(BUBBLES[i].HUE, BUBBLES[i].POS_X, BUBBLES[i].POS_Y+1);
    Set_LED_Hue(BUBBLES[i].HUE, BUBBLES[i].POS_X+1, BUBBLES[i].POS_Y-1);
    Set_LED_Hue(BUBBLES[i].HUE, BUBBLES[i].POS_X+1, BUBBLES[i].POS_Y);
    Set_LED_Hue(BUBBLES[i].HUE, BUBBLES[i].POS_X+1, BUBBLES[i].POS_Y+1);
  }
  LEDS.show();
  
  delay(SPEEDS);
  
  for(int i=0; i<3;i++){
    if(BUBBLES[i].COUNT++ > BUBBLES[i].SPEED){BUBBLES[i].COUNT=0;}
  }
  
  fadeall();
}

void fadeall() { 
  for(int i = 0; i < NUM_LEDS; i++) { 
    leds[i].nscale8(50); 
  } 
}

void Set_LED_Hue(uint8_t _hue, int _posX, int _posY){
  if(_posX >= 0 && _posX < NUM_COLS && _posY >=0 && _posY < NUM_ROWS){
    int numLED = Get_Led_Num(_posX, _posY);
    leds[numLED] = CHSV(_hue, 255,255);
  }
}


int Get_Led_Num(int posX, int posY){
  int NumLedPos = 0;
  if(posY >= NUM_ROWS){
    posY = 0;
  }
  if(posX >= NUM_COLS){
    posX = 0;
  }
  
  if(posY & 0x01){
    NumLedPos = (posY * NUM_COLS) + (NUM_COLS - posX - 1);
  }else{
    NumLedPos = (posY * NUM_COLS) + posX;
  }

  return NumLedPos;
}
