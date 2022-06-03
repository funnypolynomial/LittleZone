#include <Arduino.h>
#include "Pins.h"
#include "BTN.h"
#include "RTC.h"
#include "LCD.h"
#include "MathBox.h"
#include "Shapes.h"
#include "Sparse.h"
#include "SparseXL.h"
#include "Game.h"
// Libraries:
#include "SoftwareI2C.h"


//                                         LittleZone
// A toy-like approximation of 1980's Battlezone game's "Attract" (demo) mode.  Can show the time, so a desk accessory perhaps.
// It started when this: https://spectrum.ieee.org/battlezone popped up on my Google feed in Feb '22. 
// It occurred to me I could implement something using my Sparse algorithm from ElitePetite.
// The screen proportions are different -- the active screen is much wider, so I had to tweak the algorithm to support more than 255 columns (SparseXL)
//
// This would not have been possible without the great writeup and resources (including full disassembly) at https://6502disassembly.com/va-battlezone/
// Also, the implementation of the gnarly "MathBox" from Mame, https://github.com/mamedev/mame/blob/master/src/mame/machine/mathbox.cpp
// That code is
////   license:BSD-3-Clause
////   copyright-holders:Eric Smith
// I have modified that code and used it here.
// This is otherwise original code, interpreting the original 6502 code and guided by the 6502disassembly site.  
// I have used the original ROM data from there for the obstacle and enemy shapes and for the text glyphs.
// I relied on some YouTube videos to get a sense of the demo mode, for example https://youtu.be/fgPUGZwAwJQ & https://youtu.be/0hsGS11dTPU


// What it does:
//  The screen is divided into an upper red area and a lower green area.
//  The upper area includes status messages on the left (like "ENEMY IN RANGE"), a radar screen in the middle and socres/lives on the right.
//  The larger green area shows the battlezone, with a moon rising over a distant mountain-range and a plane in the foreground littered with
//  geometric objects and a single Enemy (one of two tanks, a missile, saucer or clock). A targeting reticle is overlaid on the scene.
//  The simulation starts with the BATTLEZONE banner crawl then runs the "demo" animation.  
//  This consists of
//    1:Pausing then
//    2:Turning left or right then
//    3:Moving forward then
//    4:Back to Pausing
//  Randomly when paused, the similation may show the Banner, High Scores or Game Over.
//  Showing the banner may also choose a random enemy (a tank, missile, saucer or clock).
//
// Clock
//  If an RTC is present the simulation can show the time as
//    analog hands on the radar screen
//    digits in the High Score
//    an "enemy" 
//
// What's not implemented:
//  Radar sweep
//  Objects on the radar
//  Lava spewing from the volcano (and volcano mouth details removed)
//  Tank tracks
//  Tank radar dish
//  Collision avoidance
//  Sounds
//
// What's different:
//  The text is a little larger (this is configurable)
//  There are only three intensity levels, represented by solid or dotted lines
//  Both reticles and the background are low intensity, obstacles are intermediate intensity, enemies are high intensity (except the saucer!).
//  The random motion is not the same as the original game
//  The Bannner/Press Start/High Scores sequence is not as in the original
//  To reduce the sparse pixel memory required for the saucer, its near clipping plane is further away and it is drawn dotted
//
// Customization:
//  Refer to the ENABLE_ and OPTION_ defines in Game.h to configure time display, scores etc.
//  Refer to RTC_I2C_ADDRESS in RTC.h if no RTC is present
//
// Configuration:
//  Touch: 
//    With a touch screen, tapping in the red area configures the time (or is ignored).
//    The time is shown as
//      HH
//      MM
//      SET
//    Tapping on anywhere in line with the hour or minute increments it (and makes it current, blinking).
//    Tapping SET sets the time, tapping in the green area exist without setting the time.
//    Tapping the green area switches to MANUAL mode and dragging the touch point turns left/right and
//    moves forward/backward. Tapping the red area exits the mode.
//  Buttons: 
//    With push-buttons, pressing SET sets the time (as above). ADJ increments the current value, SET advances to the next field.
//    Pressing ADJ kicks off Banner, High Scores or Game Over.
//
// LCD:
//  The Game's interface to an LCD is defined by the 7 LCD_ macros in Game_<size>.h
//
// Notes:
// "Low memory available, stability problems may occur."
//   This compiles for a Uno with the dire warning above. But see DEBUG_STACK_CHECK. 
//   This reports the headroom after local variables and return addresses are pushed on the stack.
//   Relatively simple to do since the main loop is basically the same.  
//   It shows of the ~400 bytes available (after globals including SparseXL::pool consume most of the 2048 bytes),
//   only ~150 are needed on the stack.

#ifdef DEBUG_STACK_CHECK
// see, for example, https://www.avrfreaks.net/forum/soft-c-avrgcc-monitoring-stack-usage
extern uint8_t _end;
uint8_t stackFill = 0;
uint16_t stackHeadroom = 9999;
void StackPaint()
{
  // Fill bytes from the top of dynamic variables up to the top of our stack
  uint8_t* ptr = &_end;
  stackFill++;  // different value each time
  while (ptr < (uint8_t*)&ptr)
  {
    *ptr = stackFill;
    ptr++;
  }
} 

void StackCheck()
{
  // Check bytes from the top of dynamic variables up to the top of our stack
  // Updates stackHeadroom
  const uint8_t *ptr = &_end;
  uint16_t       ctr = 0;
  while (*ptr == stackFill && ptr < (uint8_t*)&ptr)
  {
    ptr++;
    ctr++;
  }
  if (ctr < stackHeadroom)
    stackHeadroom = ctr;
}
#else
#define StackPaint()
#define StackCheck()
#endif

bool full = true;
void setup() 
{
#ifdef DEBUG  
  Serial.begin(38400);
  Serial.println("LittleZone");
#endif
  rtc.Setup();
#if defined(PIN_BTN_SET) && defined(PIN_BTN_ADJ)
  btn1Set.Init(PIN_BTN_SET);
  btn2Adj.Init(PIN_BTN_ADJ);
#endif 
  LCD_INIT();
  lcd.fillByte(lcd.beginFill(0, 0, LCD_WIDTH, LCD_HEIGHT), 0x00);
  MathBox::Init();
  Shapes::Init();
  Game::Init();
}

void loop() 
{
  StackPaint();
  Game::CheckInteraction();
  Game::AnimationStep();
  Game::Draw(full);
  full = false;
  StackCheck();

}
