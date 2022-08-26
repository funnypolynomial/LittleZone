#pragma once
// Large version uses 320x480 LCD Shield, no touch, ILI948x interface

#include "ILI948x.h"
// The LCD interface
// Initialise
#define LCD_INIT() { ILI948x::Init();ILI948x::DisplayOn(); }
// Define a window to fill with pixels at (_x,_y) width _w, height _h
// Returns the number of pixels to fill (unsigned long)
#define LCD_BEGIN_FILL(_x,_y,_w,_h) ILI948x::Window(_x,_y,_w,_h)
// Sends _sizeUL (unsigned long) pixels of the 16-bit colour
#define LCD_FILL_COLOUR(_sizeUL, _colorWord) ILI948x::ColourWord( _colorWord, _sizeUL)
// Sends _sizeUL (unsigned long) pixels of the 8-bit colour.
// The byte is duplicated, 0xFF and 0x00 really only make sense. Slightly faster than above.
#define LCD_FILL_BYTE(_sizeUL, _colorByte) ILI948x::ColourByte(_colorByte, _sizeUL)
// Sends a single white pixel
#define LCD_ONE_WHITE() ILI948x::OneWhite()
// Sends a single black pixel
#define LCD_ONE_BLACK() ILI948x::OneBlack()
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
