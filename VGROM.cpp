#include <Arduino.h>
#include "VGROM.h"
namespace VGROM {
// see https://6502disassembly.com/va-battlezone/Battlezone.html
#define LABEL_org 0x3000
static const uint16_t ROM[] PROGMEM =
{
#define LABEL_vg_horizon_line   0x3000
    0x0000,             //VCTR dx=-1536 dy=+0 in=6
    0x7a00,
    0xc000,             //VRTS

// 
//; VJSRs for eight-part landscape + moon.  Each landscape section is 512 units
//; wide, covering half of the screen.  At most 3 sections will be needed.
// 
//; When the player faces angle 0, the reticle is just to the right of the moon. 
//; So at that angle, landscape 0 covers the left half of the screen, landscape 1
//; covers the right.  Every 0x20 units of rotation lines up with the next section.
// 
//; Note the list is not terminated with a VRTS, so if you call here you will see
//; the first section twice.
// 
#define LABEL_vg_landscape      0x3006
    0xA80B,            //VJSR a=0x080b (0x3016)
    0xA865,            //VJSR a=0x0865 (0x30ca)
    0xA882,            //VJSR a=0x0882 (0x3104)
    0xA8C1,            //VJSR a=0x08c1 (0x3182)
    0xA8E2,            //VJSR a=0x08e2 (0x31c4)
    0xA8F9,            //VJSR a=0x08f9 (0x31f2)
    0xA917,            //VJSR a=0x0917 (0x322e)
    0xA934,            //VJSR a=0x0934 (0x3268)

// ----- (Moon)
    0x0040,             //VCTR dx=+0 dy=+64 in=0 <<<
    0x0000,
    0x1FE0,             //VCTR dx=+32 dy=-32 in=6
    0x6020,
    0x5C18,             //SVEC dx=-16 dy=-8 in=0
    0x0028,             //VCTR dx=+80 dy=+40 in=6
    0x6050,
    0x0000,             //VCTR dx=+32 dy=+0 in=6
    0x6020,
    0x1FE0,             //VCTR dx=+32 dy=-32 in=6
    0x6020,
    0x0020,             //VCTR dx=-64 dy=+32 in=6
    0x7FC0,
    0x1FC0,             //VCTR dx=+0 dy=-64 in=0
    0x0000,
    0x0040,             //VCTR dx=+128 dy=+64 in=6
    0x6080,
    0x1FC0,             //VCTR dx=+64 dy=-64 in=6
    0x6040,
    0x0000,             //VCTR dx=+32 dy=+0 in=0
    0x0020,
    0x0020,             //VCTR dx=-64 dy=+32 in=6
    0x7FC0,
    0x0020,             //VCTR dx=-32 dy=+32 in=0
    0x1FE0,
    0x1FE0,             //VCTR dx=+64 dy=-32 in=6
    0x6040,
    0x1FF0,             //VCTR dx=+64 dy=-16 in=6
    0x6040,
    0x1FF0,             //VCTR dx=+96 dy=-16 in=6
    0x6060,
    0x00A0,             //VCTR dx=+48 dy=+160 in=0
    0x0030,
    0x1FF4,             //VCTR dx=+5 dy=-12 in=14
    0xE005,
    0x5AE0,             //SVEC dx=+0 dy=-12 in=14
    0x5AFC,             //SVEC dx=-8 dy=-12 in=14
    0x5AFA,             //SVEC dx=-12 dy=-12 in=14
    0x1FFD,             //VCTR dx=-12 dy=-3 in=14
    0xFFF4,
    0x0003,             //VCTR dx=-12 dy=+3 in=14
    0xFFF4,
    0x1FF7,             //VCTR dx=+12 dy=-9 in=14
    0xE00C,
    0x1FFD,             //VCTR dx=+12 dy=-3 in=14
    0xE00C,
    0x0003,             //VCTR dx=+12 dy=+3 in=14
    0xE00C,
    0x0009,             //VCTR dx=+12 dy=+9 in=14
    0xE00C,
    0x46E3,             //SVEC dx=+6 dy=+12 in=14
    0x46E0,             //SVEC dx=+0 dy=+12 in=14
    0x46FE,             //SVEC dx=-4 dy=+12 in=14
    0x000C,             //VCTR dx=-11 dy=+12 in=14
    0xFFF5,
    0x0003,             //VCTR dx=-13 dy=+3 in=4
    0x5FF3,
    0x1FFC,             //VCTR dx=-15 dy=-4 in=4
    0x5FF1,
    0x5C5B,             //SVEC dx=-10 dy=-8 in=4
    0x1FF5,             //VCTR dx=-5 dy=-11 in=4
    0x5FFB,
    0x1FF3,             //VCTR dx=-2 dy=-13 in=4
    0x5FFE,
    0x1FF1,             //VCTR dx=+6 dy=-15 in=4
    0x4006,
    0x0003,             //VCTR dx=+27 dy=+3 in=0
    0x001B,
    0x5EA0,             //SVEC dx=+0 dy=-4 in=10
    0x1FFF,             //VCTR dx=+6 dy=-1 in=10
    0xA006,
    0x000B,             //VCTR dx=+6 dy=+11 in=10
    0xA006,
    0x1FFF,             //VCTR dx=-4 dy=-1 in=10
    0xBFFC,
    0x0002,             //VCTR dx=-1 dy=+2 in=10
    0xBFFF,
    0x4A09,             //SVEC dx=+18 dy=+20 in=0
    0x1FFD,             //VCTR dx=-3 dy=-3 in=10
    0xBFFD,
    0x0003,             //VCTR dx=+1 dy=+3 in=10
    0xA001,
    0x0001,             //VCTR dx=-3 dy=+1 in=10
    0xBFFD,
    0x43A0,             //SVEC dx=+0 dy=+6 in=10
    0x0001,             //VCTR dx=+1 dy=+1 in=4
    0x4001,
    0x1F6A,             //VCTR dx=+7 dy=-150 in=0
    0x0007,
    0xC000,             //VRTS

// -----
    0x0000,             //VCTR dx=+32 dy=+0 in=0 <<<
    0x0020,
    0x0030,             //VCTR dx=+64 dy=+48 in=6
    0x6040,
    0x1FD0,             //VCTR dx=+32 dy=-48 in=6
    0x6020,
    0x0030,             //VCTR dx=-32 dy=+48 in=0
    0x1FE0,
    0x1FF0,             //VCTR dx=+32 dy=-16 in=6
    0x6020,
    0x0020,             //VCTR dx=+64 dy=+32 in=6
    0x6040,
    0x1FC0,             //VCTR dx=+160 dy=-64 in=6
    0x60A0,
    0x0020,             //VCTR dx=-128 dy=+32 in=6
    0x7F80,
    0x0020,             //VCTR dx=-32 dy=+32 in=6
    0x7FE0,
    0x1FE0,             //VCTR dx=-64 dy=-32 in=0
    0x1FC0,
    0x1FE0,             //VCTR dx=+96 dy=-32 in=6
    0x6060,
    0x0000,             //VCTR dx=+128 dy=+0 in=0
    0x0080,
    0x0020,             //VCTR dx=+160 dy=+32 in=6
    0x60A0,
    0x1FE0,             //VCTR dx=+0 dy=-32 in=0
    0x0000,
    0xC000,             //VRTS

// -----
    0x0020,             //VCTR dx=+0 dy=+32 in=0 <<<
    0x0000,
    0x0000,             //VCTR dx=+64 dy=+0 in=6
    0x6040,
    0x1FE0,             //VCTR dx=+0 dy=-32 in=0
    0x0000,
    0x0020,             //VCTR dx=-64 dy=+32 in=6
    0x7FC0,
    0x0000,             //VCTR dx=+64 dy=+0 in=0
    0x0040,
    0x0020,             //VCTR dx=+64 dy=+32 in=6
    0x6040,
    0x1FE0,             //VCTR dx=-64 dy=-32 in=0
    0x1FC0,
    0x1FE0,             //VCTR dx=+96 dy=-32 in=6
    0x6060,
    0x0020,             //VCTR dx=+32 dy=+32 in=6
    0x6020,
    0x0020,             //VCTR dx=-64 dy=+32 in=6
    0x7FC0,
    0x0000,             //VCTR dx=+32 dy=+0 in=6
    0x6020,
    0x1FE0,             //VCTR dx=+32 dy=-32 in=6
    0x6020,
    0x0010,             //VCTR dx=+32 dy=+16 in=6
    0x6020,
    0x1FF0,             //VCTR dx=+32 dy=-16 in=6
    0x6020,
    0x1FE0,             //VCTR dx=-96 dy=-32 in=6
    0x7FA0,
    0x0020,             //VCTR dx=+96 dy=+32 in=0
    0x0060,
    0x0020,             //VCTR dx=+64 dy=+32 in=6
    0x6040,
    0x1FE0,             //VCTR dx=+64 dy=-32 in=6
    0x6040,
    0x0010,             //VCTR dx=-96 dy=+16 in=0
    0x1FA0,
    0x1FF0,             //VCTR dx=+32 dy=-16 in=6
    0x6020,
    0x0010,             //VCTR dx=+32 dy=+16 in=6
    0x6020,
    0x1FD0,             //VCTR dx=+0 dy=-48 in=0
    0x0000,
    0x0008,             //VCTR dx=+96 dy=+8 in=6
    0x6060,
    0x0018,             //VCTR dx=-64 dy=+24 in=6
    0x7FC0,
    0x0010,             //VCTR dx=+32 dy=+16 in=6
    0x6020,
    0x1FF0,             //VCTR dx=+64 dy=-16 in=6
    0x6040,
    0x1FE8,             //VCTR dx=-32 dy=-24 in=6
    0x7FE0,
    0x0028,             //VCTR dx=-32 dy=+40 in=6
    0x7FE0,
    0x1FF0,             //VCTR dx=+64 dy=-16 in=0
    0x0040,
    0x0000,             //VCTR dx=+32 dy=+0 in=6
    0x6020,
    0x1FE0,             //VCTR dx=+0 dy=-32 in=0
    0x0000,
    0xC000,             //VRTS

// -----
    0x0020,             //VCTR dx=+0 dy=+32 in=0 <<<
    0x0000,
    0x1FE0,             //VCTR dx=+64 dy=-32 in=6
    0x6040,
    0x0020,             //VCTR dx=-64 dy=+32 in=0
    0x1FC0,
    0x1FE0,             //VCTR dx=+128 dy=-32 in=6
    0x6080,
    0x0000,             //VCTR dx=+160 dy=+0 in=0
    0x00A0,
    0x0020,             //VCTR dx=+96 dy=+32 in=6
    0x6060,
    0x1FE0,             //VCTR dx=+32 dy=-32 in=6
    0x6020,
    0x0020,             //VCTR dx=-32 dy=+32 in=0
    0x1FE0,
    0x0020,             //VCTR dx=+32 dy=+32 in=6
    0x6020,
    0x1FC0,             //VCTR dx=+32 dy=-64 in=6
    0x6020,
    0x0000,             //VCTR dx=+32 dy=+0 in=0
    0x0020,
    0x0040,             //VCTR dx=-64 dy=+64 in=6
    0x7FC0,
    0x1FE0,             //VCTR dx=+32 dy=-32 in=0
    0x0020,
    0x0020,             //VCTR dx=+32 dy=+32 in=6
    0x6020,
    0x1FE0,             //VCTR dx=+32 dy=-32 in=6
    0x6020,
    0x1FE0,             //VCTR dx=+0 dy=-32 in=0
    0x0000,
    0xC000,             //VRTS

// -----
    0x0020,             //VCTR dx=+0 dy=+32 in=0 <<<
    0x0000,
    0x0020,             //VCTR dx=+64 dy=+32 in=6
    0x6040,
    0x1FC0,             //VCTR dx=+32 dy=-64 in=6
    0x6020,
    0x0000,             //VCTR dx=+32 dy=+0 in=0
    0x0020,
    0x0040,             //VCTR dx=-64 dy=+64 in=6
    0x7FC0,
    0x1FE0,             //VCTR dx=+32 dy=-32 in=0
    0x0020,
    0x0010,             //VCTR dx=+32 dy=+16 in=6
    0x6020,
    0x1FD0,             //VCTR dx=+96 dy=-48 in=6
    0x6060,
    0x0000,             //VCTR dx=+128 dy=+0 in=0
    0x0080,
    0x0020,             //VCTR dx=+160 dy=+32 in=6
    0x60A0,
    0x1FE0,             //VCTR dx=+0 dy=-32 in=0
    0x0000,
    0xC000,             //VRTS

// ----- volcano left
    0x0020,             //VCTR dx=+0 dy=+32 in=0 <<<
    0x0000,
    0x0000,             //VCTR dx=+64 dy=+0 in=6
    0x6040,
    0x1FE0,             //VCTR dx=+224 dy=-32 in=6
    0x60E0,
    0x0030,             //VCTR dx=-32 dy=+48 in=6
    0x7FE0,
    0x1FE0,             //VCTR dx=-64 dy=-32 in=6
    0x7FC0,
    0x1FF0,             //VCTR dx=+96 dy=-16 in=0
    0x0060,
    0x0040,             //VCTR dx=+96 dy=+64 in=6
    0x6060,
    0x1FD8,             //VCTR dx=+64 dy=-40 in=6
    0x6040,
    0x5418,             //SVEC dx=-16 dy=-24 in=0
    0x0060,             //VCTR dx=+64 dy=+96 in=6
    0x6040,

// *MODIFIED* the details at the top of the volcano don't look so good with dotted lines, so omit them
    //0x1FF9,             //VCTR dx=+3 dy=-7 in=6
    //0x6003,
    //0x0005,             //VCTR dx=+5 dy=+5 in=6
    //0x6005,
    //0x1FFA,             //VCTR dx=+3 dy=-6 in=6
    //0x6003,
    //0x0008,             //VCTR dx=+5 dy=+8 in=6
    //0x6005,
    0x0000,             //VCTR dx=+16 dy=0 in=6
    0x6010,
    0x6800,             //NOOP
    0x6800,             //NOOP
    0x6800,             //NOOP
    0x6800,             //NOOP
    0x6800,             //NOOP
    0x6800,             //NOOP

    0x1FA0,             //VCTR dx=+0 dy=-96 in=0
    0x0000,
    0xC000,             //VRTS

// ----- volcano right
    0x0060,             //VCTR dx=+0 dy=+96 in=0 <<<
    0x0000,
    0x1FA0,             //VCTR dx=+64 dy=-96 in=6
    0x6040,
    0x0040,             //VCTR dx=+96 dy=+64 in=6
    0x6060,
    0x1FC0,             //VCTR dx=+64 dy=-64 in=6
    0x6040,
    0x0000,             //VCTR dx=+64 dy=+0 in=0
    0x0040,
    0x0040,             //VCTR dx=-128 dy=+64 in=6
    0x7F80,
    0x1FE0,             //VCTR dx=+64 dy=-32 in=0
    0x0040,
    0x0020,             //VCTR dx=+64 dy=+32 in=6
    0x6040,
    0x1FC0,             //VCTR dx=+128 dy=-64 in=6
    0x6080,
    0x0020,             //VCTR dx=-64 dy=+32 in=0
    0x1FC0,
    0x0010,             //VCTR dx=+64 dy=+16 in=6
    0x6040,
    0x1FD0,             //VCTR dx=+32 dy=-48 in=6
    0x6020,
    0x0030,             //VCTR dx=-32 dy=+48 in=0
    0x1FE0,
    0x1FD0,             //VCTR dx=+96 dy=-48 in=6
    0x6060,
    0xC000,             //VRTS

// -----
    0x0000,             //VCTR dx=+192 dy=+0 in=0 <<<
    0x00C0,
    0x0020,             //VCTR dx=+224 dy=+32 in=6
    0x60E0,
    0x1FE0,             //VCTR dx=+64 dy=-32 in=6
    0x6040,
    0x0010,             //VCTR dx=-32 dy=+16 in=0
    0x1FE0,
    0x0030,             //VCTR dx=+64 dy=+48 in=6
    0x6040,
    0x1FC0,             //VCTR dx=+0 dy=-64 in=0
    0x0000,
    0xC000,             //VRTS

// 
//; Character glyphs.  Each glyph fits in a 16x24 cell whose origin is the bottom-
//; left corner.  The beam will be left at the initial Y coordinate, 8 units to
//; the right of the character cell.
// 
// -----
    0x48C0,             //SVEC dx=+0 dy=+16 in=12 <<<
    0x44C4,             //SVEC dx=+8 dy=+8 in=12
    0x5CC4,             //SVEC dx=+8 dy=-8 in=12
    0x58C0,             //SVEC dx=+0 dy=-16 in=12
    0x4418,             //SVEC dx=-16 dy=+8 in=0
    0x40C8,             //SVEC dx=+16 dy=+0 in=12
    0x5C04,             //SVEC dx=+8 dy=-8 in=0
    0xC000,             //VRTS

// -----
    0x4CC0,             //SVEC dx=+0 dy=+24 in=12 <<<
    0x40C6,             //SVEC dx=+12 dy=+0 in=12
    0x5EC2,             //SVEC dx=+4 dy=-4 in=12
    0x5EC0,             //SVEC dx=+0 dy=-4 in=12
    0x5EDE,             //SVEC dx=-4 dy=-4 in=12
    0x40DA,             //SVEC dx=-12 dy=+0 in=12
    0x4006,             //SVEC dx=+12 dy=+0 in=0
    0x5EC2,             //SVEC dx=+4 dy=-4 in=12
    0x5EC0,             //SVEC dx=+0 dy=-4 in=12
    0x5EDE,             //SVEC dx=-4 dy=-4 in=12
    0x40DA,             //SVEC dx=-12 dy=+0 in=12
    0xE993,             //VJMP a=0x0993 (0x3326)

// -----
    0x4CC0,             //SVEC dx=+0 dy=+24 in=12 <<<
    0x40C8,             //SVEC dx=+16 dy=+0 in=12
    0x5418,             //SVEC dx=-16 dy=-24 in=0
    0xE9D6,             //VJMP a=0x09d6 (0x33ac)

// -----
    0x4CC0,             //SVEC dx=+0 dy=+24 in=12 <<<
    0x40C4,             //SVEC dx=+8 dy=+0 in=12
    0x5CC4,             //SVEC dx=+8 dy=-8 in=12
    0x5CC0,             //SVEC dx=+0 dy=-8 in=12
    0x5CDC,             //SVEC dx=-8 dy=-8 in=12
    0x40DC,             //SVEC dx=-8 dy=+0 in=12
    0xE993,             //VJMP a=0x0993 (0x3326)

// -----
    0x40C8,             //SVEC dx=+16 dy=+0 in=12 <<<
    0x4018,             //SVEC dx=-16 dy=+0 in=0

// -----
    0x4CC0,             //SVEC dx=+0 dy=+24 in=12 <<<
    0x40C8,             //SVEC dx=+16 dy=+0 in=12
    0x5A1E,             //SVEC dx=-4 dy=-12 in=0
    0x40DA,             //SVEC dx=-12 dy=+0 in=12
    0x5A0C,             //SVEC dx=+24 dy=-12 in=0 <<<
    0xC000,             //VRTS

// -----
    0x4CC0,             //SVEC dx=+0 dy=+24 in=12 <<<
    0x40C8,             //SVEC dx=+16 dy=+0 in=12
    0x5CC0,             //SVEC dx=+0 dy=-8 in=12
    0x5C1C,             //SVEC dx=-8 dy=-8 in=0
    0x40C4,             //SVEC dx=+8 dy=+0 in=12
    0x5CC0,             //SVEC dx=+0 dy=-8 in=12
    0xE992,             //VJMP a=0x0992 (0x3324)

// -----
    0x4CC0,             //SVEC dx=+0 dy=+24 in=12 <<<
    0x5A00,             //SVEC dx=+0 dy=-12 in=0
    0x40C8,             //SVEC dx=+16 dy=+0 in=12
    0x4600,             //SVEC dx=+0 dy=+12 in=0
    0xE9EF,             //VJMP a=0x09ef (0x33de)

// -----
    0x40C8,             //SVEC dx=+16 dy=+0 in=12 <<<
    0x4C18,             //SVEC dx=-16 dy=+24 in=0
    0x40C8,             //SVEC dx=+16 dy=+0 in=12 <<<
    0x401C,             //SVEC dx=-8 dy=+0 in=0
    0x54C0,             //SVEC dx=+0 dy=-24 in=12 <<<
    0x4008,             //SVEC dx=+16 dy=+0 in=0
    0xC000,             //VRTS

// -----
    0x4400,             //SVEC dx=+0 dy=+8 in=0 <<<
    0x5CC4,             //SVEC dx=+8 dy=-8 in=12
    0x40C4,             //SVEC dx=+8 dy=+0 in=12
    0xE9B4,             //VJMP a=0x09b4 (0x3368)

// -----
    0x4CC0,             //SVEC dx=+0 dy=+24 in=12 <<<
    0x4006,             //SVEC dx=+12 dy=+0 in=0
    0x5ADA,             //SVEC dx=-12 dy=-12 in=12
    0x5AC6,             //SVEC dx=+12 dy=-12 in=12
    0x4006,             //SVEC dx=+12 dy=+0 in=0
    0xC000,             //VRTS

// -----
    0x4C00,             //SVEC dx=+0 dy=+24 in=0 <<<
    0x54C0,             //SVEC dx=+0 dy=-24 in=12
    0xE9D6,             //VJMP a=0x09d6 (0x33ac)

// -----
    0x4CC0,             //SVEC dx=+0 dy=+24 in=12 <<<
    0x5CC4,             //SVEC dx=+8 dy=-8 in=12
    0x44C4,             //SVEC dx=+8 dy=+8 in=12
    0xE9EF,             //VJMP a=0x09ef (0x33de)

// -----
    0x4CC0,             //SVEC dx=+0 dy=+24 in=12 <<<
    0x54C8,             //SVEC dx=+16 dy=-24 in=12
    0xE9B4,             //VJMP a=0x09b4 (0x3368)

// 'O' and '0'.
// -----
    0x4CC0,             //SVEC dx=+0 dy=+24 in=12 <<<
    0x40C8,             //SVEC dx=+16 dy=+0 in=12
    0x54C0,             //SVEC dx=+0 dy=-24 in=12
    0x40D8,             //SVEC dx=-16 dy=+0 in=12 <<<

// Space character.
// -----
    0x400C,             //SVEC dx=+24 dy=+0 in=0 <<<
    0xC000,             //VRTS

// -----
    0x4CC0,             //SVEC dx=+0 dy=+24 in=12 <<<
    0x40C8,             //SVEC dx=+16 dy=+0 in=12
    0x5AC0,             //SVEC dx=+0 dy=-12 in=12
    0x40D8,             //SVEC dx=-16 dy=+0 in=12
    0xE966,             //VJMP a=0x0966 (0x32cc)

// -----
    0x4CC0,             //SVEC dx=+0 dy=+24 in=12 <<<
    0x40C8,             //SVEC dx=+16 dy=+0 in=12
    0x58C0,             //SVEC dx=+0 dy=-16 in=12
    0x5CDC,             //SVEC dx=-8 dy=-8 in=12
    0x40DC,             //SVEC dx=-8 dy=+0 in=12
    0x4404,             //SVEC dx=+8 dy=+8 in=0
    0x5CC4,             //SVEC dx=+8 dy=-8 in=12
    0xE9D7,             //VJMP a=0x09d7 (0x33ae)

// -----
    0x4CC0,             //SVEC dx=+0 dy=+24 in=12 <<<
    0x40C8,             //SVEC dx=+16 dy=+0 in=12
    0x5AC0,             //SVEC dx=+0 dy=-12 in=12
    0x40D8,             //SVEC dx=-16 dy=+0 in=12
    0x4002,             //SVEC dx=+4 dy=+0 in=0
    0x5AC6,             //SVEC dx=+12 dy=-12 in=12
    0xE9D7,             //VJMP a=0x09d7 (0x33ae)

// 'S' and '5'.
// -----
    0x40C8,             //SVEC dx=+16 dy=+0 in=12 <<<
    0x46C0,             //SVEC dx=+0 dy=+12 in=12
    0x40D8,             //SVEC dx=-16 dy=+0 in=12
    0x46C0,             //SVEC dx=+0 dy=+12 in=12
    0x40C8,             //SVEC dx=+16 dy=+0 in=12
    0xE9B5,             //VJMP a=0x09b5 (0x336a)

// -----
    0x4C00,             //SVEC dx=+0 dy=+24 in=0 <<<
    0xE976,             //VJMP a=0x0976 (0x32ec)

// -----
    0x4C00,             //SVEC dx=+0 dy=+24 in=0 <<<
    0x54C0,             //SVEC dx=+0 dy=-24 in=12
    0x40C8,             //SVEC dx=+16 dy=+0 in=12
    0x4CC0,             //SVEC dx=+0 dy=+24 in=12 <<<
    0x5404,             //SVEC dx=+8 dy=-24 in=0 <<<
    0xC000,             //VRTS

// -----
    0x4C00,             //SVEC dx=+0 dy=+24 in=0 <<<
    0x54C4,             //SVEC dx=+8 dy=-24 in=12
    0x4CC4,             //SVEC dx=+8 dy=+24 in=12
    0xE9B5,             //VJMP a=0x09b5 (0x336a)

// -----
    0x4C00,             //SVEC dx=+0 dy=+24 in=0 <<<
    0x54C0,             //SVEC dx=+0 dy=-24 in=12
    0x44C4,             //SVEC dx=+8 dy=+8 in=12
    0x5CC4,             //SVEC dx=+8 dy=-8 in=12
    0xE9B4,             //VJMP a=0x09b4 (0x3368)

// -----
    0x4CC8,             //SVEC dx=+16 dy=+24 in=12 <<<
    0x4018,             //SVEC dx=-16 dy=+0 in=0
    0x54C8,             //SVEC dx=+16 dy=-24 in=12
    0xE9F0,             //VJMP a=0x09f0 (0x33e0)

// -----
    0x4004,             //SVEC dx=+8 dy=+0 in=0 <<<
    0x48C0,             //SVEC dx=+0 dy=+16 in=12
    0x44DC,             //SVEC dx=-8 dy=+8 in=12
    0x4008,             //SVEC dx=+16 dy=+0 in=0
    0x5CDC,             //SVEC dx=-8 dy=-8 in=12
    0x5808,             //SVEC dx=+16 dy=-16 in=0
    0xC000,             //VRTS

// -----
    0x4C00,             //SVEC dx=+0 dy=+24 in=0 <<<
    0x40C8,             //SVEC dx=+16 dy=+0 in=12
    0x54D8,             //SVEC dx=-16 dy=-24 in=12
    0xE9D6,             //VJMP a=0x09d6 (0x33ac)

// -----
    0x4C04,             //SVEC dx=+8 dy=+24 in=0 <<<
    0xE978,             //VJMP a=0x0978 (0x32f0)

// -----
    0x4C00,             //SVEC dx=+0 dy=+24 in=0 <<<
    0x40C8,             //SVEC dx=+16 dy=+0 in=12
    0x5AC0,             //SVEC dx=+0 dy=-12 in=12
    0x40D8,             //SVEC dx=-16 dy=+0 in=12
    0x5AC0,             //SVEC dx=+0 dy=-12 in=12

// -----
    0x40C8,             //SVEC dx=+16 dy=+0 in=12 <<<
    0x4004,             //SVEC dx=+8 dy=+0 in=0 <<<
    0xC000,             //VRTS

// -----
    0x4C00,             //SVEC dx=+0 dy=+24 in=0 <<<
    0x40C8,             //SVEC dx=+16 dy=+0 in=12 <<<
    0x54C0,             //SVEC dx=+0 dy=-24 in=12
    0x40D8,             //SVEC dx=-16 dy=+0 in=12
    0x4600,             //SVEC dx=+0 dy=+12 in=0
    0x40C8,             //SVEC dx=+16 dy=+0 in=12
    0x5A04,             //SVEC dx=+8 dy=-12 in=0
    0xC000,             //VRTS

// -----
    0x4C00,             //SVEC dx=+0 dy=+24 in=0 <<<
    0x5AC0,             //SVEC dx=+0 dy=-12 in=12
    0x40C8,             //SVEC dx=+16 dy=+0 in=12
    0x4600,             //SVEC dx=+0 dy=+12 in=0
    0xE9EF,             //VJMP a=0x09ef (0x33de)

// -----
    0x4600,             //SVEC dx=+0 dy=+12 in=0 <<<
    0x40C8,             //SVEC dx=+16 dy=+0 in=12
    0x5AC0,             //SVEC dx=+0 dy=-12 in=12
    0x40D8,             //SVEC dx=-16 dy=+0 in=12
    0x4CC0,             //SVEC dx=+0 dy=+24 in=12
    0x540C,             //SVEC dx=+24 dy=-24 in=0
    0xC000,             //VRTS

// -----
    0x4C00,             //SVEC dx=+0 dy=+24 in=0 <<<
    0x40C8,             //SVEC dx=+16 dy=+0 in=12 <<<
    0x54C0,             //SVEC dx=+0 dy=-24 in=12 <<<
    0x4004,             //SVEC dx=+8 dy=+0 in=0 <<<
    0xC000,             //VRTS

// -----
    0x4CC0,             //SVEC dx=+0 dy=+24 in=12 <<<
    0xE9DA,             //VJMP a=0x09da (0x33b4)

// -----
    0x4608,             //SVEC dx=+16 dy=+12 in=0 <<<
    0x40D8,             //SVEC dx=-16 dy=+0 in=12
    0x46C0,             //SVEC dx=+0 dy=+12 in=12
    0xE9EE,             //VJMP a=0x09ee (0x33dc)

//; VJSRs to the instructions that draw glyphs.  Because these are all VJSRs, and
//; the list is terminated with a VRTS, you can exercise the full set by calling
//; the first entry.  (The self-test code does this.)
#define LABEL_vg_glyph_calls    0x33F0
    0xA993,             //VJSR a=0x0993 (0x3326) ' '
    0xA98F,             //VJSR a=0x098f (0x331e) '0'
    0xA9CF,             //VJSR a=0x09cf (0x339e) '1'
    0xA9D1,             //VJSR a=0x09d1 (0x33a2) '2'
    0xA9D9,             //VJSR a=0x09d9 (0x33b2) '3'
    0xA9E1,             //VJSR a=0x09e1 (0x33c2) '4'
    0xA9A9,             //VJSR a=0x09a9 (0x3352) '5'
    0xA9E6,             //VJSR a=0x09e6 (0x33cc) '6'
    0xA9ED,             //VJSR a=0x09ed (0x33da) '7'
    0xA9F2,             //VJSR a=0x09f2 (0x33e4) '8'
    0xA9F4,             //VJSR a=0x09f4 (0x33e8) '9'
    0xA941,             //VJSR a=0x0941 (0x3282) 'A'
    0xA949,             //VJSR a=0x0949 (0x3292) 'B'
    0xA955,             //VJSR a=0x0955 (0x32aa) 'C'
    0xA959,             //VJSR a=0x0959 (0x32b2) 'D'
    0xA960,             //VJSR a=0x0960 (0x32c0) 'E'
    0xA962,             //VJSR a=0x0962 (0x32c4) 'F'
    0xA968,             //VJSR a=0x0968 (0x32d0) 'G'
    0xA96F,             //VJSR a=0x096f (0x32de) 'H'
    0xA974,             //VJSR a=0x0974 (0x32e8) 'I'
    0xA97B,             //VJSR a=0x097b (0x32f6) 'J'
    0xA97F,             //VJSR a=0x097f (0x32fe) 'K'
    0xA985,             //VJSR a=0x0985 (0x330a) 'L'
    0xA988,             //VJSR a=0x0988 (0x3310) 'M'
    0xA98C,             //VJSR a=0x098c (0x3318) 'N'
    0xA98F,             //VJSR a=0x098f (0x331e) 'O'
    0xA995,             //VJSR a=0x0995 (0x332a) 'P'
    0xA99A,             //VJSR a=0x099a (0x3334) 'Q'
    0xA9A2,             //VJSR a=0x09a2 (0x3344) 'R'
    0xA9A9,             //VJSR a=0x09a9 (0x3352) 'S'
    0xA9AF,             //VJSR a=0x09af (0x335e) 'T'
    0xA9B1,             //VJSR a=0x09b1 (0x3362) 'U'
    0xA9B7,             //VJSR a=0x09b7 (0x336e) 'V'
    0xA9BB,             //VJSR a=0x09bb (0x3376) 'W'
    0xA9C0,             //VJSR a=0x09c0 (0x3380) 'X'
    0xA9C4,             //VJSR a=0x09c4 (0x3388) 'Y'
    0xA9CB,             //VJSR a=0x09cb (0x3396) 'Z'
    0xA993,             //VJSR a=0x0993 (0x3326) ' '
    0xA9D6,             //VJSR a=0x09d6 (0x33ac) '-'
    0xAA2A,             //VJSR a=0x0a2a (0x3454) (C)
    0xAA33,             //VJSR a=0x0a33 (0x3466) (P)
    0xC000,             //VRTS

// Hexagonal "circle" for (C) and (P).
// -----
    0x4200,             //SVEC dx=+0 dy=+4 in=0 <<<
    0x48C0,             //SVEC dx=+0 dy=+16 in=12
    0x42C4,             //SVEC dx=+8 dy=+4 in=12
    0x5EC4,             //SVEC dx=+8 dy=-4 in=12
    0x58C0,             //SVEC dx=+0 dy=-16 in=12
    0x5EDC,             //SVEC dx=-8 dy=-4 in=12
    0x42DC,             //SVEC dx=-8 dy=+4 in=12
    0xC000,             //VRTS

// -----
    0xAA22,             //VJSR a=$0a22 ($3444) <<<
    0x0001,             //VCTR dx=+11 dy=+1 in=0
    0x000B,
    0x40DD,             //SVEC dx=-6 dy=+0 in=12
    0x47C0,             //SVEC dx=+0 dy=+14 in=12
    0x40C3,             //SVEC dx=+6 dy=+0 in=12
    0x1FED,             //VCTR dx=+13 dy=-19 in=0
    0x000D,
    0xC000,             //VRTS

// -----
    0xAA22,             //VJSR a=$0a22 ($3444) <<<
    0x0001,             //VCTR dx=+5 dy=+1 in=0
    0x0005,
    0x47C0,             //SVEC dx=+0 dy=+14 in=12
    0x40C3,             //SVEC dx=+6 dy=+0 in=12
    0x5DC0,             //SVEC dx=+0 dy=-6 in=12
    0x40DD,             //SVEC dx=-6 dy=+0 in=12
    0x1FF3,             //VCTR dx=+19 dy=-13 in=0
    0x0013,
    0xC000,             //VRTS

//; Projectile explosion, displayed when a projectile from the player or an enemy
//; tank strikes something.  The pattern is scaled up over the course of a few
//; frames.
// -----
#define LABEL_vg_proj_explosion 0x347A
    0x0000,             //VCTR dx=-64 dy=+0 in=0
    0x1FC0,
    0x0000,             //VCTR dx=+0 dy=+0 in=14
    0xE000,
    0x1FC0,             //VCTR dx=-64 dy=-64 in=0
    0x1FC0,
    0x0000,             //VCTR dx=+0 dy=+0 in=14
    0xE000,
    0x1FC0,             //VCTR dx=+64 dy=-64 in=0
    0x0040,
    0x0000,             //VCTR dx=+0 dy=+0 in=14
    0xE000,
    0x0020,             //VCTR dx=+96 dy=+32 in=0
    0x0060,
    0x0000,             //VCTR dx=+0 dy=+0 in=14
    0xE000,
    0x1FE0,             //VCTR dx=+64 dy=-32 in=0
    0x0040,
    0x0000,             //VCTR dx=+0 dy=+0 in=14
    0xE000,
    0x0040,             //VCTR dx=+0 dy=+64 in=0
    0x0000,
    0x0000,             //VCTR dx=+0 dy=+0 in=14
    0xE000,
    0x0060,             //VCTR dx=+32 dy=+96 in=0
    0x0020,
    0x0000,             //VCTR dx=+0 dy=+0 in=14
    0xE000,
    0x0060,             //VCTR dx=-32 dy=+96 in=0
    0x1FE0,
    0x0000,             //VCTR dx=+0 dy=+0 in=14
    0xE000,
    0x1FE0,             //VCTR dx=-128 dy=-32 in=0
    0x1F80,
    0x0000,             //VCTR dx=+0 dy=+0 in=14
    0xE000,
    0x0020,             //VCTR dx=-96 dy=+32 in=0
    0x1FA0,
    0x0000,             //VCTR dx=+0 dy=+0 in=14
    0xE000,
    0xC000,             //VRTS
    
// -----
#define LABEL_vg_reticle1 0x34CC
    0x8040,             //CNTR
    0x1F51,             //VCTR dx=+0 dy=-175 in=0
    0x0000,
    0x0064,             //VCTR dx=+0 dy=+100 in=6
    0x6000,
    0x0019,             //VCTR dx=-75 dy=+25 in=0
    0x1FB5,
    0x1FE7,             //VCTR dx=+0 dy=-25 in=6
    0x6000,
    0x0000,             //VCTR dx=+150 dy=+0 in=6
    0x6096,
    0x0019,             //VCTR dx=+0 dy=+25 in=6
    0x6000,
    0x0064,             //VCTR dx=+0 dy=+100 in=0
    0x0000,
    0x0019,             //VCTR dx=+0 dy=+25 in=6
    0x6000,
    0x0000,             //VCTR dx=-150 dy=+0 in=6
    0x7F6A,
    0x1FE7,             //VCTR dx=+0 dy=-25 in=6
    0x6000,
    0x0019,             //VCTR dx=+75 dy=+25 in=0
    0x004B,
    0x0064,             //VCTR dx=+0 dy=+100 in=6
    0x6000,
    0xC000,             //VRTS

// -----
#define LABEL_vg_reticle2 0x3500 
    0x8040,             //CNTR
    0x1F51,             //VCTR dx=+0 dy=-175 in=0
    0x0000,
    0x0064,             //VCTR dx=+0 dy=+100 in=14
    0xE000,
    0x0028,             //VCTR dx=+0 dy=+40 in=6
    0x6000,
    0x0000,             //VCTR dx=-35 dy=+0 in=0
    0x1FDD,
    0x1FD8,             //VCTR dx=-40 dy=-40 in=14
    0xFFD8,
    0x0000,             //VCTR dx=+150 dy=+0 in=14
    0xE096,
    0x0028,             //VCTR dx=-40 dy=+40 in=14
    0xFFD8,
    0x0046,             //VCTR dx=+0 dy=+70 in=0
    0x0000,
    0x0028,             //VCTR dx=+40 dy=+40 in=14
    0xE028,
    0x0000,             //VCTR dx=-150 dy=+0 in=14
    0xFF6A,
    0x1FD8,             //VCTR dx=+40 dy=-40 in=14
    0xE028,
    0x0000,             //VCTR dx=+35 dy=+0 in=0
    0x0023,
    0x0028,             //VCTR dx=+0 dy=+40 in=6
    0x6000,
    0x0064,             //VCTR dx=+0 dy=+100 in=14
    0xE000,
    0xC000,             //VRTS

//; Radar frame and vision code.  Leaves beam at center of radar.
// -----
#define LABEL_vg_radar 0x353C
    0x8040,             //CNTR
    0x013C,             //VCTR dx=+68 dy=+316 in=0
    0x0044,
    0x40FC,             //SVEC dx=-8 dy=+0 in=14
    0x1FC4,             //VCTR dx=-60 dy=-60 in=0
    0x1FC4,
    0x5CE0,             //SVEC dx=+0 dy=-8 in=14
    0x0044,             //VCTR dx=-60 dy=+68 in=0
    0x1FC4,
    0x40FC,             //SVEC dx=-8 dy=+0 in=14
    0x0000,             //VCTR dx=+68 dy=+0 in=0
    0x0044,
    0x0034,             //VCTR dx=-36 dy=+52 in=10
    0xBFDC,
    0x0008,             //VCTR dx=+36 dy=+8 in=0
    0x0024,
    0x44E0,             //SVEC dx=+0 dy=+8 in=14
    0x1FF0,             //VCTR dx=+36 dy=-16 in=0
    0x0024,
    0x1FCC,             //VCTR dx=-36 dy=-52 in=10
    0xBFDC,
    0xC000,             //VRTS

//; Tank icon, indicates number of lives left.  Also used in high score list.
// -----
    0x43DD,             //SVEC dx=-6 dy=+6 in=12 <<<
    0x0003,             //VCTR dx=+9 dy=+3 in=12
    0xC009,
    0x0006,             //VCTR dx=+3 dy=+6 in=12
    0xC003,
    0x1FF7,             //VCTR dx=+36 dy=-9 in=12
    0xC024,
    0x5DDD,             //SVEC dx=-6 dy=-6 in=12
    0x0000,             //VCTR dx=-36 dy=+0 in=12
    0xDFDC,
    0x4609,             //SVEC dx=+18 dy=+12 in=0
    0x0000,             //VCTR dx=+21 dy=+0 in=12
    0xC015,
    0x1FFD,             //VCTR dx=+0 dy=-3 in=12
    0xC000,
    0x0000,             //VCTR dx=-9 dy=+0 in=12
    0xDFF7,
    0x1FF7,             //VCTR dx=+27 dy=-9 in=0
    0x001B,
    0xC000,             //VRTS
#define LABEL_vg_life_icon 0x3590
    0xAAB4             //VJSR a=0x0ab4 (0x3568)
    
// OMITTED vg_hit_cracks & vg_selftest_pat 
};

const uint16_t* Base()
{
    return ROM;
}

uint16_t LandscapeSectionOffset(uint16_t Section)
{
    return (LABEL_vg_landscape - LABEL_org)/sizeof(uint16_t)  + (Section % 8);
}

uint16_t HorizonOffset()
{
    return (LABEL_vg_horizon_line - LABEL_org)/sizeof(uint16_t);
}

uint16_t GlyphOffset(char Glyph)
{
    uint16_t idx = 0;
    if ('0' <= Glyph && Glyph <= '9')
        idx = 1 + Glyph - '0';
    else if ('A' <= Glyph && Glyph <= 'Z')
        idx = 11 + Glyph - 'A';
    else if (Glyph == '-')
        idx = 11 + 26 + 1;
    return (LABEL_vg_glyph_calls - LABEL_org)/sizeof(uint16_t) + idx;
}

uint16_t GlyphOffset(uint8_t index)
{
    return (LABEL_vg_glyph_calls - LABEL_org)/sizeof(uint16_t) + index;
}

uint16_t ReticleOffset(bool Reticle2)
{
    return ((Reticle2?LABEL_vg_reticle2:LABEL_vg_reticle1) - LABEL_org)/sizeof(uint16_t);
}

uint16_t RadarOffset()
{
    return (LABEL_vg_radar - LABEL_org)/sizeof(uint16_t);
}

uint16_t LifeOffset()
{
    return (LABEL_vg_life_icon - LABEL_org)/sizeof(uint16_t);
}
}
