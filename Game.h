#pragma once

// Switch to larger Sparse algorithm and go larger than 256 pixels
#define SCREEN_FULL_SIZE
// Draw the lower (green) part in two passes (requiring less Sparse data)
#define SCREEN_SPLIT

// Switch between Small (240x320 Touch) and Large (320x480) LCD's
#include "Game_Small.h"
//#include "Game_Large.h"

#ifdef SCREEN_FULL_SIZE
#define SPARSE SparseXL
#else
#define SPARSE Sparse
#endif
#define SCREEN_UPPER_HEIGHT   SCREEN_HEIGHT/5  // the top 1/5 of the screen (red)
#define SCREEN_LOWER_HEIGHT 4*SCREEN_HEIGHT/5  // the bottom 4/5 of the screen (green)

// Configuration
#define ENABLE_TIME_SCORE           // Show Time as the Score (Digital) {#}
#define ENABLE_TIME_RADAR           // Show Time as the Radar (Analog) {#}
#define ENABLE_TIME_ENEMY           // Show Time as the Enemy {#}
#define ENABLE_TIME_24HOUR          // Digital time is 24-hour (vs 12-hour) {#}
#define ENABLE_LARGER_TEXT          // Make text more legible (eg Score)
#define ENABLE_STATUS_MSGS          // Show ENEMY TO LEFT etc (slows framerate)
//#define ENABLE_FLASH_MSGS           // If ENABLE_STATUS_MSGS then ALSO flash them (disturbs framerate)
#define ENABLE_RANDOMIZE            // Seed the PRNG with the time at startup {#}
// If defined, enemy does not change randomly. Use Shapes::tank1, Shapes::tank2, Shapes::missile, Shapes::saucer or Shapes::clock {#}
//#define ENABLE_FIXED_ENEMY Shapes::clock

// {#}: N/A if there is no RTC connected

// 7-digit Score used when not ENABLE_TIME_SCORE
#define OPTION_SCORE      "   1000"
// 7-digit High Score
#define OPTION_HIGH_SCORE "   5000"

#define M_STR(_s) _s "\0"
// The list of high scores and intials (multi-string, see Game::HiScoreListM_STR)
// If the last char is a '!' a tank icon will be appended
//          HIGH SCORES
//         XXXX000 AAA
#define OPTION_HIGH_SCORE_LIST   \
    M_STR("1000000 MEW!")        \
    M_STR(" 900000 TAY ")        \
    M_STR(" 800000 LDA ")        \
    M_STR(" 700000 ADC ")        \
    M_STR(" 600000 CLC ")        \
    M_STR(" 500000 PLP ")        \
    M_STR(" 400000 ASL ")        \
    M_STR(" 300000 ROR ")        \
    M_STR(" 200000 SEC ")        \
    M_STR(" 100000 BCC ")

// Debugging, Serial, stats etc
//#define DEBUG_STACK_CHECK          // Check that there's enough stack. Reported in Score, currently ~250 bytes unused.
//#define DEBUG
#ifdef DEBUG
// Dump variable to serial
#define DBG(_x) { Serial.print(#_x);Serial.print(":");Serial.println(_x); }
#else
#define DBG(_x)
#endif

namespace Game
{
    void Init();
    void CheckInteraction();
    void AnimationStep();
    void Draw(bool full);

    void RotatePlayer(int16_t degrees);
    void MovePlayer(int16_t distance);

    void DrawClockObject(int16_t* screenCoords);

    // Game variables
    // player position, double coord letters denote Battlezone's unusual frame of reference
    extern int16_t unit_pos_ZZ; // INTO screen
    extern int16_t unit_pos_XX; // screen LEFT
    // player orientation angle
    // note that our plyr_facing is NOT "a 9-bit value shifted all the way left"
    // it's 0000000H LLLLLLLL not HHHHHHHH L0000000
    extern uint16_t plyr_facing; // angle 0..360 as 0..511, anti-clockwise
};
