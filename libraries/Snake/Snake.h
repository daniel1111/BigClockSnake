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

typedef unsigned char byte;

#define SNAKE_ARRAY_SIZE 200 // Snake length can at most be 1+ 4x this value
#define FOOD_TIMEOUT 50      // How long to show food for

class Snake
{
  public:
    Snake(byte max_x, byte max_y, byte *framebuffer, short _fb_size);
    void dir_up()     { set_dir(NORTH); };
    void dir_down()   { set_dir(SOUTH); };
    void dir_left()   { set_dir(WEST);  }; 
    void dir_right()  { set_dir(EAST);  };
    void tick();
    void render();
    byte game_over()  {return _game_over;}

  private:
    enum direction {NORTH=0, EAST=1, SOUTH=3, WEST=2}; // numbered such that the inverse is the opposite direction

    byte _food_x;
    byte _food_y;
    byte _food_timer; /* Counts up on each tick. 0 = spawn, 1 to FOOD_TIMEOUT = show food  */
    byte _max_x;
    byte _max_y;
    byte _head_x;
    byte _head_y;
    direction _dir;
    short _fb_size;
    short _snake_len;
    byte _ele[SNAKE_ARRAY_SIZE]; // snake - 2bits per element (not including head)
    byte *_fb;
    bool _game_over;

    void set_xy(byte x, byte y, byte val);
    void next_pos(direction dir, byte *x, byte *y);
    void mov(direction mov_dir, byte con_sid, direction *new_dir, byte *new_sid);
    void line_h(byte y, byte start_x, byte end_x);
    void line_v(byte x, byte start_y, byte end_y);
    void set_dir(direction dir);
    byte get_element(short n);
    void set_element(byte n, byte val);
};