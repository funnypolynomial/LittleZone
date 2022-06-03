#pragma once

// The near clipping plane is further away for the complex saucer
#define SHAPES_NEAR_PLANE(_type) ((_type == Shapes::saucer)?0x06FF:0x03FF)
#define SHAPES_FAR_PLANE                                           0x7AFF
namespace Shapes
{
    enum Objects 
    {
        tallPyr = 0x00,
        tallBox = 0x01,
        tank1   = 0x02,
        lowPyr  = 0x0C,
        // NOT IMPLEMENTED: radar  = 0x0D,
        lowBox  = 0x0F,
        missile = 0x16,
        BA      = 0x17, TTLE = 0x1E, ZONE = 0x1F,
        saucer  = 0x20,
        tank2   = 0x21,

        clock   = 0x2c,

        NoObj   = 0x80
    };    

    void Init();

    uint32_t DrawVisibleObstacles(uint32_t obstacles);
    bool DrawObject(uint8_t type, uint8_t facing, int16_t centreXX, int16_t centreZZ, int16_t& range, bool& visible, uint8_t& dirn);
    void DrawTransformedObject(uint8_t type);
    int16_t Sin(uint8_t angle);
    int16_t Cos(uint8_t angle);

    extern int16_t objectDeltaYY;
    extern bool nearClip;
};

