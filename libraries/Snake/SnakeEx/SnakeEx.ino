#include <Snake.h>

/* Snake example. Ouputs an 8x8 snake game over serial. Up/Down/Left/Right is W/S/A/D, to restart, reset the Arduino.
 * Works best from gtkterm (Linux) and presumably hyperterminal (Windows). Still not particulary playable though as this
 * is really meant for matrix displays.  
 */

#define MAX_X 16
#define MAX_Y 16

byte _framebuf[MAX_X][MAX_Y]; 
Snake *_sn;

void setup()
{
  _sn = new Snake(MAX_X, MAX_Y, set_xy);
  Serial.begin(9600);
}

void loop()
{
  memset(_framebuf, 0, sizeof(_framebuf)); // wipe buffer
  _sn->tick();    // advance game state
  _sn->render();  // call set_xy function to draw 
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

/* Used by Snake class when render() called */
void set_xy(byte x, byte y, byte val)
{
  _framebuf[x][y] = val;
}

void output()
{  
  /* Output game image from _framebuf over serial. */
  for (int y=0; y < MAX_Y; y++)
  {
    for (int x=0; x < MAX_X; x++)
    {
      if (_framebuf[x][y])
        Serial.print("X");
      else
        Serial.print("-");
    }
    Serial.print("\n");
  } 
}