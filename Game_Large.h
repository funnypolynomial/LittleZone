#pragma once
// Large version uses 320x480 LCD Shield, no touch, ILI948x interface

#include "LCD.h"
// The LCD interface
// Initialise
#define LCD_INIT() lcd.init();
// Define a window to fill with pixels at (_x,_y) width _w, height _h
// Returns the number of pixels to fill (unsigned long)
#define LCD_BEGIN_FILL(_x,_y,_w,_h) lcd.beginFill(_x,_y,_w,_h)
// Sends _sizeUL (unsigned long) pixels of the 16-bit colour
#define LCD_FILL_COLOUR(_sizeUL, _colorWord) lcd.fillColour(_sizeUL, _colorWord)
// Sends _sizeUL (unsigned long) pixels of the 8-bit colour.
// The byte is duplicated, 0xFF and 0x00 really only make sense. Slightly faster than above.
#define LCD_FILL_BYTE(_sizeUL, _colorByte) lcd.fillByte(_sizeUL, _colorByte)
// Sends a single white pixel
#define LCD_ONE_WHITE() lcd.fillByte(1, 0xFF)
// Sends a single black pixel
#define LCD_ONE_BLACK() lcd.fillByte(1, 0x00)
// True if there is a touch. Returns position in (int) _x, _y
#define LCD_GET_TOUCH(_x, _y) false


// The whole LCD
#define LCD_WIDTH  480
#define LCD_HEIGHT 320

// The Game screen
// TOO BIG!
#ifdef SCREEN_FULL_SIZE
#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 320
#define SCREEN_SCALE_NUM  4166  // 320/768 = 0.4166
#define SCREEN_SCALE_DEN 10000
#else
#define SCREEN_WIDTH  255
#define SCREEN_HEIGHT 192
#define SCREEN_SCALE_NUM 1
#define SCREEN_SCALE_DEN 4
#endif

#define SCREEN_OFFSET_X ((LCD_WIDTH - SCREEN_WIDTH)/2)
#define SCREEN_OFFSET_Y ((LCD_HEIGHT - SCREEN_HEIGHT)/2)