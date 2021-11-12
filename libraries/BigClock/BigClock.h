/*
  BigClock.h - Library for driving station BigClocks
*/
#ifndef BigClock_h
#define BigClock_h

#include "Arduino.h"
#include "SPI.h"
#include "TimerOne.h"
#include <avr/pgmspace.h>

#define MAX_X 12
#define MAX_Y 26

#define BOARD_TOP 0
#define BOARD_BOTTOM 1
#define PIN_BOARDSEL 2 

class BigClock
{
  public:
    BigClock();
    void init();
    void output(byte *framebuffer);
    static void sCallback();
    void callback();
    int latchPin;
    int woutPin;


    
  private:

    void write_sbit(bool b);
    void flush_sbit();
    bool get_bit(byte *fb, int x, int y);
    void output_segment(int board, byte *framebuf, bool odd_lines, int segment, int row_start, bool tst); // output 6x13 segment (0-7)
    void output_board(int board, byte *framebuffer);
    

    byte t;
    volatile byte bflg;
    byte b1;
    short bcount;
    byte by;
    short int c;
};

#endif