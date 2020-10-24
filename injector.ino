#define INJECTORS_PIN 2
#define BTN_PIN A4
#define BTN_GND A5
// Display
#define DISP_CLKPIN A0
#define DISP_DIOPIN A1
#define DISP_VCC A2
#define DISP_GND A3

/************** БУКВЫ И СИМВОЛЫ *****************/
#define _A 0x77
#define _B 0x7f
#define _C 0x39
#define _D 0x3f
#define _E 0x79
#define _F 0x71
#define _G 0x3d
#define _H 0x76
#define _J 0x1e
#define _L 0x38
#define _N 0x37
#define _O 0x3f
#define _P 0x73
#define _S 0x6d
#define _U 0x3e
#define _Y 0x6e
#define _a 0x5f
#define _b 0x7c
#define _c 0x58
#define _d 0x5e
#define _e 0x7b
#define _f 0x71
#define _h 0x74
#define _i 0x10
#define _j 0x0e
#define _l 0x06
#define _n 0x54
#define _o 0x5c
#define _q 0x67
#define _r 0b00110001
#define _t 0x78
#define _u 0x1c
#define _y 0x6e
#define _- 0x40
#define __ 0x08
#define _= 0x48
#define _empty 0x00

#define _0 0x3f
#define _1 0x06
#define _2 0x5b
#define _3 0x4f
#define _4 0x66
#define _5 0x6d
#define _6 0x7d
#define _7 0x07
#define _8 0x7f
#define _9 0x6f
/************** БУКВЫ И СИМВОЛЫ *****************/

  // Дисплей
  #include <TM1637Display.h>
  TM1637Display display(DISP_CLKPIN, DISP_DIOPIN);


#define ENC_CLKPIN 9
#define ENC_DTPIN 8
#define ENC_SWPIN 7
#define ENC_PWRPIN 6
#define ENC_GNDPIN 5
#include "GyverEncoder.h"
Encoder enc1(ENC_CLKPIN, ENC_DTPIN, ENC_SWPIN);  // для работы c кнопкой

byte menu = 0;
bool menuEntered, started = false;

int T = 100, Rpm = 600, t1=10, d=0;
unsigned long t, lastInjectorOnTime = 0, timer=60000, timerStarted = 0, lastTimeRefresh=0;

void setup() {
  //Serial.begin(9600); 

  pinMode(INJECTORS_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(BTN_GND, OUTPUT);

  pinMode(DISP_VCC, OUTPUT); digitalWrite(DISP_VCC, HIGH);
  pinMode(DISP_GND, OUTPUT); digitalWrite(DISP_GND, LOW);
  
  pinMode(ENC_PWRPIN, OUTPUT); digitalWrite(ENC_PWRPIN, HIGH);
  pinMode(ENC_GNDPIN, OUTPUT); digitalWrite(ENC_GNDPIN, LOW);
  

  display.setBrightness(7);

  enc1.setType(TYPE1);

  setRpm();
  refreshDisplay();
}


void loop() {
  t = millis();
  //if(!digitalRead(BTN_PIN)) ;

  if(timerStarted && (t > timerStarted+timer)) {
    timerStarted = 0;
    started = false;
  }  


  if(started) {
    if(t > (lastInjectorOnTime + t1)) {
      digitalWrite(INJECTORS_PIN, LOW);   
      digitalWrite(LED_BUILTIN, LOW);   
    } 
    if(t > (lastInjectorOnTime + T)) {
      digitalWrite(INJECTORS_PIN, HIGH);  
      digitalWrite(LED_BUILTIN, HIGH);  
      lastInjectorOnTime = t;
    }
  } else {
    if(!digitalRead(BTN_PIN)) {
      digitalWrite(INJECTORS_PIN, HIGH);  
      digitalWrite(LED_BUILTIN, HIGH);  
    } else {
      digitalWrite(INJECTORS_PIN, LOW);   
      digitalWrite(LED_BUILTIN, LOW);   
    }
  }

  enc1.tick();

  if(menu==0) {
    if(timerStarted) {
      if(t-lastTimeRefresh >= 1000) {
        displayInt(int(0.001*(timer - t + timerStarted)));
        lastTimeRefresh = t;
      }
    }
  }

  if (enc1.isClick()) {
    if(menu==0) {
      if(!timerStarted) {
        timerStarted = t;
        started = true;
      } else {
        timerStarted = 0;
        started = false;
      }
    } else {
      menuEntered=!menuEntered; 
    }
    refreshDisplay();
  }

  if(!menuEntered) {
    if (enc1.isLeft()) {if(menu>0) {menu--;} refreshDisplay();}
    if (enc1.isRight()) {if(menu<4) {menu++;} refreshDisplay();}
  } else {
    d=0;
    if(enc1.isLeft()) d=-1;
    if(enc1.isFastL()) d=-10;
    if(enc1.isRight()) d=1;
    if(enc1.isFastR()) d=10;

    if(d) {
      if(menu==1) {
        setRpm(d);
      };
      if(menu==2) {
        sett1(d);
      };
      if(menu==3) {
        setT(d);
      };
      if(menu==4) {
        setTimer(d);
      };
      refreshDisplay();
    }
  }


}

void setRpm(int d = 0) {
  if(d<0 && Rpm+d <= 0) return;
  if(d>0 && Rpm+d > 20000) return;
  Rpm = Rpm + d;
  T = int(240000/Rpm);
}

void sett1(int d = 0) {
  if(d<0 && t1+d <= 0) return;
  if(d>0 && t1+d > T) return;
  t1 = t1 + d;
}

void setT(int d = 0) {
  if(d<0 && T-d < t1) return;
  T = T + d;
  Rpm = int(240000/T);
}

void setTimer(int d = 0) {
  if(d<0 && timer+d <= 0) return;
  if(d>0 && timer+d > 3600000) return;
  timer = timer + d*1000;
}


void refreshDisplay() {
  if(!menuEntered) {
    if(menu==0 && !timerStarted) displayWord(_S,_t,_r,_t);
    if(menu==1) displayWord(_r,_P,_empty,_empty);
    if(menu==2) displayWord(_t,_l,_empty,_empty);
    if(menu==3) displayWord(_t,_empty,_empty,_empty);
    if(menu==4) displayWord(_d,_u,_r,_a);
  } else {
    if(menu==1) displayInt(Rpm);
    if(menu==2) displayInt(t1);
    if(menu==3) displayInt(T);
    if(menu==4) displayInt(0.001*timer);
  }
}


void displayCurRacer(int unsigned number) {
  uint8_t data[] = { _r, 0x00, 0x00, 0x00 }; 
  data[1] = display.encodeDigit(int(number/100)-10*int(number/1000));
  data[2] = display.encodeDigit(int(number/10)-10*int(number/100));
  data[3] = display.encodeDigit(int(number)-10*int(number/10));
  display.setSegments(data, 4, 0);
}

void displayInt(int unsigned number) {
  uint8_t data[] = { 0x00, 0x00, 0x00, 0x00 }; 
  data[0] = display.encodeDigit(int(number/1000)-10*int(number/10000));
  data[1] = display.encodeDigit(int(number/100)-10*int(number/1000));
  data[2] = display.encodeDigit(int(number/10)-10*int(number/100));
  data[3] = display.encodeDigit(int(number)-10*int(number/10));
  display.setSegments(data, 4, 0);
}

void displayWord(uint8_t d1,uint8_t d2,uint8_t d3,uint8_t d4) {
  uint8_t data[] = {d1,d2,d3,d4}; 
  display.setSegments(data);
}
