#include <Arduino.h>
#include "BZROM.h"

namespace BZROM { // Non AVG ROM data

// Mark a table entry as not-applicable, unused and not linked in
#define NA(_name) NULL
// Shape Helpers command & vectors to simplify the formatting
#define CMDS(_name) static const uint8_t _name[] PROGMEM = {
#define CMD_END 0xFF};
// These macros TRANSFORM the data to an array of 16-bit ints, for convenience
#define S16(_lo, _hi) (int16_t)(_lo | (_hi << 8))
#define SHAPE(_name, _len) static const int16_t _name[] PROGMEM = { S16((_len-1)/2, 0),
// zz, xx, yy == +X into the monitor, +Y to the right, and +Z up. :-(
#define VTX(_zzl,_zzh,_xxl,_xxh,_yyl,_yyh)  S16(_zzl,_zzh), S16(_xxl,_xxh), S16(_yyl,_yyh),

/////////////////////////////////////// SHAPE COMMANDS
// Draw commands use a byte code where each byte has the form VVVVVCCC.  The top
// 5 bits are the vertex index (0-31), the bottom 3 are the command (0-7).
// 
// The commands are:
// 
//   0 - draw point at vertex
//   1 - set intensity with vertex index value (overridden for saucer/logo)
//   2 - move without drawing
//   3 - move to center of screen, then move without drawing
//   4 - draw to vertex with intensity=1 (h/w uses intensity from STAT)
//   5 - draw scaled AVG commands from 0x347a (for projectile explosion)
//   6 - no-op
// 
// The list is terminated with 0xff.
// 

CMDS(shp_c_pyr)        0x03,0xa1,0x24,0x0c,0x04,0x1c,0x24,0x14,0x1c,0x12,0x0c,CMD_END //used for narrow & wide pyramid
CMDS(shp_c_box)        0x03,0xa1,0x0c,0x14,0x1c,0x04,0x24,0x2c,0x34,0x3c,0x24,0x2a,0x0c,0x12,0x34,0x3a, //used for tall & short box
                           0x1c,CMD_END
CMDS(shp_c_tank1)      0xbb,0xa1,/*0xb4,*/0x62,0x6c,0x72,0xa4,0x94,0x7c,0x74,0x8c,0x84,0x9c,0xac,0x8c,0x7a, // *MODIFIED* omit the radar mount
                           0x84,0x9a,0x94,0xa2,0xac,0x1b,0x04,0x24,0x3c,0x34,0x14,0x1c,0x3c,0x5c,0x54,0x34,
                           0x2c,0x4c,0x54,0x6c,0x4c,0x44,0x5c,0x64,0x44,0x24,0x2c,0x0c,0x14,0x0a,0x04,CMD_END
CMDS(shp_c_projectile) 0x03,0xe1,0x24,0x0c,0x04,0x1c,0x24,0x14,0x1c,0x12,0x0c,CMD_END
CMDS(shp_c_tread)      0x03,0x81,0x0c,0x12,0x1c,0x22,0x2c,CMD_END //used for all 8 tread objects
CMDS(shp_c_radar)      0x03,0xa1,0x0c,0x14,0x1c,0x04,0x24,0x2c,0x34,0x3c,0x24,0x3a,0x1c,CMD_END
CMDS(shp_c_prj_explos) 0x03,0x05,CMD_END
CMDS(shp_c_chunk2)     0x03,0xa1,0x0c,0x14,0x1c,0x04,0x24,0x2c,0x0c,0x2a,0x14,0x1a,0x24,0x32,0x64,0x54,
                           0x3c,0x34,0x4c,0x44,0x5c,0x6c,0x4c,0x3a,0x44,0x5a,0x54,0x62,0x6c,CMD_END
CMDS(shp_c_chunk0)     0x03,0xa1,0x1c,0x2c,0x14,0x04,0x0c,0x14,0x2c,0x24,0x0c,0x22,0x1c,CMD_END
CMDS(shp_c_chunk1)     0x03,0xa1,0x0c,0x14,0x1c,0x04,0x24,0x34,0x3c,0x2c,0x24,0x2a,0x0c,0x3a,0x14,0x1a,
                           0x34,CMD_END
CMDS(shp_c_missile)    0x6b,0xa1,0x64,0x34,0x04,0x0c,0x3c,0x44,0x4c,0x54,0x5c,0x34,0x3c,0x64,0x44,0x14,
                           0x1c,0x4c,0x64,0x54,0x24,0x2c,0x5c,0x64,0xc3,0xe1,0xbc,0xb4,0xc4,0xcc,0xbc,0xca,
                           0xb4,0x0a,0xa1,0x14,0x1a,0x24,0x2a,0x04,0x93,0x9c,0xa4,0xac,0x94,0x74,0x7c,0x84,
                           0x8c,0x74,0x7a,0x9c,0xa2,0x84,0x8a,0xac,CMD_END
CMDS(shp_c_saucer)     0x83,0xa1,0x44,0x4c,0x84,0x54,0x5c,0x84,0x64,0x6c,0x84,0x74,0x7c,0x84,0x03,0x3c,
                           0x7c,0x44,0x04,0x0c,0x4c,0x54,0x14,0x1c,0x5c,0x64,0x24,0x2c,0x6c,0x74,0x34,0x3c,
                           0x32,0x2c,0x22,0x1c,0x12,0x0c,CMD_END
CMDS(shp_c_chunk5)     0x03,0xa1,0x14,0x0c,0x1c,0x04,0x0c,0x12,0x1c,CMD_END
CMDS(shp_c_chunk4)     0x0b,0xa1,0x14,0x1c,0x3c,0x34,0x2c,0x24,0x04,0x0c,0x2c,0x32,0x14,CMD_END
CMDS(shp_c_spatter)    0x3b,0xa1,0x00,0x08,0x10,0x18,0x20,0x28,0x30,0x38,CMD_END //used for all 8 spatter objects
CMDS(shp_c_tank2)      0x03,0xa1,0x0c,0x24,0x04,0x1c,0x14,0x2c,0x1c,0x12,0x0c,0x22,0x2c,0x4b,0x54,0x34,
                           0x74,0x6c,0x4c,0x44,0x3c,0x34,0x5c,0x64,0x44,0x62,0x6c,0x72,0x5c,0x9b,0xb4,0xac,
                           0xa4,0x84,0x7c,0x94,0x8c,0x84,0x7a,0x9c,0xb2,0x94,0x8a,0xac,0xba,0xe1,0xc4,CMD_END
CMDS(shp_c_ba)         0x03,0xf1,0x0c,0x14,0x1c,0x24,0x2c,0x34,0x04,0x3a,0x44,0x4c,0x54,0x5c,0x3c,0x62,
                           0x6c,0x74,0x7c,0x64,0x82,0x8c,0x94,0x9c,0x84,CMD_END
CMDS(shp_c_ttle)       0x03,0x0c,0x14,0x1c,0x24,0x2c,0x34,0x3c,0x44,0x4c,0x54,0x5c,0x64,0x6c,0x74,0x7c,
                           0x3c,0x84,0x8c,0x94,0x9c,0xa4,0x04,CMD_END
CMDS(shp_c_zone)       0x0b,0x04,0x2c,0x24,0x1c,0x14,0x0c,0x1c,0x3c,0x34,0x0c,0x4a,0x44,0x5c,0x54,0x4c,
                           0x72,0xb4,0xbc,0xc4,0x64,0x6c,0x74,0x7c,0x84,0x8c,0x94,0x9c,0xa4,0xac,0xb4,CMD_END

static const uint8_t* const shape_code_addrs[] PROGMEM =
{
    shp_c_pyr,         //0x00
    shp_c_box,         //0x01
    shp_c_tank1,       //0x02
 NA(shp_c_projectile), //0x03
    shp_c_tread,       //0x04
    shp_c_tread,       //0x05
    shp_c_tread,       //0x06
    shp_c_tread,       //0x07
    shp_c_tread,       //0x08
    shp_c_tread,       //0x09
    shp_c_tread,       //0x0a
    shp_c_tread,       //0x0b
    shp_c_pyr,         //0x0c
 NA(shp_c_radar),      //0x0d
 NA(shp_c_prj_explos), //0x0e
    shp_c_box,         //0x0f
 NA(shp_c_chunk0),     //0x10
 NA(shp_c_chunk1),     //0x11
 NA(shp_c_chunk2),     //0x12
    shp_c_radar,       //0x13
 NA(shp_c_chunk1),     //0x14
 NA(shp_c_chunk0),     //0x15
    shp_c_missile,     //0x16
    shp_c_ba,          //0x17
 NA(shp_c_chunk1),     //0x18
 NA(shp_c_chunk4),     //0x19
 NA(shp_c_chunk0),     //0x1a
 NA(shp_c_chunk5),     //0x1b
 NA(shp_c_chunk0),     //0x1c
 NA(shp_c_chunk4),     //0x1d
    shp_c_ttle,        //0x1e
    shp_c_zone,        //0x1f
    shp_c_saucer,      //0x20
    shp_c_tank2,       //0x21
    NULL,              //0x22
    NULL,              //0x23
 NA(shp_c_spatter),    //0x24
 NA(shp_c_spatter),    //0x25
 NA(shp_c_spatter),    //0x26
 NA(shp_c_spatter),    //0x27
 NA(shp_c_spatter),    //0x28
 NA(shp_c_spatter),    //0x29
 NA(shp_c_spatter),    //0x2a
 NA(shp_c_spatter),    //0x2b
// ==========================
    NULL               //0x2c (clock)
};

/////////////////////////////////////// SHAPE VERTICES
// Shape vertex data.
// 
// Each shape is a list of signed 16-bit integers:
//   the number of vertices (N) followed by
//   zz, xx & yy coordinates (repeated N times).
// Note: this differs from the original data which used raw bytes.
//       The macros transform the data.
// 
// The table holds 45 addresses, several of which are unused.  
// The largest shape has 26 vertices.
// 
SHAPE(shp_v_naropyr, 0x1f)               //shape 0x00
               VTX(0x00,0xfe,0x00,0xfe,0xc0,0xfe) //-512,-512,-320
               VTX(0x00,0xfe,0x00,0x02,0xc0,0xfe)
               VTX(0x00,0x02,0x00,0x02,0xc0,0xfe)
               VTX(0x00,0x02,0x00,0xfe,0xc0,0xfe)
               VTX(0x00,0x00,0x00,0x00,0x40,0x01)}; //0,0,320
SHAPE(shp_v_tallbox, 0x31)               //shape 0x01
               VTX(0x00,0xfe,0x00,0xfe,0xc0,0xfe)
               VTX(0x00,0xfe,0x00,0x02,0xc0,0xfe)
               VTX(0x00,0x02,0x00,0x02,0xc0,0xfe)
               VTX(0x00,0x02,0x00,0xfe,0xc0,0xfe)
               VTX(0x00,0xfe,0x00,0xfe,0x40,0x01)
               VTX(0x00,0xfe,0x00,0x02,0x40,0x01)
               VTX(0x00,0x02,0x00,0x02,0x40,0x01)
               VTX(0x00,0x02,0x00,0xfe,0x40,0x01)};
SHAPE(shp_v_project, 0x1f)               //shape 0x03
               VTX(0xd8,0xff,0xd8,0xff,0xd0,0xff)
               VTX(0xd8,0xff,0xd8,0xff,0xf8,0xff)
               VTX(0xd8,0xff,0x28,0x00,0xf8,0xff)
               VTX(0xd8,0xff,0x28,0x00,0xd0,0xff)
               VTX(0x50,0x00,0x00,0x00,0xe4,0xff)};
SHAPE(shp_v_tank1  , 0x91)               //shape 0x02
               VTX(0x20,0xfd,0x00,0xfe,0xc0,0xfe)
               VTX(0x20,0xfd,0x00,0x02,0xc0,0xfe)
               VTX(0xc8,0x03,0x00,0x02,0xc0,0xfe)
               VTX(0xc8,0x03,0x00,0xfe,0xc0,0xfe)
               VTX(0x00,0xfc,0xc8,0xfd,0x30,0xff)
               VTX(0x00,0xfc,0x38,0x02,0x30,0xff)
               VTX(0xe0,0x04,0x38,0x02,0x30,0xff)
               VTX(0xe0,0x04,0xc8,0xfd,0x30,0xff)
               VTX(0x58,0xfd,0xa8,0xfe,0x88,0xff)
               VTX(0x58,0xfd,0x58,0x01,0x88,0xff)
               VTX(0xa8,0x02,0x58,0x01,0x88,0xff)
               VTX(0xa8,0x02,0xa8,0xfe,0x88,0xff)
               VTX(0x00,0xfe,0x58,0xff,0x30,0x00)
               VTX(0x00,0xfe,0xa8,0x00,0x30,0x00)
               VTX(0x80,0xff,0xd8,0xff,0xf8,0xff)
               VTX(0x80,0xff,0x28,0x00,0xf8,0xff)
               VTX(0x80,0x00,0x28,0x00,0xd0,0xff)
               VTX(0x80,0x00,0xd8,0xff,0xd0,0xff)
               VTX(0x60,0x04,0x28,0x00,0xf8,0xff)
               VTX(0x60,0x04,0x28,0x00,0xd0,0xff)
               VTX(0x60,0x04,0xd8,0xff,0xf8,0xff)
               VTX(0x60,0x04,0xd8,0xff,0xd0,0xff)
               VTX(0x00,0xfe,0x00,0x00,0x30,0x00)
               VTX(0x00,0xfe,0x00,0x00,0x50,0x00)}; //antenna at Z=-512 X=0 Y=80
SHAPE(shp_v_rtread3, 0x25)               //shape 0x07
               VTX(0x00,0xfc,0xc8,0xfd,0x30,0xff)
               VTX(0x00,0xfc,0x38,0x02,0x30,0xff)
               VTX(0x68,0xfc,0xdc,0xfd,0x08,0xff)
               VTX(0x68,0xfc,0x24,0x02,0x08,0xff)
               VTX(0xd0,0xfc,0xec,0xfd,0xe0,0xfe)
               VTX(0xd0,0xfc,0x14,0x02,0xe0,0xfe)};
SHAPE(shp_v_rtread2, 0x25)               //shape 0x06
               VTX(0x18,0xfc,0xcc,0xfd,0x28,0xff)
               VTX(0x18,0xfc,0x34,0x02,0x28,0xff)
               VTX(0x80,0xfc,0xe0,0xfd,0x00,0xff)
               VTX(0x80,0xfc,0x20,0x02,0x00,0xff)
               VTX(0xe8,0xfc,0xf0,0xfd,0xd8,0xfe)
               VTX(0xe8,0xfc,0x10,0x02,0xd8,0xfe)};
SHAPE(shp_v_rtread1, 0x25)               //shape 0x05
               VTX(0x34,0xfc,0xd4,0xfd,0x1c,0xff)
               VTX(0x34,0xfc,0x2c,0x02,0x1c,0xff)
               VTX(0x9c,0xfc,0xe4,0xfd,0xf4,0xfe)
               VTX(0x9c,0xfc,0x1c,0x02,0xf4,0xfe)
               VTX(0x04,0xfd,0xf8,0xfd,0xcc,0xfe)
               VTX(0x04,0xfd,0x08,0x02,0xcc,0xfe)};
SHAPE(shp_v_rtread0, 0x25)               //shape 0x04
               VTX(0x4c,0xfc,0xd8,0xfd,0x14,0xff)
               VTX(0x4c,0xfc,0x28,0x02,0x14,0xff)
               VTX(0xb4,0xfc,0xe8,0xfd,0xec,0xfe)
               VTX(0xb4,0xfc,0x18,0x02,0xec,0xfe)
               VTX(0x20,0xfd,0xfc,0xfd,0xc4,0xfe)
               VTX(0x20,0xfd,0x04,0x02,0xc4,0xfe)};
SHAPE(shp_v_ftread0, 0x25)               //shape 0x08
               VTX(0xe0,0x04,0xc8,0xfd,0x30,0xff)
               VTX(0xe0,0x04,0x38,0x02,0x30,0xff)
               VTX(0x80,0x04,0xdc,0xfd,0x08,0xff)
               VTX(0x80,0x04,0x24,0x02,0x08,0xff)
               VTX(0x20,0x04,0xec,0xfd,0xe0,0xfe)
               VTX(0x20,0x04,0x14,0x02,0xe0,0xfe)};
SHAPE(shp_v_ftread1, 0x25)               //shape 0x09
               VTX(0xc8,0x04,0xcc,0xfd,0x28,0xff)
               VTX(0xc8,0x04,0x34,0x02,0x28,0xff)
               VTX(0x68,0x04,0xe0,0xfd,0x00,0xff)
               VTX(0x68,0x04,0x20,0x02,0x00,0xff)
               VTX(0x08,0x04,0xf0,0xfd,0xd8,0xfe)
               VTX(0x08,0x04,0x10,0x02,0xd8,0xfe)};
SHAPE(shp_v_ftread2, 0x25)               //shape 0x0a
               VTX(0xb0,0x04,0xd4,0xfd,0x1c,0xff)
               VTX(0xb0,0x04,0x2c,0x02,0x1c,0xff)
               VTX(0x50,0x04,0xe4,0xfd,0xf4,0xfe)
               VTX(0x50,0x04,0x1c,0x02,0xf4,0xfe)
               VTX(0xf0,0x03,0xf8,0xfd,0xcc,0xfe)
               VTX(0xf0,0x03,0x08,0x02,0xcc,0xfe)};
SHAPE(shp_v_ftread3, 0x25)               //shape 0x0b
               VTX(0x98,0x04,0xd8,0xfd,0x14,0xff)
               VTX(0x98,0x04,0x28,0x02,0x14,0xff)
               VTX(0x38,0x04,0xe8,0xfd,0xec,0xfe)
               VTX(0x38,0x04,0x18,0x02,0xec,0xfe)
               VTX(0xd8,0x03,0xfc,0xfd,0xc4,0xfe)
               VTX(0xd8,0x03,0x04,0x02,0xc4,0xfe)};
SHAPE(shp_v_radar  , 0x31)               //shape 0x0d,0x13
               VTX(0x00,0x00,0xb0,0xff,0x50,0x00) //bottom #1 at 0,-80 height=80
               VTX(0x50,0x00,0x60,0xff,0x64,0x00)
               VTX(0x50,0x00,0x60,0xff,0x78,0x00)
               VTX(0x00,0x00,0xb0,0xff,0x8c,0x00)
               VTX(0x00,0x00,0x50,0x00,0x50,0x00) //bottom #2 at 0,80 height=80
               VTX(0x50,0x00,0xa0,0x00,0x64,0x00)
               VTX(0x50,0x00,0xa0,0x00,0x78,0x00)
               VTX(0x00,0x00,0x50,0x00,0x8c,0x00)};
SHAPE(shp_v_prj_explos, 0x07)               //shape 0x0e
               VTX(0x00,0x00,0x00,0x00,0x00,0x00)};
SHAPE(shp_v_chunk2 , 0x55)               //shape 0x12
               VTX(0xb4,0xfd,0xa8,0xfe,0x6c,0xff)
               VTX(0xb4,0xfd,0x58,0x01,0x6c,0xff)
               VTX(0x4c,0x02,0x58,0x01,0x18,0xfe)
               VTX(0x4c,0x02,0xa8,0xfe,0x18,0xfe)
               VTX(0xf0,0xfe,0x58,0xff,0xd0,0xff)
               VTX(0xf0,0xfe,0xa8,0x00,0xd0,0xff)
               VTX(0x00,0x00,0xd8,0xff,0x44,0xff)
               VTX(0x00,0x00,0x28,0x00,0x44,0xff)
               VTX(0xb4,0x00,0x28,0x00,0xe0,0xfe)
               VTX(0xb4,0x00,0xd8,0xff,0xe0,0xfe)
               VTX(0x38,0x04,0x28,0x00,0x0c,0xfe)
               VTX(0x10,0x04,0x28,0x00,0xe8,0xfd)
               VTX(0x38,0x04,0xd8,0xff,0x0c,0xfe)
               VTX(0x10,0x04,0xd8,0xff,0xe8,0xfd)};
SHAPE(shp_v_chunk0 , 0x25)               //shape 0x10,0x15,0x1a,0x1c
               VTX(0xdc,0x00,0x00,0x00,0xf0,0xfe)
               VTX(0xc0,0xfe,0xb0,0xff,0x44,0xff)
               VTX(0x54,0x01,0x50,0x00,0xa0,0xff)
               VTX(0x48,0xff,0x00,0x00,0x9c,0xfe)
               VTX(0x84,0xff,0xb0,0xff,0x00,0xff)
               VTX(0x8c,0xff,0x50,0x00,0x30,0xff)};
SHAPE(shp_v_chunk1 , 0x31)               //shape 0x11,0x14,0x18
               VTX(0x10,0xff,0x88,0xff,0xc0,0xfe)
               VTX(0x88,0xfe,0x40,0x00,0xe8,0xfe)
               VTX(0xd0,0x02,0xa0,0x00,0x80,0xfe)
               VTX(0x80,0x02,0x88,0xff,0xc0,0xfe)
               VTX(0xd8,0xff,0xc0,0xff,0xb0,0xff)
               VTX(0x00,0x00,0x20,0x00,0xc4,0xff)
               VTX(0x38,0x00,0x60,0xff,0x38,0xff)
               VTX(0x78,0x00,0xc8,0x00,0x10,0xff)};
SHAPE(shp_v_chunk5 , 0x19)               //shape 0x1b
               VTX(0xb0,0xff,0xf4,0xff,0xe0,0xfe)
               VTX(0xd8,0x01,0x70,0x00,0x50,0xfe)
               VTX(0x20,0x03,0xd4,0xff,0x0c,0x00)
               VTX(0x58,0x00,0xf0,0xff,0xf4,0xfe)};
SHAPE(shp_v_chunk4 , 0x31)               //shape 0x19,0x1d
               VTX(0xd4,0xfe,0xb8,0xff,0x48,0xff)
               VTX(0x18,0xff,0x58,0xff,0x48,0xff)
               VTX(0x18,0xff,0xf0,0xfe,0x14,0xff)
               VTX(0xd4,0xfe,0xf0,0xfe,0xe4,0xfe)
               VTX(0xa0,0xff,0xa8,0x00,0x34,0xff)
               VTX(0x28,0x00,0x0c,0x00,0x40,0xff)
               VTX(0x28,0x00,0xfc,0xfe,0xbc,0xfe)
               VTX(0xa0,0xff,0x18,0xff,0x6c,0xfe)};
SHAPE(shp_v_widepyr, 0x1f)               //shape 0x0c
               VTX(0xe0,0xfc,0xe0,0xfc,0xc0,0xfe)
               VTX(0xe0,0xfc,0x20,0x03,0xc0,0xfe)
               VTX(0x20,0x03,0x20,0x03,0xc0,0xfe)
               VTX(0x20,0x03,0xe0,0xfc,0xc0,0xfe)
               VTX(0x00,0x00,0x00,0x00,0x90,0x01)};
SHAPE(shp_v_shortbox, 0x31)               //shape 0x0f
               VTX(0x80,0xfd,0x80,0xfd,0xc0,0xfe)
               VTX(0x80,0xfd,0x80,0x02,0xc0,0xfe)
               VTX(0x80,0x02,0x80,0x02,0xc0,0xfe)
               VTX(0x80,0x02,0x80,0xfd,0xc0,0xfe)
               VTX(0x80,0xfd,0x80,0xfd,0xd8,0xff)
               VTX(0x80,0xfd,0x80,0x02,0xd8,0xff)
               VTX(0x80,0x02,0x80,0x02,0xd8,0xff)
               VTX(0x80,0x02,0x80,0xfd,0xd8,0xff)};
SHAPE(shp_v_missile, 0x9d)               //shape 0x16
               VTX(0x80,0xfe,0x90,0x00,0x00,0x00)
               VTX(0x80,0xfe,0x48,0x00,0x30,0x00)
               VTX(0x80,0xfe,0xb8,0xff,0x30,0x00)
               VTX(0x80,0xfe,0x70,0xff,0x00,0x00)
               VTX(0x80,0xfe,0xb8,0xff,0xd0,0xff)
               VTX(0x80,0xfe,0x48,0x00,0xd0,0xff)
               VTX(0xa0,0xff,0x20,0x01,0x00,0x00)
               VTX(0xa0,0xff,0xc0,0x00,0x60,0x00)
               VTX(0xa0,0xff,0x40,0xff,0x60,0x00)
               VTX(0xa0,0xff,0xe0,0xfe,0x00,0x00)
               VTX(0xa0,0xff,0x40,0xff,0xa0,0xff)
               VTX(0xa0,0xff,0xc0,0x00,0xa0,0xff)
               VTX(0x80,0x04,0x00,0x00,0x00,0x00)
               VTX(0x70,0x05,0x00,0x00,0x00,0x00)
               VTX(0x70,0xff,0x70,0xff,0x58,0xff)
               VTX(0x70,0xff,0x90,0x00,0x58,0xff)
               VTX(0x90,0x00,0x90,0x00,0x58,0xff)
               VTX(0x90,0x00,0x70,0xff,0x58,0xff)
               VTX(0xd0,0xff,0xd0,0xff,0xa4,0xff)
               VTX(0xd0,0xff,0x30,0x00,0xa4,0xff)
               VTX(0x30,0x00,0x30,0x00,0xac,0xff)
               VTX(0x30,0x00,0xd0,0xff,0xac,0xff)
               VTX(0xa0,0xff,0x00,0x00,0x60,0x00)
               VTX(0x10,0x02,0x48,0x00,0x30,0x00)
               VTX(0x10,0x02,0xb8,0xff,0x30,0x00)
               VTX(0x30,0x00,0x00,0x00,0x90,0x00)};
SHAPE(shp_v_spatter0, 0x31)               //shape 0x24
               VTX(0x00,0x00,0x34,0x00,0x4c,0xff)
               VTX(0x24,0x00,0x24,0x00,0x4c,0xff)
               VTX(0x34,0x00,0x00,0x00,0x4c,0xff)
               VTX(0x24,0x00,0xdc,0xff,0x4c,0xff)
               VTX(0x00,0x00,0xcc,0xff,0x4c,0xff)
               VTX(0xdc,0xff,0xdc,0xff,0x4c,0xff)
               VTX(0xcc,0xff,0x00,0x00,0x4c,0xff)
               VTX(0xdc,0xff,0x24,0x00,0x4c,0xff)};
SHAPE(shp_v_spatter1, 0x31)               //shape 0x25
               VTX(0x00,0x00,0x64,0x00,0x38,0xff)
               VTX(0x48,0x00,0x48,0x00,0x38,0xff)
               VTX(0x64,0x00,0x00,0x00,0x38,0xff)
               VTX(0x48,0x00,0xb8,0xff,0x38,0xff)
               VTX(0x00,0x00,0x9c,0xff,0x38,0xff)
               VTX(0xb8,0xff,0xb8,0xff,0x38,0xff)
               VTX(0x9c,0xff,0x00,0x00,0x38,0xff)
               VTX(0xb8,0xff,0x48,0x00,0x38,0xff)};
SHAPE(shp_v_spatter2, 0x31)               //shape 0x26
               VTX(0x00,0x00,0x98,0x00,0x24,0xff)
               VTX(0x6c,0x00,0x6c,0x00,0x24,0xff)
               VTX(0x98,0x00,0x00,0x00,0x24,0xff)
               VTX(0x6c,0x00,0x94,0xff,0x24,0xff)
               VTX(0x00,0x00,0x68,0xff,0x24,0xff)
               VTX(0x94,0xff,0x94,0xff,0x24,0xff)
               VTX(0x68,0xff,0x00,0x00,0x24,0xff)
               VTX(0x94,0xff,0x6c,0x00,0x24,0xff)};
SHAPE(shp_v_spatter3, 0x31)               //shape 0x27
               VTX(0x00,0x00,0xc8,0x00,0x10,0xff)
               VTX(0x90,0x00,0x90,0x00,0x10,0xff)
               VTX(0xc8,0x00,0x00,0x00,0x10,0xff)
               VTX(0x90,0x00,0x70,0xff,0x10,0xff)
               VTX(0x00,0x00,0x38,0xff,0x10,0xff)
               VTX(0x70,0xff,0x70,0xff,0x10,0xff)
               VTX(0x38,0xff,0x00,0x00,0x10,0xff)
               VTX(0x70,0xff,0x90,0x00,0x10,0xff)};
SHAPE(shp_v_spatter4, 0x31)               //shape 0x28
               VTX(0x00,0x00,0xfc,0x00,0xfc,0xfe)
               VTX(0xb0,0x00,0xb0,0x00,0xfc,0xfe)
               VTX(0xfc,0x00,0x00,0x00,0xfc,0xfe)
               VTX(0xb0,0x00,0x50,0xff,0xfc,0xfe)
               VTX(0x00,0x00,0x04,0xff,0xfc,0xfe)
               VTX(0x50,0xff,0x50,0xff,0xfc,0xfe)
               VTX(0x04,0xff,0x00,0x00,0xfc,0xfe)
               VTX(0x50,0xff,0xb0,0x00,0xfc,0xfe)};
SHAPE(shp_v_spatter5, 0x31)               //shape 0x29
               VTX(0x00,0x00,0x2c,0x01,0xe8,0xfe)
               VTX(0xd4,0x00,0xd4,0x00,0xe8,0xfe)
               VTX(0x2c,0x01,0x00,0x00,0xe8,0xfe)
               VTX(0xd4,0x00,0x2c,0xff,0xe8,0xfe)
               VTX(0x00,0x00,0xd4,0xfe,0xe8,0xfe)
               VTX(0x2c,0xff,0x2c,0xff,0xe8,0xfe)
               VTX(0xd4,0xfe,0x00,0x00,0xe8,0xfe)
               VTX(0x2c,0xff,0xd4,0x00,0xe8,0xfe)};
SHAPE(shp_v_spatter6, 0x31)               //shape 0x2a
               VTX(0x00,0x00,0x60,0x01,0xd4,0xfe)
               VTX(0x08,0x01,0x08,0x01,0xd4,0xfe)
               VTX(0x60,0x01,0x00,0x00,0xd4,0xfe)
               VTX(0x08,0x01,0xf8,0xfe,0xd4,0xfe)
               VTX(0x00,0x00,0xa0,0xfe,0xd4,0xfe)
               VTX(0xf8,0xfe,0xf8,0xfe,0xd4,0xfe)
               VTX(0xa0,0xfe,0x00,0x00,0xd4,0xfe)
               VTX(0xf8,0xfe,0x08,0x01,0xd4,0xfe)};
SHAPE(shp_v_spatter7, 0x31)               //shape 0x2b
               VTX(0x00,0x00,0x90,0x01,0xc0,0xfe)
               VTX(0x1c,0x01,0x1c,0x01,0xc0,0xfe)
               VTX(0x90,0x01,0x00,0x00,0xc0,0xfe)
               VTX(0x1c,0x01,0xe4,0xfe,0xc0,0xfe)
               VTX(0x00,0x00,0x70,0xfe,0xc0,0xfe)
               VTX(0xe4,0xfe,0xe4,0xfe,0xc0,0xfe)
               VTX(0x70,0xfe,0x00,0x00,0xc0,0xfe)
               VTX(0xe4,0xfe,0x1c,0x01,0xc0,0xfe)};
SHAPE(shp_v_saucer , 0x67)               //shape 0x20
               VTX(0x10,0xff,0x00,0x00,0xd8,0xff)
               VTX(0x60,0xff,0xa0,0x00,0xd8,0xff)
               VTX(0x00,0x00,0xf0,0x00,0xd8,0xff)
               VTX(0xa0,0x00,0xa0,0x00,0xd8,0xff)
               VTX(0xf0,0x00,0x00,0x00,0xd8,0xff)
               VTX(0xa0,0x00,0x60,0xff,0xd8,0xff)
               VTX(0x00,0x00,0x10,0xff,0xd8,0xff)
               VTX(0x60,0xff,0x60,0xff,0xd8,0xff)
               VTX(0x40,0xfc,0x00,0x00,0x50,0x00)
               VTX(0x58,0xfd,0xa8,0x02,0x50,0x00)
               VTX(0x00,0x00,0xc0,0x03,0x50,0x00)
               VTX(0xa8,0x02,0xa8,0x02,0x50,0x00)
               VTX(0xc0,0x03,0x00,0x00,0x50,0x00)
               VTX(0xa8,0x02,0x58,0xfd,0x50,0x00)
               VTX(0x00,0x00,0x40,0xfc,0x50,0x00)
               VTX(0x58,0xfd,0x58,0xfd,0x50,0x00)
               VTX(0x00,0x00,0x00,0x00,0x18,0x01)};
SHAPE(shp_v_tank2  , 0x97)               //shape 0x21
               VTX(0xb0,0x05,0x70,0x01,0xc0,0xfe)
               VTX(0x38,0xfe,0x28,0x02,0xc0,0xfe)
               VTX(0x38,0xfe,0xd8,0xfd,0xc0,0xfe)
               VTX(0xb0,0x05,0x90,0xfe,0xc0,0xfe)
               VTX(0x38,0xfe,0xc8,0x01,0xa4,0xff)
               VTX(0x38,0xfe,0x38,0xfe,0xa4,0xff)
               VTX(0x48,0x04,0x00,0x00,0xec,0xfe)
               VTX(0xf0,0xfe,0x10,0x01,0x8c,0xff)
               VTX(0x38,0xfe,0x10,0x01,0xa4,0xff)
               VTX(0x38,0xfe,0xf0,0xfe,0xa4,0xff)
               VTX(0xf0,0xfe,0xf0,0xfe,0x8c,0xff)
               VTX(0xf0,0xfe,0xb8,0x00,0x2c,0x00)
               VTX(0x38,0xfe,0xb8,0x00,0x2c,0x00)
               VTX(0x38,0xfe,0x48,0xff,0x2c,0x00)
               VTX(0xf0,0xfe,0x48,0xff,0x2c,0x00)
               VTX(0x00,0x05,0x58,0x00,0xd4,0xff)
               VTX(0x58,0x00,0x58,0x00,0xd4,0xff)
               VTX(0x58,0x00,0xa8,0xff,0xd4,0xff)
               VTX(0x00,0x05,0xa8,0xff,0xd4,0xff)
               VTX(0x00,0x05,0x58,0x00,0x00,0x00)
               VTX(0xa8,0xff,0x58,0x00,0x00,0x00)
               VTX(0xa8,0xff,0xa8,0xff,0x00,0x00)
               VTX(0x00,0x05,0xa8,0xff,0x00,0x00)
               VTX(0x38,0xfe,0x00,0x00,0x2c,0x00)
               VTX(0x38,0xfe,0x00,0x00,0x14,0x01)};
SHAPE(shp_v_ba,        0x79)               //shape 0x17
               VTX(0xe0,0x00,0x00,0x14,0x20,0x00)
               VTX(0xe0,0x00,0x00,0x0f,0x20,0x00)
               VTX(0xa0,0x02,0x80,0x0c,0x58,0x00)
               VTX(0x60,0x04,0xc0,0x0d,0x90,0x00)
               VTX(0x40,0x06,0x80,0x0c,0xc8,0x00)
               VTX(0x00,0x08,0x00,0x0f,0x00,0x01)
               VTX(0x00,0x08,0x00,0x14,0x00,0x01)
               VTX(0xa0,0x02,0x80,0x11,0x58,0x00)
               VTX(0xa0,0x02,0x40,0x10,0x58,0x00)
               VTX(0x60,0x04,0x80,0x11,0x90,0x00)
               VTX(0x40,0x06,0x40,0x10,0xc8,0x00)
               VTX(0x40,0x06,0x80,0x11,0xc8,0x00)
               VTX(0xe0,0x00,0x80,0x0c,0x20,0x00)
               VTX(0x20,0x00,0xc0,0x08,0x58,0x00)
               VTX(0xe0,0x00,0x00,0x05,0x20,0x00)
               VTX(0x00,0x08,0xc0,0x08,0x00,0x01)
               VTX(0x80,0x03,0x00,0x0a,0x70,0x00)
               VTX(0x00,0x04,0xc0,0x08,0x80,0x00)
               VTX(0x80,0x03,0x80,0x07,0x70,0x00)
               VTX(0x40,0x05,0xc0,0x08,0xa8,0x00)};
SHAPE(shp_v_ttle,      0x7f)               //shape 0x1e
               VTX(0xe0,0x00,0x80,0x02,0x20,0x00)
               VTX(0x40,0x06,0x40,0x01,0xc8,0x00)
               VTX(0x40,0x06,0x80,0xfd,0xc8,0x00)
               VTX(0xe0,0x00,0x40,0xfc,0x20,0x00)
               VTX(0x40,0x06,0x00,0xfb,0xc8,0x00)
               VTX(0x40,0x06,0x40,0xf7,0xc8,0x00)
               VTX(0xe0,0x00,0x40,0xf7,0x20,0x00)
               VTX(0xe0,0x00,0x00,0xf1,0x20,0x00)
               VTX(0xc0,0x01,0xc0,0xea,0x38,0x00)
               VTX(0xa0,0x02,0x80,0xee,0x58,0x00)
               VTX(0x80,0x03,0x80,0xee,0x70,0x00)
               VTX(0x60,0x04,0x00,0xec,0x90,0x00)
               VTX(0x40,0x05,0x80,0xee,0xa8,0x00)
               VTX(0x40,0x06,0x80,0xee,0xc8,0x00)
               VTX(0x20,0x07,0xc0,0xea,0xe0,0x00)
               VTX(0x00,0x08,0x00,0xf1,0x00,0x01)
               VTX(0xa0,0x02,0xc0,0xf4,0x58,0x00)
               VTX(0x00,0x08,0xc0,0xf4,0x00,0x01)
               VTX(0x00,0x08,0x80,0x07,0x00,0x01)
               VTX(0x40,0x06,0x80,0x07,0xc8,0x00)
               VTX(0x40,0x06,0xc0,0x03,0xc8,0x00)};
SHAPE(shp_v_zone,      0x97)               //shape 0x1f
               VTX(0x00,0xf8,0xc0,0x12,0x00,0xff)
               VTX(0x00,0xf8,0xc0,0x08,0x00,0xff)
               VTX(0xc0,0xf9,0xc0,0x0d,0x38,0xff)
               VTX(0x20,0xff,0xc0,0x08,0xe0,0xff)
               VTX(0x20,0xff,0xc0,0x12,0xe0,0xff)
               VTX(0x60,0xfd,0xc0,0x0d,0xa8,0xff)
               VTX(0x00,0xf8,0x40,0x01,0x00,0xff)
               VTX(0x20,0xff,0x40,0x01,0xe0,0xff)
               VTX(0xc0,0xf9,0x40,0x06,0x38,0xff)
               VTX(0xc0,0xf9,0xc0,0x03,0x38,0xff)
               VTX(0x60,0xfd,0xc0,0x03,0xa8,0xff)
               VTX(0x60,0xfd,0x40,0x06,0xa8,0xff)
               VTX(0x00,0xf8,0x00,0x00,0x00,0xff)
               VTX(0xa0,0xfb,0x80,0xfd,0x70,0xff)
               VTX(0x00,0xf8,0x00,0xf6,0x00,0xff)
               VTX(0xe0,0xf8,0xc0,0xef,0x20,0xff)
               VTX(0xc0,0xf9,0x80,0xf3,0x38,0xff)
               VTX(0xc0,0xfa,0x80,0xf3,0x58,0xff)
               VTX(0xa0,0xfb,0x00,0xf1,0x70,0xff)
               VTX(0x80,0xfc,0x80,0xf3,0x90,0xff)
               VTX(0x60,0xfd,0x80,0xf3,0xa8,0xff)
               VTX(0x40,0xfe,0xc0,0xef,0xc8,0xff)
               VTX(0x20,0xff,0x00,0xf6,0xe0,0xff)
               VTX(0xa0,0xfb,0x80,0xf8,0x70,0xff)
               VTX(0x20,0xff,0x00,0x00,0xe0,0xff)};

#define DIGIT_HEIGHT 400
#define DIGIT_WIDTH  300
#define DIGIT_GAP    150
#define CLOCK_ORG_XX (-(2*DIGIT_WIDTH + 3*DIGIT_GAP/2))
#define CLOCK_ORG_YY (-DIGIT_HEIGHT/2 - 100)
#if 1
// Clock vertices, 7-segment chars
// 4--5 A--B
// |  | |  |
// 2--3 8--9
// |  | |  |
// 0--1 6--7 ...
#define SEG(_dig, _row, _col)   0, CLOCK_ORG_XX + _dig*(DIGIT_WIDTH+DIGIT_GAP) + _col*DIGIT_WIDTH, CLOCK_ORG_YY + _row*DIGIT_HEIGHT/2
#define DIGIT(_dig)             SEG(_dig, 0, 0),SEG(_dig, 0, 1), SEG(_dig, 1, 0),SEG(_dig, 1, 1), SEG(_dig, 2, 0),SEG(_dig, 2, 1)
static const int16_t shp_v_clock[] PROGMEM = // shape 0x2c *NOT IN ORIGINAL!*
{ 
    S16(3*6*4, 0),   
    // zz,  xx,  yy,
    DIGIT(0),
    DIGIT(1),
    DIGIT(2),
    DIGIT(3),
};
#else
// Clock vertices, 7-segment chars
// +--+ +--+ +--+ +--+
// |  | |  | |  | |  |
// 2--+ +--+ +--+ +--+
// |  | |  | |  | |  |
// 0--1 3--+ +--+ +--+
// 0th is origin, 1st defines digit width, 2nd defines half char height, 3rd defines gap between chars
static const int16_t shp_v_clock[] PROGMEM = // shape 0x2c *NOT IN ORIGINAL!*
{ 
    S16(12, 0),   
    // zz,  xx,                                         yy,
    0,      CLOCK_ORG_XX,                               CLOCK_ORG_YY,
    0,      CLOCK_ORG_XX + DIGIT_WIDTH,                 CLOCK_ORG_YY,
    0,      CLOCK_ORG_XX,                               CLOCK_ORG_YY + DIGIT_HEIGHT/2,
    0,      CLOCK_ORG_XX + DIGIT_WIDTH + DIGIT_GAP,     CLOCK_ORG_YY,
};
#endif

static const int16_t* const shape_vertex_addrs[] PROGMEM =
{
    shp_v_naropyr,     //0x00
    shp_v_tallbox,     //0x01
    shp_v_tank1,       //0x02
 NA(shp_v_project),    //0x03
    shp_v_rtread0,     //0x04
    shp_v_rtread1,     //0x05
    shp_v_rtread2,     //0x06
    shp_v_rtread3,     //0x07
    shp_v_ftread0,     //0x08
    shp_v_ftread1,     //0x09
    shp_v_ftread2,     //0x0a
    shp_v_ftread3,     //0x0b
    shp_v_widepyr,     //0x0c
    shp_v_radar,       //0x0d
 NA(shp_v_prj_explos), //0x0e
    shp_v_shortbox,    //0x0f
 NA(shp_v_chunk0),     //0x10
 NA(shp_v_chunk1),     //0x11
 NA(shp_v_chunk2),     //0x12
    shp_v_radar,       //0x13
 NA(shp_v_chunk1),     //0x14
 NA(shp_v_chunk0),     //0x15
    shp_v_missile,     //0x16
    shp_v_ba,          //0x17
    shp_v_chunk1,      //0x18
 NA(shp_v_chunk4),     //0x19
 NA(shp_v_chunk0),     //0x1a
 NA(shp_v_chunk5),     //0x1b
 NA(shp_v_chunk0),     //0x1c
 NA(shp_v_chunk4),     //0x1d
    shp_v_ttle,        //0x1e
    shp_v_zone,        //0x1f
    shp_v_saucer,      //0x20
    shp_v_tank2,       //0x21
    NULL,              //0x22
    NULL,              //0x23
 NA(shp_v_spatter0),   //0x24
 NA(shp_v_spatter1),   //0x25
 NA(shp_v_spatter2),   //0x26
 NA(shp_v_spatter3),   //0x27
 NA(shp_v_spatter4),   //0x28
 NA(shp_v_spatter5),   //0x29
 NA(shp_v_spatter6),   //0x2a
 NA(shp_v_spatter7),   //0x2b
// ==========================
    shp_v_clock,       //0x2c  *NOT IN ORIGINAL!*
};

const uint8_t* ShapeCodes(uint8_t shapeIdx)
{
    return pgm_read_ptr_near(shape_code_addrs + shapeIdx);
}

const int16_t* ShapeVertices(uint8_t shapeIdx)
{
    return pgm_read_ptr_near(shape_vertex_addrs + shapeIdx);
}

/////////////////////////////////////// SINE TABLE
// 
// Table of sines.  There are 65 entries, representing the first quadrant (angles
// 0-90, inclusive).  Each entry is a 16-bit signed value:
// 
//   sine_tab[n] = 32768 * sin(alpha)
// 
// Because this is only the first quadrant, all values are positive.
// 
static const int16_t sine_tab[] PROGMEM =
{
    0x0000,
    0x0324,
    0x0647,
    0x096A,
    0x0C8B,
    0x0FAB,
    0x12C8,
    0x15E2,
    0x18F8,
    0x1C0B,
    0x1F19,
    0x2223,
    0x2528,
    0x2826,
    0x2B1F,
    0x2E11,
    0x30FB,
    0x33DE,
    0x36BE,
    0x398C,
    0x3C56,
    0x3F17,
    0x41CE,
    0x447A,
    0x471C,
    0x49B4,
    0x4C3F,
    0x4EBF,
    0x5133,
    0x539B,
    0x55F5,
    0x5842,
    0x5A82,
    0x5CB4,
    0x5ED7,
    0x60EC,
    0x62F2,
    0x64EB,
    0x66CF,
    0x68A6,
    0x6A6D,
    0x6C24,
    0x6DC4,
    0x6F5F,
    0x70E2,
    0x7255,
    0x73B5,
    0x7504,
    0x7641,
    0x776C,
    0x7884,
    0x798A,
    0x7A7D,
    0x7B5D,
    0x7C2A,
    0x7CE3,
    0x7D8A,
    0x7E1D,
    0x7E9D,
    0x7F09,
    0x7F62,
    0x7FA7,
    0x7FD8,
    0x7FF6,
    0x7FFF,             //can't represent +32768, but this will do
};

/////////////////////////////////////// OBSTACLES
// Obstacle coordinates.  There are 21 of them.
// 
// See also the type/facing data at 0x3fcc and the projectile collision diameters
// at 0x6139.
// 
static const uint16_t obstacle_ZZ_pos[] PROGMEM =
{         
    0x2000,
    0x0000,
    0x0000,
    0x4000,
    0x8000,
    0x8000,
    0x8000,
    0x4000,
    0x3000,
    0xC000,
    0xF700,
    0xC800,
    0xD800,
    0x9400,
    0x9800,
    0xE800,
    0x7000,
    0x7800,
    0x4000,
    0x2400,
    0x2C00,
};

static const uint16_t obstacle_XX_pos[] PROGMEM =
{
    0x2000,
    0x4000,
    0x8000,
    0x8000,
    0x8000,
    0x4000,
    0x0000,
    0x0000,
    0x5000,
    0x1800,
    0x4400,
    0x4000,
    0x8C00,
    0x0C00,
    0xE800,
    0xE400,
    0x9C00,
    0xCC00,
    0xB400,
    0xBC00,
    0xF400,
};

// Obstacle type and facing data.
// 
// Each entry is two bytes.  The first is the object (shape) type, the second is the
// facing angle.  (All obstacles are symmetric, so every 90-degree turn
// effectively returns to zero.)
// 
// See also the map position data at 0x7681 and the projectile collision diameters
// at 0x6139.
// 
static const uint8_t obstacle_t_f[] PROGMEM =
{
 // type, face (21 entries)
    0x0C, 0x00,           // wide pyramid
    0x0F, 0x10,           // short box
    0x0C, 0x20,           // wide pyramid
    0x0F, 0x40,           // short box
    0x0C, 0x18,           // wide pyramid
    0x00, 0x28,           // narrow pyramid
    0x01, 0x30,           // tall box
    0x00, 0x38,           // narrow pyramid
    0x01, 0x40,           // tall box
    0x0F, 0x48,           // short box
    0x0C, 0x50,           // wide pyramid
    0x00, 0x58,           // narrow pyramid
    0x01, 0x60,           // tall box
    0x0F, 0x68,           // short box
    0x0C, 0x70,           // wide pyramid
    0x00, 0x78,           // narrow pyramid
    0x01, 0x80,           // tall box
    0x0F, 0x88,           // short box
    0x0C, 0x90,           // wide pyramid
    0x00, 0x98,           // narrow pyramid
    0x01, 0xA0,           // tall box
    0xFF,                 // end of list
};

/////////////////////////////////////// PRE-DEFINED STRINGS
// https://6502disassembly.com/va-battlezone/Battlezone.html#Symstr_addr_en
// position, char indices, high bit set on last
//+128,+320 'SCORE ' *MODIFIED* 7 digits of score OMITTED
const uint8_t ScoreStr[]   PROGMEM = {0x20,0x50, 0x3A,0x1A,0x32,0x38,0x1E,0x80}; 
//+128,+280 'HIGH SCORE  ' *MODIFIED* 7 digits of score OMITTED
const uint8_t HighScoreStr[] PROGMEM = {0x20,0x46, 0x24,0x26,0x22,0x24,0x00,0x3A,0x1A,0x32,0x38,0x1E,0x00,0x00,0x80};
//-168,-240 '(C)(P)  ATARI 1980'
const uint8_t AtariStr[] PROGMEM = {0xD6,0xC4, 0x4E,0x50,0x00,0x00,0x16,0x3C,0x16,0x38,0x26,0x00,0x04,0x14,0x12,0x82};
//-112,+160 'HIGH SCORES'
const uint8_t HighScoresStr[] PROGMEM = {0xE4,0x28, 0x24,0x26,0x22,0x24,0x00,0x3A,0x1A,0x32,0x38,0x1E,0xBA};
//-112,+96 'GAME OVER'
const uint8_t GameOverStr[] PROGMEM = {0xE4,0x18, 0x22,0x16,0x2E,0x1E,0x00,0x32,0x40,0x1E,0xB8};
//-136,+0 'PRESS START'
const uint8_t PressStartStr[] PROGMEM = {0xDE,0x00, 0x34,0x38,0x1E,0x3A,0x3A,0x00,0x3A,0x3C,0x16,0x38,0xBC};
//-440,+296 'ENEMY TO '
const uint8_t EnemyToStr[] PROGMEM = {0x92,0x4A, 0x1E,0x30,0x1E,0x2E,0x46,0x00,0x3C,0x32,0x80};
//(rel) 'LEFT'
const uint8_t LeftStr[] PROGMEM = {0x00,0x00, 0x2C,0x1E,0x20,0xBC};
//(rel) 'RIGHT'
const uint8_t RightStr[] PROGMEM = {0x00,0x00, 0x38,0x26,0x22,0x24,0xBC};
//(rel) 'REAR'
const uint8_t RearStr[] PROGMEM = {0x00,0x00, 0x38,0x1E,0x16,0xB8};
//-440,+360 'ENEMY IN RANGE'
const uint8_t EnemyInRangeStr[] PROGMEM = {0x92,0x5A, 0x1E,0x30,0x1E,0x2E,0x46,0x00,0x26,0x30,0x00,0x38,0x16,0x30,0x22,0x9E};
//-440,+328 'MOTION BLOCKED BY OBJECT'
const uint8_t MotionBlockedStr[] PROGMEM = {0x92,0x52, 0x2E,0x32,0x3C,0x26,0x32,0x30,0x00,0x18,0x2C,0x32,0x1A,0x2A,0x1E,0x1C,
                                            0x00,0x18,0x46,0x00,0x32,0x18,0x28,0x1E,0x1A,0xBC};
}
