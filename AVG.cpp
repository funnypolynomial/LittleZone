#include <Arduino.h>
#include "Game.h"
#include "AVG.h"
#include "Shapes.h"
#include "Sparse.h"
#include "SparseXL.h"
#include "MathBox.h"

// https://6502disassembly.com/va-battlezone/
// https://arcarc.xmission.com/Tech/neilw_xy.txt
// https://6502disassembly.com/va-battlezone/Battlezone.html

#include "VGROM.h"

namespace AVG 
{

 // cursor coords in CRT space
int16_t cur_beam_X = 0;   // x=-512...+512
int16_t cur_beam_Y = 0;   // y=-384...+384
const uint16_t* ReturnAddr = NULL;
byte STAT = 0x0F;   // The intensity used when '1' is specified.  But if 0b1111IIII, intensity I overrides all

int16_t Sext(uint16_t len, int16_t bit)
{
    // Sign extend
    // "with a negative number represented by the upper bit being set."
    if (len & (1 << bit))
    {
        len |= 0xFFFF ^ ((1 << bit) - 1);
    }

    return (int16_t)len;
}

int16_t Intensity(int16_t ii)
{
    // see avg_disasm.cpp
    if (ii != 1)
        ii *= 2;
    return ii;
}

int16_t Scale(int32_t s, int32_t num, int32_t den)
{
    // scale by num/den with rounding
    int32_t round = 0;
    if (s >= 0)
        round = ((+s*num) % den) > (den/2)?+1:0;
    else
        round = ((-s*num) % den) > (den/2)?-1:0;
    return num*s/den + round;
}

int16_t ScaleX(int16_t x)
{
    // scale & transform CRT coord to pixels
    return Scale(x, SCREEN_SCALE_NUM, SCREEN_SCALE_DEN) + SCREEN_WIDTH/2;
}

int16_t ScaleY(int16_t y)
{
    // scale & transform CRT coord to pixels
    return SCREEN_HEIGHT/2 - Scale(y, SCREEN_SCALE_NUM, SCREEN_SCALE_DEN);
}

void Draw(int16_t dX, int16_t dY, int16_t I, int16_t vectorScalePercent)
{
    // Draw a line from current postion to {dX, dY} with ehe given intensity and scale
    if (vectorScalePercent != 100)
    {
        dX = Scale(dX, vectorScalePercent, 100);
        dY = Scale(dY, vectorScalePercent, 100);
    }

    if (I)
    {
        if ((STAT & 0xF0) == 0xF0)
            I = STAT & 0x0F;
        else if (I == 1)
            I = STAT & 0x0F;
        int16_t x0 = ScaleX(cur_beam_X), y0 = ScaleY(cur_beam_Y);
        cur_beam_X += dX;
        cur_beam_Y += dY;
        // Somewhat arbitrary division at < 8 & 12 to {*   *  }, {* * * *} & {********} dot patterns
        SPARSE::Line(x0, y0, ScaleX(cur_beam_X), ScaleY(cur_beam_Y), 0, SCREEN_WIDTH, (I < 8)?0x88:((I < 12)?0xAA:0x00));
    }
    else
    {
        cur_beam_X += dX;
        cur_beam_Y += dY;
    }
}


void MoveBeamTo(int16_t X, int16_t Y)
{
    cur_beam_X = X;
    cur_beam_Y = Y;
}

void MoveBeam(int16_t dX, int16_t dY)
{
    Draw(dX, dY, 0x00, 100);
}

void DrawBeam(int16_t dX, int16_t dY)
{
    Draw(dX, dY, 0x0F, 100);
}

void Run(uint16_t offset, int16_t scalePercent)
{
    // Execute the ROM beam commands from the offset, with the scale
    const uint16_t* pCmd = VGROM::Base() + offset;
    ReturnAddr = NULL;
    bool firstOp = true;
    bool run = true;
    while (run)
    {
        uint16_t cmd = pgm_read_word_near(pCmd);
        uint8_t opcode = (cmd & 0xE000) >> 8;
        switch (opcode)
        {
            case 0x00:  // Draw relative vector.      0x00     000YYYYY YYYYYYYY IIIXXXXX XXXXXXXX
            {
                int16_t Y = Sext(cmd & 0x1FFF, 12);
                pCmd++;
                cmd = pgm_read_word_near(pCmd);
                int16_t I = Intensity(cmd >> 13);
                int16_t X = Sext(cmd & 0x1FFF, 12);
                Draw(X, Y, I, scalePercent);
                break;
            }
            case 0x20:  // Halt                       0x20     00100000 00000000
            {
                run = false;
                break;
            }
            case 0x40:  // Draw short relative vector 0x40     010YYYYY IIIXXXXX
            {
                int16_t Y = Sext((cmd & 0x1F00) >> 8, 4);
                int16_t I = Intensity((cmd >> 5) & 0x07);
                int16_t X = Sext(cmd & 0x1F, 4);

                Draw(2*X, 2*Y, I, scalePercent);  // see avg_disasm.cpp
                break;
            }
            case 0x60:  // New color/intensity        0x60     0110URGB IIIIIIII
            {           // New scale                  0x70     0111USSS SSSSSSSS
                // NOT IMPLEMENTED: intensity/scale changes
                // NOTE: 0x6800 (U=1) is used as a NO-OP to skip volcano details
                break;
            }
            case 0x80:  // Center                     0x80     10000000 00000000
            {
                MoveBeamTo(0, 0);
                break;
            }
            case 0xA0:  // Jump to subroutine         0xA0     101AAAAA AAAAAAAA
            {
                // Treat starting with a JSR as a special case, return when it does, a little odd (but see the glyph table)
                if (!firstOp)
                    ReturnAddr = pCmd;
                pCmd = VGROM::Base() + ((cmd & 0x1FFF) - VGROM::BaseAddress) - 1;
                break;
            }
            case 0xC0:  // Return from subroutine     0xC0     11000000 00000000
            {
                if (ReturnAddr)
                {
                    pCmd = ReturnAddr;
                    ReturnAddr = NULL;
                }
                else
                    run = false;
                break;
            }
            case 0xE0:  // Jump to new address        0xE0     111AAAAA AAAAAAAA
            {
                pCmd = VGROM::Base() + ((cmd & 0x1FFF) - VGROM::BaseAddress) - 1;
                break;
            }
        }
        pCmd++;
        firstOp = false;
    }
}

void ROMStr(const uint8_t* pgmStr, int16_t scalePercent, int16_t xOverride, int16_t yDeltaOverride)
{
    // draws a pre-defined BZ-style string 
    // see https://6502disassembly.com/va-battlezone/Battlezone.html#SymDrawStringPtr
    //; Each table entry is a two-byte header followed by a DCI string.
    //;  +00: absolute X position / 4
    //;  +01: absolute Y position / 4
    //;  +02: character data, with high bit set on last byte
    //; 
    //; If the X and Y position are both zero, the string is drawn at the current beam
    //; position.
    //; 
    //; The character set is:
    //;   ' ', 0-9, A-Z, ' ', '-', '(C)', '(P)'
    //; 
    //; Each character value is 2x the character index, which allows us to use the
    //; value directly as an index into a table of 16-bit VJSR instructions.
    int16_t x = 4*(char)pgm_read_byte_near(pgmStr++);
    int16_t y = 4*(char)pgm_read_byte_near(pgmStr++) + yDeltaOverride;
    if (xOverride != -1)
        x = xOverride;
    if (x || y)
        MoveBeamTo(x, y);
    uint8_t ch;
    do
    {
        ch = pgm_read_byte_near(pgmStr++);
        uint8_t index = (ch & 0x7F)/2;
        Run(VGROM::GlyphOffset(index), scalePercent);
    } while (!(ch & 0x80));
}

// read a char from a pointer to PROGMEM or RAM
char ProgMemCharReader(const char* a) { return pgm_read_byte_near(a); }
char StdCharReader    (const char* a) { return *a; }

const char* Str(const char* pStr, CharReader charReader, int16_t scalePercent, int16_t x, int16_t y)
{
    // Draws the string pStr at scale and position, uses charReader to access PROGMEM or RAM
    // returns pointer to character after the end NUL
    if (!(x == -1 && y == -1))
        MoveBeamTo(x, y);
    char ch = charReader(pStr++);
    while (ch)
    {
        Run(VGROM::GlyphOffset(ch), scalePercent);
        ch = charReader(pStr++);
    }
    return pStr;  
}

}
