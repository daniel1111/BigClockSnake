#include <Snake.h>
#include <BigClock.h>
#include "SPI.h"
#include "TimerOne.h"

/*
 * Screen is 96x26 pixels, stored in a 12byte (=96 bits) x 26 byte array
 * 
 * Two player snake - games are displayed side-by-side on the LCD
 */ 

byte _framebuf[MAX_X][MAX_Y]; 
BigClock _bc;
Snake *_sn1;
Snake *_sn2;
byte _dir;
byte _game_over;

enum {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};

void setup()
{
  _bc.init(); // Init BigClock/SPI, etc
  _sn1 = new Snake(MAX_X*4, MAX_Y, set_xy1);
  _sn2 = new Snake(MAX_X*4, MAX_Y, set_xy2);  
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

/* Used by Snake class when render() called */
void set_xy1(byte x, byte y, byte val)
{
  if (_sn1->game_over())
    val = !val;
  byte *a=  &_framebuf[x/8][y];

  if (val)
    *a |= (1 << x%8);
  else
    *a &= ~(1 << x%8);  
}

void set_xy2(byte x, byte y, byte val)
{
  if (_sn2->game_over())
    val = !val;
    
  x+= (MAX_X*8) / 2;

  byte *a=  &_framebuf[x/8][y];

  if (val)
    *a |= (1 << x%8);
  else
    *a &= ~(1 << x%8);  
}

void loop()
{
  _sn1->tick();
  _sn2->tick();

  for (int x=0; x < MAX_X/2; x++)
    memset(_framebuf[x], (_sn1->game_over() ? 0xFF : 0x00), sizeof(_framebuf[x]));
    
  for (int x=MAX_X/2; x < MAX_X; x++)
    memset(_framebuf[x], (_sn2->game_over() ? 0xFF : 0x00), sizeof(_framebuf[x]));
    
  _sn1->render();
  _sn2->render();
  _bc.output(&_framebuf[0][0]);
  
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    
    switch (inChar)
    {
      case 'w':
      case 'W':
        _sn1->dir_up();
        break;
        
      case 's':
      case 'S':
        _sn1->dir_down();
        break;
         
      case 'a':
      case 'A':
        _sn1->dir_left();
        break;
        
      case 'd':
      case 'D':
        _sn1->dir_right();

      case 'i':
      case 'I':
        _sn2->dir_up();
        break;

      case 'k':
      case 'K':
        _sn2->dir_down();
        break;

      case 'j':
      case 'J':
        _sn2->dir_left();
        break;

      case 'l':
      case 'L':
        _sn2->dir_right();
        break;
    }
  }
}
/*

ISR(PCINT1_vect) {    // Interrupt service routine. Every single PCINT8..14 (=ADC0..5) change
                      // will generate an interrupt: but this will always be the same interrupt routine
  if (digitalRead(A0)==0) _sn->dir_up();
  if (digitalRead(A1)==0) _sn->dir_down();
  if (digitalRead(A2)==0) _sn->dir_left();
  if (digitalRead(A3)==0) _sn->dir_right();
}

*/
