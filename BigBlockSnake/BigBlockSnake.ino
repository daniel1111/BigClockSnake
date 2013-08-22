#include <Snake.h>
#include <BigClock.h>
#include "SPI.h"
#include "TimerOne.h"

/*
 * Screen is 96x26 pixels, stored in a 12byte (=96 bits) x 26 byte array
 */ 

byte _framebuf[MAX_X][MAX_Y]; 
BigClock _bc;
Snake *_sn;
byte _dir;

enum {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};

void setup()
{
  _bc.init(); // Init BigClock/SPI, etc
  _sn = new Snake(MAX_X*8, MAX_Y, &_framebuf[0][0], sizeof(_framebuf));
  Serial.begin(9600);
  
  pinMode(A0, INPUT);	   
  pinMode(A1, INPUT);	   
  pinMode(A2, INPUT);	   
  pinMode(A3, INPUT);	   
  digitalWrite(A0, HIGH);  
  digitalWrite(A1, HIGH);  
  digitalWrite(A2, HIGH);  
  digitalWrite(A3, HIGH);  
  PCICR = 0x02;          // Enable PCINT1 interrupt
  PCMSK1 = 0b00001111;  
}

void loop()
{
  _sn->tick();
  _sn->render();
  _bc.output(&_framebuf[0][0]);
 // delay(100);
  
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    
    switch (inChar)
    {
      case 'w':
      case 'W':
        _sn->dir_down();
        break;
        
      case 's':
      case 'S':
        _sn->dir_up();
        break;
        
      case 'a':
      case 'A':
        _sn->dir_left();
        break;
        
      case 'd':
      case 'D':
        _sn->dir_right();
        break;
    }
  }
}


ISR(PCINT1_vect) {    // Interrupt service routine. Every single PCINT8..14 (=ADC0..5) change
                      // will generate an interrupt: but this will always be the same interrupt routine
  if (digitalRead(A0)==0) _sn->dir_up();
  if (digitalRead(A1)==0) _sn->dir_down();
  if (digitalRead(A2)==0) _sn->dir_left();
  if (digitalRead(A3)==0) _sn->dir_right();
}


