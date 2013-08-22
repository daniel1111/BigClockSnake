/*
 * Copyright (c) 2013, Daniel Swann <hs@dswann.co.uk>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the owner nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "Snake.h"
#include <string.h>
#include <Arduino.h>

Snake::Snake(byte max_x, byte max_y, byte *framebuffer, short fb_size)
{
  _max_x = max_x;
  _max_y = max_y;

  // Start with the snake in the middle
  _head_x = max_x/2;
  _head_y = max_y/2;
  _fb = framebuffer;

  _dir = EAST;

  memset(_ele, 0, SNAKE_ARRAY_SIZE);
  _snake_len = 1; // Initial snake length
  _game_over = false;
  _fb_size = fb_size;
  _food_x = 0;
  _food_y = 0;
  _food_timer = 0;

  randomSeed(analogRead(5));
}

void Snake::set_dir(direction dir)
{
  // Don't allow switching to opposite direction... would be suicide
  if (dir != (~(_dir) & 3))
    _dir = dir;
}

void Snake::tick()
{
  direction mov_dir;
  byte x;
  byte y;
  byte new_sid;

  if (_game_over)
    return;

  if (_food_timer == 0)  // time to spawn food
  {
    _food_x = random(_max_x-2)+1; // don't spawn on border
    _food_y = random(_max_y-2)+1;
  }

  // move head
  next_pos(_dir, &_head_x, &_head_y);

  // Test for hitting edge
  if 
    (
      (_head_x == 0)        ||
      (_head_x == _max_x-1) ||
      (_head_y == 0)        ||
      (_head_y == _max_y-1)
    )
  {
    _game_over = true;
    return;
  }

  // Test for eating food
  if (_food_x != 0)
  {
    if ((_head_x == _food_x) &&
        (_head_y == _food_y))
    {
      if (_food_timer == 0)
      {
         // food spawned on the new location of the head... try spawning again next tick
        _food_x = 0;
        _food_y = 0;
      } else  
      {
        // Food eaten!
        _food_timer = 0;
        _food_x = 0;
        _food_y = 0;
        if (_snake_len < (SNAKE_ARRAY_SIZE*8)) _snake_len++;
      }
    }
  }

  x = _head_x;
  y = _head_y;

  // Move tail. Well, recalc how each element links to the next
  mov_dir = _dir;
  for (int i=0; i < _snake_len; i++)
  {
    next_pos(mov_dir, &x, &y);

    // Lookout for heading touching tail
    if ((x==_head_x) && (y==_head_y))
      _game_over = true;

    // Check food just spawned isn't on the snake
    if (x==_food_x && y==_food_y && _food_x > 0 && _food_y > 0 && _food_timer == 0)
    {
      _food_x = 0;
      _food_y = 0;
      _food_timer = 0;
    }

    mov(mov_dir, get_element(i), &mov_dir, &new_sid);
    set_element(i, new_sid);
  }

  if (_food_x!=0)
    _food_timer++;

  if (_food_timer > FOOD_TIMEOUT)
  {
    _food_timer = 0;
    _food_x = 0;
    _food_y = 0;
  }
}

void Snake::mov(direction mov_dir, byte con_sid, direction *new_dir, byte *new_sid)
{
  *new_sid = mov_dir;
  *new_dir = (direction)con_sid;
}

void Snake::next_pos(direction dir, byte *x, byte *y)
{
  switch (dir)
  {
    case NORTH:
      (*y)--;
      break;

    case EAST:
      (*x)++;
      break;

    case SOUTH:
      (*y)++;
      break;

    case WEST:
      (*x)--;
      break;
  }
}

void Snake::render()
{
  byte x = _head_x;
  byte y = _head_y;
  byte dir;

  // Wipe framebuffer
  memset(_fb, (_game_over ? 0xff : 0x00), _fb_size);

 // Draw head
 set_xy(x, y, 1);

 // Draw body
 for (int i=0; i < _snake_len; i++)
 {
   dir = ~(get_element(i)) & 3;

   next_pos((direction)dir, &x, &y);
   set_xy(x, y, 1);
 }

 // Draw border
 line_h(0       , 0, _max_x-1);
 line_h(_max_y-1, 0, _max_x-1);
 line_v(0       , 0, _max_y-1);
 line_v(_max_x-1, 0, _max_y-1);
 
 // Draw food
 if (_food_x > 0 && _food_y > 0)
   set_xy(_food_x, _food_y, 1); 
}

void Snake::line_h(byte y, byte start_x, byte end_x)
{
  for (byte x=start_x; x <= end_x; x++)
    set_xy(x, y, 1);
}

void Snake::line_v(byte x, byte start_y, byte end_y)
{
  for (byte y=start_y; y <= end_y; y++)
    set_xy(x, y, 1);
}

void Snake::set_xy(byte x, byte y, byte val)
// set/clear position x,y in the framebuffer 
{
  if (_game_over)
    val = !val;

  byte *a=  &_fb[((x/8)*_max_y) + y];

  if (val)
    *a |= (1 << x%8);
  else
    *a &= ~(1 << x%8);
}

byte Snake::get_element(short n)
// get the two bits from _ele that releate to element n
{

  byte a = _ele[n/4] & (3 << (n%4)*2);
  a >>= (n%4)*2;

  return a;
}

void Snake::set_element(byte n, byte val)
{
  byte a = _ele[n/4];

  a &= ~(3 << (n%4)*2); // zero the relevant two bits 
  a |= val << (n%4)*2;  // set new val

  _ele[n/4] = a;
}
