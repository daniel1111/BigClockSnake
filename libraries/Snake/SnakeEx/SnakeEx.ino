#include <Snake.h>

/* Snake example. Ouputs an 16x8 snake game over serial. Up/Down/Left/Right is W/S/A/D, to restart, reset the Arduino.
 * Works best from gtkterm (Linux) and presumably hyperterminal (Windows). Still not particulary playable though as this
 * is really meant for matrix displays.  
 */
 
/*
 * Set array size to hold screen state. As 1 bit, not 1 byte, is stored per pixel, MAX_X is the width of the display divided by 8. 
 * MAX_Y is the height.
 * 
 * Generate a 16x8 game:
 */ 
#define MAX_X 2
#define MAX_Y 8

byte _framebuf[MAX_X][MAX_Y]; 
Snake *_sn;

void setup()
{
  _sn = new Snake(MAX_X*8, MAX_Y, &_framebuf[0][0], sizeof(_framebuf));
  Serial.begin(9600);
}

void loop()
{
  _sn->tick();    // advance game state
  _sn->render();  // write to _framebuf
  output();       // output _framebuf over serial
  delay(1000);
  
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    
    switch (inChar)
    {
      case 'w':
      case 'W':
        _sn->dir_up();
        break;
        
      case 's':
      case 'S':
        _sn->dir_down();
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

void output()
{
  /* Output game image from _framebuf over serial. */
  for (int y=0; y < MAX_Y; y++)
  {
    for (int x=0; x < MAX_X*8; x++)
    {
      if ((_framebuf[x/8][y]) & ( 1 << (x%8))) 
        Serial.print("X");
      else
        Serial.print("-");
    }
    Serial.print("\n");
  } 
}
