#include "Arduino.h"
#include "RTC.h"
#include "Shapes.h"
#include "Sparse.h"
#include "SparseXL.h"
#include "BZROM.h"
#include "VGROM.h"
#include "Game.h"
#include "AVG.h"
#include "MathBox.h"
#include "BTN.h"

#ifdef DEBUG_STACK_CHECK
extern uint16_t stackHeadroom;
#endif

namespace Game
{
void I2A(uint8_t i, char*& pBuff, char leading)
{
  // convert i to 2-digit string at pBuff, using leading for the leading 0
  if (i < 10)
    *pBuff++ = leading;
  else
    *pBuff++ = '0' + i / 10;
  *pBuff++ = '0' + i % 10;
}

// game variables
#ifndef ENABLE_FIXED_ENEMY
uint8_t enemyType = Shapes::tank1;
#else
uint8_t enemyType = ENABLE_FIXED_ENEMY;
#endif
uint8_t enemyRotation = 0x60;
int16_t unit_pos_ZZ = 0; // into screen
int16_t unit_pos_XX = 0; // right
uint16_t plyr_facing = 0; // angle 0..360 as 0..511, anti-clockwise


enum AnimationPhase
{
    Pausing,
    TurningLeft,
    TurningRight,
    MovingForward,
    MovingBackward,
    ShowingBanner,
    ShowingHighScores,
    
    ManualControl
};

#define TEXT_SLOW_BLINK 6  // blink 'PRESS START' every 5 frames
#define TEXT_FAST_BLINK 3  // blink messages every 5 frames
enum AnimationPhase animationPhase = AnimationPhase::Pausing;
bool settingTime = false;
uint8_t animationStep = 1;
uint8_t showGameOverCounter = 0;  // counts down to not showing "Game Over"
uint8_t showBlockedCounter = 0;  // counts down to not showing "Motion blocked"
bool approachLatch = false;
byte displayedMinute = 0xFF;
#define MSG_BLINK_FLAG   0x80
#define MSG_BLOCKED_FLAG 0x40
#define MSG_MANUAL_FLAG  0x20
#define MSG_RANGE_FLAG   0x04
#define MSG_DIRN_MASK    0x03
uint8_t displayedMessageFlags = 0x00;    // 0bKBM00RDD K=blink on, B=motion blocked, M=manual, R=in range, DDD = 0/1/2/3 = direction none/left/right/rear
uint32_t frameCounter = 0;
bool forceAlternateMode = false;
uint8_t alternateMode = 0;  // 0/1/2 = Banner/High Score/Game Over

// size of movement steps
#define PLAYER_ROTATION_STEP 5
#define PLAYER_MOVEMENT_STEP 1000

#define LIFE_ICON_SIZE_PERCENT 100
#ifdef ENABLE_LARGER_TEXT
// Make score text and ("half size") high score text a bit more legible
#ifdef SCREEN_FULL_SIZE
#define SCORE_TEXT_PERCENT     110
#define HALF_SIZE_TEXT_PERCENT  75
#else
#define SCORE_TEXT_PERCENT     120
#define HALF_SIZE_TEXT_PERCENT  70
#endif
#else
#define SCORE_TEXT_PERCENT     100
#define HALF_SIZE_TEXT_PERCENT  50
#endif

// PRNG:
uint32_t _lfsr = 0xBA77UL;  // BATTUL :-)
uint8_t POKEY_RANDOM(uint8_t max)
{ 
  // 17-bit LFSR w/ taps @ 17 14, period = 131071
  // returns 0..max inclusive
  uint32_t bit  = ((_lfsr >> 0) ^ (_lfsr >> 3)) & 1;
  _lfsr =  (_lfsr >> 1) | (bit << 16);
  return (uint8_t)(_lfsr % (max+1)); 
}

uint8_t EnemyRotation()
{
    // Return the angle the current enemy is rotated by. 
    // A constant except for the spinning saucer & clock
    if (enemyType == Shapes::saucer)
        return (uint8_t)(frameCounter << 2);        // spin
    else if (enemyType == Shapes::clock)
    {
        // face player, but swing
        const uint8_t bits = 5;
        int16_t swing = frameCounter & ((1 << bits) - 1);
        if (frameCounter & (1 << bits))
            swing = ((1 << bits) - 1) - swing;
        if (frameCounter & (2 << bits))
            swing = -swing;
        return (plyr_facing >> 1) + 128 + swing;
    }
    else
        return enemyRotation;
}

void NewEnemy()
{
    // Pick a new enemy at random
#ifndef ENABLE_FIXED_ENEMY    
    uint8_t enemy = POKEY_RANDOM(4);
    switch (enemy)
    {
        case 0:  enemyType = Shapes::missile; break;
        case 1:  enemyType = Shapes::saucer; break;
        case 2:  enemyType = Shapes::tank2; break;
        case 3:  enemyType = Shapes::tank1; break;
#if defined(RTC_I2C_ADDRESS) && defined(ENABLE_TIME_ENEMY)
        case 4:  enemyType = Shapes::clock; break;
#endif
        default: enemyType = Shapes::tank1; break;
    }
    enemyRotation = POKEY_RANDOM(255);
    // Aways at the same location
#endif    
}

void StartBanner()
{
    // Start showing the banner/crawl 
    animationPhase = AnimationPhase::ShowingBanner;
    animationStep = 0;
    Shapes::objectDeltaYY = 0;
    unit_pos_ZZ = 0;
    unit_pos_XX = 0;
    plyr_facing = 0;
    alternateMode = 1;
    if (frameCounter)
    {
       NewEnemy();
    }
}

// Multi-strings are one or more strings concatenated into a single string, usually in PROGMEM.
// Strings are separated by a NUL.  The multi-string ends with two NUL's
const char HiScoreListMStr[] PROGMEM = OPTION_HIGH_SCORE_LIST;

void StartHighScores()
{
    // Start showing the high scores screen
    animationPhase = AnimationPhase::ShowingHighScores;
    animationStep = 25;
    alternateMode = 2;
}

void StartGameOver()
{
    // Start showing Game Over
    showGameOverCounter = 150;
    alternateMode = 0;
}

void Init()
{
#ifdef ENABLE_RANDOMIZE
    rtc.Seed(_lfsr);
#endif  
    animationPhase = AnimationPhase::Pausing;
    animationStep = 1;
    approachLatch = false;
    StartBanner();
}

void AnimateApproach(bool enemyInSights, int16_t range)
{
    // Move up to the enemy when it gets into our sights
    // But not again until it's out of our sights
    if (enemyInSights)
    {
        if (!approachLatch && animationPhase != AnimationPhase::MovingForward)
        {
            animationPhase = AnimationPhase::MovingForward;
            animationStep = (range > 0x4000)?range >> 10:10;    // Or use SHAPES_NEAR_PLANE?
            approachLatch = true;
        }
    }
    else
        approachLatch = false;
}

void RadarHandVector(uint8_t angle, int16_t len, int16_t& dX, int16_t& dY)
{
    // Compute the vector deltas for a hand
    int32_t cosAngle = Shapes::Cos(angle);
    int32_t sinAngle = Shapes::Sin(angle);
    int32_t hyp = len;
    hyp *= sinAngle;
    hyp /= 32767;
    dX = hyp;
    hyp = len;
    hyp *= cosAngle;
    hyp /= 32767;
    dY = hyp;
}

void DrawRadarClock()
{
    // draw the radar ticks and the hour and minute hands instead of the POV 'V'
    AVG::MoveBeamTo(0, 0);
    AVG::MoveBeam(+68, +316);
    AVG::DrawBeam(-8, +0);      // 3 O'Clock tick
    AVG::MoveBeam(-60, -60);
    AVG::DrawBeam(+0, -8);      // 6 O'Clock tick
    AVG::MoveBeam(-60, +68);
    AVG::DrawBeam(-8, +0);      // 9 O'Clock tick
    AVG::MoveBeam(+68, +60);
    AVG::DrawBeam(+0, +8);      // 12 O'Clock tick  
    AVG::MoveBeam(+0, -68);     // centre
    int16_t dX,  dY;
    RadarHandVector(256*rtc.m_Minute/60, 65, dX,  dY);
    AVG::DrawBeam(dX,  dY);
    AVG::MoveBeam(-dX,  -dY);
    RadarHandVector(256*(rtc.m_Hour24 % 12)/12 + 256*rtc.m_Minute/60/12, 40, dX,  dY);
    AVG::DrawBeam(dX,  dY);
}

const int16_t bannerSpeedFactor = 4;
void DrawBanner()
{
    // Draw the banner/crawl
    //     BATTLE
    //      ZONE
    // receding towards the sky
    int16_t range;
    bool visible;
    uint8_t dirn;

    // The 8 & 64 steps sizes are from the original (but multiplied to speed up crawl)
    // Starting coords are arbitrary to look OK and not produce a hz line along the "TT" of more than 255 pixels
    Shapes::objectDeltaYY = -500 + bannerSpeedFactor*8*animationStep;
    int16_t ZZ = 2000 + bannerSpeedFactor*64*animationStep;
    Shapes::DrawObject(Shapes::BA,   0x00, 0x0000, ZZ, range, visible, dirn);
    Shapes::DrawObject(Shapes::TTLE, 0x00, 0x0000, ZZ, range, visible, dirn);
    Shapes::DrawObject(Shapes::ZONE, 0x00, 0x0000, ZZ, range, visible, dirn);
    AVG::ROMStr(BZROM::AtariStr);
}

void DrawHighScores()
{
    // Draw the high score initials
    AVG::ROMStr(BZROM::HighScoresStr);
    int16_t x = -128, y = +104;
    const char* pStr = HiScoreListMStr;
    while (pgm_read_byte_near(pStr))
    {
        pStr = AVG::Str(pStr, AVG::ProgMemCharReader, 100, x, y);
        if (pgm_read_byte_near(pStr - 2) == '!')
            AVG::Run(VGROM::LifeOffset(), 100);
        y -= 40;
        x -= 4; // shift the scores to the left a little with each line, not all ROMs do this
    }
}

// Convert char to ROM-style encoding
#define ROMCHR(_ch) (2*(_ch-54))
//-440,+264 'MANUAL'
const uint8_t ManualStr[] PROGMEM = {0x92,0x42, ROMCHR('M'),ROMCHR('A'),ROMCHR('N'),ROMCHR('U'),ROMCHR('A'),ROMCHR('L')+0x80};
void DrawMessages(uint8_t flags)
{
    // Draw the messages indicated by flags
    displayedMessageFlags = flags;
    int16_t xOverride = (HALF_SIZE_TEXT_PERCENT > 50)?-510:-1;  // move the messages far left if > 50% or 'MOTION BLOCKED BY OBJECT' hits radar
    int16_t yDeltaOverride = (HALF_SIZE_TEXT_PERCENT > 50)?-10:0;  // move the messages down if > 50% or too close to the top
    if (flags & MSG_MANUAL_FLAG)
       AVG::ROMStr(ManualStr, HALF_SIZE_TEXT_PERCENT, xOverride, yDeltaOverride);
#ifdef ENABLE_STATUS_MSGS
    if (flags & MSG_BLOCKED_FLAG || showBlockedCounter)
    {
        AVG::ROMStr(BZROM::MotionBlockedStr, HALF_SIZE_TEXT_PERCENT, xOverride, yDeltaOverride);
    }
    if (flags & MSG_BLINK_FLAG)
    {
        if (flags & MSG_RANGE_FLAG)
            AVG::ROMStr(BZROM::EnemyInRangeStr, HALF_SIZE_TEXT_PERCENT, xOverride, yDeltaOverride);
        flags &= MSG_DIRN_MASK;
        if (flags)
        {
            AVG::ROMStr(BZROM::EnemyToStr, HALF_SIZE_TEXT_PERCENT, xOverride, yDeltaOverride);
            switch (flags)
            {
                case 1:  AVG::ROMStr(BZROM::LeftStr,  HALF_SIZE_TEXT_PERCENT);  break;
                case 2:  AVG::ROMStr(BZROM::RightStr, HALF_SIZE_TEXT_PERCENT); break;
                default: AVG::ROMStr(BZROM::RearStr,  HALF_SIZE_TEXT_PERCENT);  break;
            }
        }
    }
#endif
}

const char pgmOptionScore[] PROGMEM = OPTION_SCORE;
const char pgmOptionHighScore[] PROGMEM = OPTION_HIGH_SCORE;
void DrawStatus()
{
    // Radar, lives, score (time)
    AVG::MoveBeamTo(128, 360);
    for (int life = 0; life < 3; life++)
    { 
        AVG::Run(VGROM::LifeOffset(), 100);
    }

#if defined(ENABLE_TIME_SCORE) || defined(ENABLE_TIME_RADAR)
    rtc.ReadTime();
#endif

#if defined(ENABLE_TIME_RADAR)
    DrawRadarClock();
#else
    AVG::Run(VGROM::RadarOffset());
#endif

    AVG::ROMStr(BZROM::ScoreStr, SCORE_TEXT_PERCENT); // "SCORE"
#ifdef DEBUG_STACK_CHECK
    // report stack headroom in score
    char strBuffer2[5];
    char* pStr2 = strBuffer2;
    I2A(stackHeadroom / 100, pStr2, ' ');
    I2A(stackHeadroom % 100, pStr2, '0');
    *pStr2 = 0;
    AVG::Str(strBuffer2, AVG::StdCharReader, SCORE_TEXT_PERCENT);
#elif defined(ENABLE_TIME_SCORE)
    // show the time as the score
    char strBuffer[8];
    // poke the time into the string
    char* pStr = strBuffer + 3;
    memset(strBuffer, ' ', sizeof(strBuffer));
#ifdef ENABLE_TIME_24HOUR
    I2A(rtc.m_Hour24, pStr, '0');
#else
    byte hour = rtc.m_Hour24;
    if (hour > 12)
        hour -= 12;
    else if (hour == 0)
        hour = 12;
    I2A(hour, pStr, ' ');
#endif
    *pStr = 0;
    I2A(rtc.m_Minute, pStr, '0');
    *pStr = 0;
    AVG::Str(strBuffer, AVG::StdCharReader, SCORE_TEXT_PERCENT);
#else
    // show a fixed score
    AVG::Str(pgmOptionScore, AVG::ProgMemCharReader, SCORE_TEXT_PERCENT);
#endif

    AVG::ROMStr(BZROM::HighScoreStr, HALF_SIZE_TEXT_PERCENT); 
    AVG::Str(pgmOptionHighScore, AVG::ProgMemCharReader, HALF_SIZE_TEXT_PERCENT);
}

void DrawReticle(bool enemyInSights)
{
    AVG::STAT = 0xF7; // force both reticles to be dim/dotted
    AVG::Run(VGROM::ReticleOffset(enemyInSights));
    AVG::STAT = 0x0F;
}

void DrawGameOver()
{
    // Overlay the screen with Game Over, if enabled
    if (showGameOverCounter)
    {
        showGameOverCounter--;
        AVG::ROMStr(BZROM::GameOverStr);
        AVG::ROMStr(BZROM::AtariStr);
        if ((frameCounter % (2*TEXT_SLOW_BLINK)) < TEXT_SLOW_BLINK)
            AVG::ROMStr(BZROM::PressStartStr); 
    }
}

void DrawBackground()
{
    // Draw the background mountains
    // see https://6502disassembly.com/va-battlezone/Battlezone.html#SymDrawBackground

    uint16_t temp = plyr_facing << 3;  // plyr_facing is stored unshifted
    uint8_t A = ((temp >> 8) & 0x01) | 0x02; // 2 or 3
    uint16_t deltaX = (A << 8) | (temp & 0xFF);
    AVG::MoveBeamTo(deltaX, 0);
    // We skip the actual draw to not flood the Sparse data (see SPARSE::PaintHorizon):
    //AVG::Run(VGROM::HorizonOffset());  // horizon, moves back -1536 (=-3*512) 
    AVG::MoveBeamTo(deltaX-1536, 0);
    uint16_t segment = ((temp >> 8) ^ 0x0F) >> 1; // halve it for an index, not an offset to the word
    // draw 3 segments to guarantee coverage (512 units each)
    AVG::Run(VGROM::LandscapeSectionOffset(segment++));
    AVG::Run(VGROM::LandscapeSectionOffset(segment++));
    AVG::Run(VGROM::LandscapeSectionOffset(segment));
}

void Draw(bool full)
{
    // Draw the entire screen
    if (settingTime) return;
    byte* pRowStart = NULL;
    Shapes::nearClip = false;

    // Green
    SPARSE::Clear(SCREEN_UPPER_HEIGHT + 1);

    uint32_t visibleObstacles = -1;
    if (animationPhase != AnimationPhase::ShowingHighScores)
    {
        DrawBackground();

        AVG::STAT = 0xFA;
        visibleObstacles = Shapes::DrawVisibleObstacles(visibleObstacles);
        AVG::STAT = 0x0F;
    }

    bool enemyVisible = false;
    bool enemyInSights = false;
    int16_t enemyRange = 0;
    uint8_t enemyDirection = 0;
    if (animationPhase == AnimationPhase::ShowingBanner)
    {
        DrawBanner();
    }
    else if (animationPhase == AnimationPhase::ShowingHighScores)
    {
        DrawHighScores();
    }
    else
    {
        if (enemyType == Shapes::saucer)    // reduce the load on Sparse for the complex saucer
            AVG::STAT = 0xFA;
        enemyInSights = Shapes::DrawObject(enemyType, EnemyRotation(), 0x1000, 0x4000, enemyRange, enemyVisible, enemyDirection);
        DrawReticle(enemyInSights);
        DrawGameOver();
    }
   
    pRowStart = NULL;
    // paint the green area in three bands...
    int minRow = 0;
    int maxRow = SCREEN_HEIGHT/2 - SCREEN_UPPER_HEIGHT;
    int orgY = SCREEN_UPPER_HEIGHT + 1;
    if (animationPhase == AnimationPhase::ShowingHighScores)
        maxRow++;
    // ... top
    SPARSE::Paint(SCREEN_OFFSET_X, SCREEN_OFFSET_Y + orgY, minRow, maxRow, pRowStart, RGB(0, 255, 0));

    if (animationPhase != AnimationPhase::ShowingHighScores)
    {
        minRow = maxRow;
        orgY += maxRow - minRow;
        maxRow = minRow + 1;
        // ... horizon line (line itself is not in Sparse, it's the background for this row) ...
        SPARSE::PaintHorizon(SCREEN_OFFSET_X, SCREEN_OFFSET_Y + orgY, minRow, maxRow, pRowStart, RGB(0, 255, 0), RGB(0, 255, 0)); // maxRow is redundant?
    }

#ifdef SCREEN_SPLIT
    minRow = maxRow;
    orgY += maxRow - minRow;
    maxRow += 3*SCREEN_HEIGHT/5 - SCREEN_HEIGHT/2 - 1;
    // ... bottom (to half way)...
    SPARSE::Paint(SCREEN_OFFSET_X, SCREEN_OFFSET_Y + orgY, minRow, maxRow, pRowStart, RGB(0, 255, 0));

    // ... remainder as a SECOND PASS. Render again and paint
    orgY = 3*SCREEN_HEIGHT/5;
    SPARSE::Clear(orgY);
    if (animationPhase == AnimationPhase::ShowingBanner)
    {
        DrawBanner();
    }
    else if (animationPhase == AnimationPhase::ShowingHighScores)
    {
        DrawHighScores();
    }
    else
    {
        // Second pass, draw the enemy first vs last (if visible), re-uses the transformed coordinates
        if (enemyType == Shapes::saucer)
            AVG::STAT = 0xFA;
        if (enemyVisible)
            Shapes::DrawTransformedObject(enemyType);
        DrawReticle(enemyInSights);
        DrawGameOver();
    }

    if (animationPhase != AnimationPhase::ShowingHighScores)
    {
        // skip obstacles found to be invisible in the first pass
        AVG::STAT = 0xFA;
        Shapes::DrawVisibleObstacles(visibleObstacles);
        AVG::STAT = 0x0F;
    }

    pRowStart = NULL;
    SPARSE::Paint(SCREEN_OFFSET_X, SCREEN_OFFSET_Y + orgY, 0, 2*SCREEN_HEIGHT/5, pRowStart, RGB(0, 255, 0));
#else
    // draw everything below the horizon
    minRow = maxRow;
    orgY += maxRow - minRow;
    maxRow = SCREEN_LOWER_HEIGHT;
    // ... bottom
    SPARSE::Paint(SCREEN_OFFSET_X, SCREEN_OFFSET_Y + orgY, minRow, maxRow, pRowStart, RGB(0, 255, 0));
#endif

    bool status = full;
#if defined(ENABLE_TIME_SCORE) || defined(ENABLE_TIME_RADAR)
    // redraw if the status is showing the time and the time has changed
    byte minute = rtc.ReadMinute();
    if (minute != displayedMinute)
    {
        status = true;
        displayedMinute = minute;
        displayedMessageFlags = 0xFF;   // force redraw of messages
    }
#endif

    // what messages should show
    uint8_t flags = 0x00;
#ifdef ENABLE_STATUS_MSGS
#ifdef ENABLE_FLASH_MSGS
    flags = ((frameCounter % (2*TEXT_FAST_BLINK)) < TEXT_FAST_BLINK)?MSG_BLINK_FLAG:0;  // blink
#else
    flags = MSG_BLINK_FLAG;
#endif
    flags |= (enemyRange < SHAPES_FAR_PLANE/2)?MSG_RANGE_FLAG:0;    // range
    if (Shapes::nearClip && animationPhase == MovingForward)   // block
    {
        flags |= MSG_BLOCKED_FLAG; 
        showBlockedCounter = 5; // show blocked for a few frames
    }
    if (animationPhase == AnimationPhase::ManualControl)
        flags |= MSG_MANUAL_FLAG;
    flags |= enemyDirection;    // to left (etc)
#endif
#ifdef DEBUG_STACK_CHECK
    static uint8_t prevHeadroom = 0xFF;
    if (prevHeadroom != stackHeadroom)
    {
      status = true;
      prevHeadroom = stackHeadroom;
    }
#endif
    if (status || flags != displayedMessageFlags)   // need to redraw
    {
        // Red
        SPARSE::Clear(0);
        if (animationPhase != AnimationPhase::ShowingBanner && animationPhase != AnimationPhase::ShowingHighScores)
        {
            if (flags != displayedMessageFlags || showBlockedCounter)
                DrawMessages(flags);
        }
        else
            displayedMessageFlags = flags;
        DrawStatus();
        pRowStart = NULL;
        SPARSE::Paint(SCREEN_OFFSET_X, SCREEN_OFFSET_Y, 0, SCREEN_UPPER_HEIGHT, pRowStart, RGB(255, 0, 0));
    }
    if (showBlockedCounter)
        showBlockedCounter--;
    if (animationPhase != AnimationPhase::ShowingBanner && animationPhase != AnimationPhase::ShowingHighScores && animationPhase != AnimationPhase::ManualControl)
        AnimateApproach(enemyInSights, enemyRange);

    frameCounter++;
}

void RotatePlayer(int16_t degrees)
{
    // turn the player
    plyr_facing += degrees;
    plyr_facing %= 512;
}

void MovePlayer(int16_t distance)
{
    // move player distance in the direction they are facing
    int32_t H = distance;
    int32_t dZZ = (H*Shapes::Cos(plyr_facing >> 1)) >> 16;
    int32_t dXX = (H*Shapes::Sin(plyr_facing >> 1)) >> 16;
    unit_pos_ZZ += 2*dZZ;
    unit_pos_XX += 2*dXX;
}

bool GetTouch(int& x, int& y)
{
    // True if the screen has been touched.
    // "debounced", x & y relative to the top-left of the game screen
    int x1, y1;
    if (LCD_GET_TOUCH(x1, y1))
    {
        delay(10);
        if (LCD_GET_TOUCH(x, y))
        {
            if ((abs(x - x1) < 5 && abs(y - y1) < 5))
            {
                x -= SCREEN_OFFSET_X;
                y -= SCREEN_OFFSET_Y;
                return true;
            }
        }
    }
    return false;
}

void WaitForNoTouch()
{
    // wait for up
    int na;
    while (LCD_GET_TOUCH(na, na)) 
        ; 
    (void)na;
}

// Setting the time is a little convoluted because this was developed in a 
// Windows simulation and the main loop is very different
uint8_t setHour24, setMinute;
bool settingHour, setBlink;
uint16_t blinkTimer;
uint8_t activityCounter;
const char pgmSET[] PROGMEM = "SET";
void SetTime_Draw()
{
    byte* pRowStart = NULL;
    SPARSE::Clear(0);
    int16_t scale = 150;
    int16_t x = 0, y = +340;;
    char buff[3];
    char* pBuff = buff;
    int16_t now = (int16_t)millis();
    if (uint16_t(now - blinkTimer) > 250)
    {
        setBlink = !setBlink;
        blinkTimer = now;
        activityCounter++;
    }
    I2A(setHour24, pBuff, '0');
    *pBuff = 0;
    if (!settingHour || setBlink)
        AVG::Str(buff, AVG::StdCharReader, scale, x, y);
    y -= scale*VGROM::GlyphHeight/100;
    pBuff = buff;
    I2A(setMinute, pBuff, '0');
    *pBuff = 0;
    if (settingHour || setBlink)
        AVG::Str(buff, AVG::StdCharReader, scale, x, y);
    y -= scale*VGROM::GlyphHeight/100;
    AVG::Str(pgmSET, AVG::ProgMemCharReader, scale, x, y);
    SPARSE::Paint(SCREEN_OFFSET_X, SCREEN_OFFSET_Y, 0, SCREEN_UPPER_HEIGHT, pRowStart, RGB(255, 0, 0));
}

void SetTime_Start(bool touch)
{
    rtc.ReadTime();
    setHour24 = rtc.m_Hour24;
    setMinute = rtc.m_Minute;
    settingHour = setBlink = true;
    activityCounter = 0;
    blinkTimer = (int16_t)millis();
    SetTime_Draw();
    if (touch)
        WaitForNoTouch();
    settingTime = true;
}

bool SetTime_Check()
{
    if (activityCounter >= 128)
        return false;
    int x, y;
    SetTime_Draw();
    if (btn1Set.CheckButtonPress())
    {
        activityCounter = 0;
        blinkTimer = (int16_t)millis();
        if (settingHour)
            settingHour = false;
        else
        {
            rtc.m_Hour24 = setHour24;
            rtc.m_Minute = setMinute;
            rtc.WriteTime();
            return false;
        }
    }
    else if (btn2Adj.CheckButtonPress())
    {
        activityCounter = 0;
        blinkTimer = (int16_t)millis();
        if (settingHour)
        {
            setHour24++;
            setHour24 = setHour24 % 24;
        }
        else
        {
            setMinute++;
            setMinute = setMinute % 60;
        }
    }
    if (GetTouch(x, y))
    {
        WaitForNoTouch();
        activityCounter = 0;
        blinkTimer = (int16_t)millis();
        if (y < SCREEN_UPPER_HEIGHT/3)
        {
            settingHour = true;
            setHour24++;
            setHour24 = setHour24 % 24;
        }
        else if (y < 2*SCREEN_UPPER_HEIGHT/3)
        {
            settingHour = false;
            setMinute++;
            setMinute = setMinute % 60;
        }
        else if (y <= SCREEN_UPPER_HEIGHT)
        {
            rtc.m_Hour24 = setHour24;
            rtc.m_Minute = setMinute;
            rtc.WriteTime();
            return false;
        }
        else
            return false;
    }
    return true;
}

void ManualMode()
{
    // Animation is paused and clicks move around
    int x, y;
    if (GetTouch(x, y))
    {
        if (y <= SCREEN_UPPER_HEIGHT)
        {
            // click in red area exits manual mode
            animationPhase = AnimationPhase::Pausing;
            animationStep = 0;
            displayedMessageFlags = 0xFF;
        }
        else
        {
            // make the position relative to the centre of the green area
            x -= SCREEN_WIDTH/2;
            y -= SCREEN_UPPER_HEIGHT + SCREEN_LOWER_HEIGHT/2;
            // divide screen with two diagonals, upper, lower, left and right triangle for forward, backward, left and right
            if (x > 0 && abs(y) < x)
                RotatePlayer(-PLAYER_ROTATION_STEP);
            else if (x < 0 && abs(y) < -x)
                RotatePlayer(+PLAYER_ROTATION_STEP);
            else if (y < 0 && abs(x) < -y)
                MovePlayer(+PLAYER_MOVEMENT_STEP);
            else if (y > 0 && abs(x) < y)
                MovePlayer(-PLAYER_MOVEMENT_STEP);
        }
    }
}

void CheckInteraction()
{
    // Check for button presses or screen touches
    if (settingTime)
    {
        if (!SetTime_Check())
        {
            settingTime = false;
            displayedMessageFlags = 0xFF;
        }
    }
    else  if (animationPhase == AnimationPhase::ManualControl)
    {
        ManualMode();
    }
    else
    {
        int x, y;
        if (GetTouch(x, y))
        {
            if (y < SCREEN_UPPER_HEIGHT)
            {
#ifdef RTC_I2C_ADDRESS
                SetTime_Start(true);
#endif
            }
            else
            {
                animationPhase = AnimationPhase::ManualControl;
            }
        }
#ifdef RTC_I2C_ADDRESS
        else if (btn1Set.CheckButtonPress())
           SetTime_Start(false);
#endif
        else if (btn2Adj.CheckButtonPress())
        {
            forceAlternateMode = true;
            animationStep = 0;
            animationPhase = AnimationPhase::Pausing;
        }
    }
}

void AnimationStep()
{
    // Create motion in "Attract" mode.  NOT the original game's logic, just a simple heuristic
    // [pausing] -> [turning left/right] -> [moving forward] -> [pausing]
    // but randomly, the next in the sequence of
    //               [pausing] -> [Banner] -> [pausing]
    //           or, [pausing] -> [High Scores] -> [pausing]
    //           or, [pausing] -> [Game Over enabled] -> [pausing]
    if (settingTime || animationPhase == AnimationPhase::ManualControl) return;
    if (animationPhase == AnimationPhase::ShowingBanner)
    {
        animationStep++;
        if (animationStep >= 150/bannerSpeedFactor)
        {
            // Done
            Shapes::objectDeltaYY = 0;
            animationStep = 0;
        }
        else
            return;
    }
    else if (animationPhase == AnimationPhase::ShowingHighScores)
    {
        if (animationStep)
        {
            animationStep--;
            return;
        }
    }

    if (animationStep)
    {
        // continue
        animationStep--;
        switch (animationPhase)
        {
            case AnimationPhase::TurningLeft:
                RotatePlayer(-PLAYER_ROTATION_STEP);
                break;
            case AnimationPhase::TurningRight:
                RotatePlayer(+PLAYER_ROTATION_STEP);
                break;
            case AnimationPhase::MovingForward:
                if (Shapes::nearClip)
                    animationStep = 0;  // we got quite close to something, stop
                else
                    MovePlayer(+PLAYER_MOVEMENT_STEP);
                break;
            case AnimationPhase::MovingBackward:
                MovePlayer(-PLAYER_MOVEMENT_STEP);
                break;
            default:
                break;
        }
    }
    else
    {
        // new phase
        switch (animationPhase)
        {
            case AnimationPhase::Pausing:
                // pausing->turning left/right
                // BUT, about 10% of the time, enable Banner/High Scores/Game Over 
                if (POKEY_RANDOM(100) <= 10 || forceAlternateMode)
                {
                    forceAlternateMode = false;
                    if (alternateMode == 0)
                        StartBanner();
                    else if (alternateMode == 1)
                        StartHighScores();
                    else
                    {
                        StartGameOver();
                        animationPhase = POKEY_RANDOM(1)?AnimationPhase::TurningLeft:AnimationPhase::TurningRight;
                        animationStep = 20 + POKEY_RANDOM(20);
                    }
                }
                else
                {
                    animationPhase = POKEY_RANDOM(1)?AnimationPhase::TurningLeft:AnimationPhase::TurningRight;
                    animationStep = 20 + POKEY_RANDOM(20);
                }
                break;
            case AnimationPhase::TurningLeft:
            case AnimationPhase::TurningRight:
                // turning->moving forward
                animationPhase = AnimationPhase::MovingForward;
                animationStep = 20 + POKEY_RANDOM(20);
                break;
            case AnimationPhase::MovingForward:
            case AnimationPhase::MovingBackward:
                // moving->pausing
                animationPhase = AnimationPhase::Pausing;
                animationStep = 5 + POKEY_RANDOM(5);
                break;
            default:
                // Banner/High scores->pausing
                animationPhase = AnimationPhase::Pausing;
                animationStep = 5 + POKEY_RANDOM(5);
                break;
        }        
    }
}

// Each nibble in char defn's below specifies a node on a grid (starting from least significant):
// draw      move
// 4  5      C  D
//
// 2  3      A  B
//                 
// 0  1      8  9
static const uint32_t char_Defns[] PROGMEM = {0x00015409, 0x00000059, 0x00453209, 0x002B4518, 0x003D4239,
                                              0x00542318, 0x0004013A, 0x0000015C, 0x02B04518, 0x00032459};

void DrawClockObject(int16_t* screenCoords)
{

    // synthesize the lines that show the time as 4x 7-segment digits
    int16_t cX = 0, cY = 0;
    char strTime[5];
    // poke the time into the string
    char* pStr = strTime;
#ifdef ENABLE_TIME_24HOUR
    I2A(rtc.m_Hour24, pStr, '0');
#else
    byte hour = rtc.m_Hour24;
    if (hour > 12)
        hour -= 12;
    else if (hour == 0)
        hour = 12;
    I2A(hour, pStr, ' ');
#endif
    I2A(rtc.m_Minute, pStr, '0');
    *pStr = 0;
    AVG::MoveBeamTo(0, 0);
    for (int digit = 0; digit < 4; digit++)
    {
        uint32_t defn = (strTime[digit] == ' ')?0:pgm_read_dword(char_Defns  + strTime[digit] - '0');
        int16_t* digitCoords = screenCoords + 2*6*digit;
        while (defn)
        {
            uint8_t thisNode = defn & 0x07;
            int16_t X = digitCoords[2*thisNode];
            int16_t Y = digitCoords[2*thisNode + 1];
            if (defn & 0x08)
                AVG::MoveBeam(X - cX, Y - cY);
            else
                AVG::DrawBeam(X - cX, Y - cY);
            cX = X;
            cY = Y;
            defn >>= 4;
        }
    }
}

}
