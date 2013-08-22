#include <BigClock.h>
#include "SPI.h"
#include "TimerOne.h"

/* Scroll H logo across screen */

/*
 * Screen is 96x26 pixels, stored in a 12byte (=96 bits) x 26byte array
 */ 

byte framebuf[MAX_X][MAX_Y]; 
BigClock bc;

byte t;
extern const unsigned char GIMP_IMAGE_pixel_data[26 * 26 * 3 + 1];

void setup()
{
  bc.init(); // Init BigClock/SPI, etc
  t = 0;
}

void loop()
{
  setbuf();
  bc.output(&framebuf[0][0]);
  delay(150);
  
  if (++t >= 96)
    t = 1; // Advance logo position on screen
}

void setbuf()
{
  // Clear buffer
  memset(framebuf, 0x00, sizeof(framebuf));

  // Copy in image from progmem
  for (int x = 0; x < 96; x++) 
    for (int y = 0; y < 26; y++)
    {
      if (getxy(x, y))
      {
        framebuf[x/8][y] |= 1 << (x%8);
      }       
    } 
}

boolean getxy(int x, int y)
{
  x = x - t;
  if ((x > 25) || (x < 0))
    return false;
  
  if (pgm_read_byte_near(GIMP_IMAGE_pixel_data + (3 * ((y*26) + x))) == 0xFF)
    return false;
  else
    return true; 
}

