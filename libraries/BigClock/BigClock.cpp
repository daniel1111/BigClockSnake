/*
  BigClock.h - Library for driving station BigClocks
 *
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

#include "Arduino.h"
#include "BigClock.h"
#include "../SPI/SPI.h"
#include "../Timer1/TimerOne.h"

static BigClock *bc;
    
BigClock::BigClock()
{
  latchPin = 4;
  woutPin = 6;
}

void BigClock::init()
{
  b1 = 0;
  bflg = 0;
  pinMode(BigClock::latchPin, OUTPUT);
  pinMode(woutPin, OUTPUT);  
  pinMode(PIN_BOARDSEL, OUTPUT);  

  digitalWrite(PIN_BOARDSEL, LOW);
  
  SPI.begin();
  SPI.setDataMode(SPI_MODE1);
  SPI.setClockDivider(SPI_CLOCK_DIV4); // 8 is closest to output from original clock controller
  SPI.setBitOrder(LSBFIRST);
  Timer1.initialize(4004); // 4ms   
  
  bc = this;
  
  Timer1.attachInterrupt(BigClock::sCallback);
  delay(100);

  // Init vaiables
  t=0;
  bcount = 0;
  by = 0;  
  c=0;   
}

void BigClock::sCallback()
{
  bc->callback();  
}

void BigClock::callback()
{
 
  byte c;
  
  c = digitalRead(woutPin);
  
  if (c)
  {
    if (bflg>=3)
    {
      digitalWrite(latchPin, LOW);
      delayMicroseconds(16); 
      digitalWrite(latchPin, HIGH);
      bflg = 0; 
    }
       
    digitalWrite(woutPin, LOW);
  } else
    digitalWrite(woutPin, HIGH);
    
  if (bflg)
    bflg++;  
 
}

void BigClock::write_sbit(bool b)
{
  by |= (b  << bcount++);
  
  if (bcount >= 8)
  {
    SPI.transfer(by);
    by = 0;
    bcount = 0;
  }
}

void BigClock::flush_sbit()
{
  if (bcount)
  {
    SPI.transfer(by);
    by = 0;
    bcount = 0;  
  }
}


bool BigClock::get_bit(byte *fb, int x, int y)
/* x = 0-95
 * y = 0-25
 */
{
  if (y < 13) 
  {
    x = 95 - x;  
    y = 12 - y;
  }
  
  return fb[((x/8)*MAX_Y) + y]  & (1 << (x%8));
}

void BigClock::output_segment(int board, byte *framebuf, bool odd_lines, int segment, int row_start, bool tst) // output 6x13 segment (0-7)
{  
  int row, col, j;
  row = row_start;
  col = 95-(segment*6);
  row += 2;

  for (j=((segment%2) ? 0 : 1); j < (8*5) + ((segment%2) ? 0 : 1); j++)
  {
    if (odd_lines)
    {   
      if (j)
        write_sbit(get_bit(framebuf, col, row));
      else
        write_sbit(get_bit(framebuf, col+1, board ? 12 : 25));
    } else
    {
      if (j)
      {
        if (  (j%13)  && (row==1) && board)
        { 
          write_sbit(get_bit(framebuf, col+1, (row-2 == -1) ? row - 1 : row - 2));
        }
        else if (  (j%13)  && (row==14) && !board)
        {
          write_sbit(get_bit(framebuf, col+1, (row-2 == 12) ? row - 1 : row - 2));
        } else
        {
          write_sbit(get_bit(framebuf, col, (row-2 == 12) ? row - 1 : row - 2)); 
        }
      }
      else 
        write_sbit(get_bit(framebuf, col+1, board ? 11 : 24));       
    }
  
    if ( (j==6) || (j==13) || (j==19) || (j==26) || (j==32) )
    {
      col--;
    }    

    if (!((j-6)%13))
    {
      row = row_start;   
    } else if (!(j%13))
    {
      row = row_start + 2;
    } else
      row+=2;
  }
 
  if (tst)
  {
    write_sbit(false);    
    write_sbit(false);
    write_sbit(false);
    write_sbit(false);
    write_sbit(false);
    write_sbit(false);
    write_sbit(false);
    write_sbit(false);    
  } else  
  if (odd_lines)
  {
    write_sbit(true);    
    write_sbit(false);
    write_sbit(false);
    write_sbit(false);
    write_sbit(false);
    write_sbit(false); // all white
    write_sbit(false);
    write_sbit(false);
  } else
  {
    write_sbit(false);      
    write_sbit(false);
    write_sbit(false);
    write_sbit(false);
    write_sbit(false);// <<<<<< true=flash on change ?
    write_sbit(false);
    write_sbit(false);
    write_sbit(false);  
  }     
}

void BigClock::output(byte *framebuffer)
{
  output_board(BOARD_TOP, framebuffer);
  output_board(BOARD_BOTTOM, framebuffer);
}

void BigClock::output_board(int board, byte *framebuffer)
{
  if (board)
    digitalWrite(PIN_BOARDSEL, LOW);
  else
    digitalWrite(PIN_BOARDSEL, HIGH);
    
  // Output odd lines for right side of display (numbering lines 0-12)
  for (int n=0; n <8; n++)
  {
      output_segment(board, framebuffer, false, n, board ? 1 : 14, false);
  }      

 // Output odd lines for left side of display    
  for (int n=8; n <16; n++)
  {
      output_segment(board, framebuffer, true, n, board ? 0 : 13, false);
  }
 
  delayMicroseconds(20); 
  digitalWrite(latchPin, LOW);
  delayMicroseconds(400); 
  digitalWrite(latchPin, HIGH);
  delayMicroseconds(120);   
  
  for (int n=0; n <8; n++)
  {
    output_segment(board, framebuffer, true, n, board ? 0 : 13, false);    
  }
 
  for (int n=8; n <16; n++)
  {
    output_segment(board, framebuffer, false, n, board ? 1 : 14, true);    
  }  

  bflg = 1; 
  delay(25);  
}

