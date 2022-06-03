#include <Arduino.h>
#include "Game.h"
#include "AVG.h"
#include "Shapes.h"
#include "MathBox.h"
#include "BZROM.h"

namespace Shapes
{

int16_t objectDeltaYY;
bool nearClip;
void Init()
{
    // "For correct behavior, the division routine should iterate 16 times, but Battlezone sets the value to 10. 
    // This effectively scales the screen coordinates down by a factor of 64. "
    MathBox::R6 = 10;
    objectDeltaYY = 0;
    nearClip = false;
}

int16_t Sin(uint8_t angle)
{
    // see https://6502disassembly.com/va-battlezone/Battlezone.html#SymCalcSine512
    bool negate = false;
    int16_t result = 0x0000;
    if (angle > 0x7F)
    {
        angle &= 0x7F;
        negate = true;
    }
    if (angle <= 0x7F)
    {
        if (angle > 0x40)
            angle = (0x7F) - angle + 1;
        result = pgm_read_int16_near(BZROM::sine_tab + angle);   
    }
    if (negate)
        result = -result;
    return result;
}

int16_t Cos(uint8_t angle)
{
    return Sin(angle + 0x40);
}

int16_t CalcScreenY(int16_t vertexYY)
{
    // see addr 0x5E05
    // NOT IMPLEMENTED: horizon adjustment!
    MathBox::RB = vertexYY;
    return MathBox::Function(MathBox::DIVIDE_B7);
}

int16_t screen_coords[2*26];

bool TransformObject(uint8_t type, uint8_t facing, int16_t centreXX, int16_t centreZZ, bool& straddlesCentre)
{
    // see https://6502disassembly.com/va-battlezone/Battlezone.html#SymTxfrmObject
    const int16_t* pShape = BZROM::ShapeVertices(type);
    uint8_t theta = (Game::plyr_facing >> 1) - facing + 0x80;
    //uint8_t distantIntensity = (centreZZ >> 8) & 0xF0;
    MathBox::R0 = -Cos(theta);
    MathBox::R1 = +Sin(theta);
    MathBox::R2 = centreZZ;
    MathBox::R3 = centreXX;
    MathBox::RA = 0;
    int16_t len = pgm_read_int16_near(pShape++);
    int16_t* pScreenXY = screen_coords;
    int16_t screenMinX = +0x7FFF, screenMaxX = -0x7FFF;
    while (len)
    {
        MathBox::R4 = pgm_read_int16_near(pShape++);    // ZZ
        MathBox::R5 = pgm_read_int16_near(pShape++);    // XX
        int16_t  YY = pgm_read_int16_near(pShape++);    // YY
        if ((type & 0x10) == 0x10)
            YY += objectDeltaYY;
        int16_t screen_x = -MathBox::Function(MathBox::SCREEN_X);
        int16_t screen_y = CalcScreenY(YY);
        *pScreenXY++ = screen_x;
        *pScreenXY++ = screen_y;
        len -= 3;
        if (screen_x > screenMaxX)
            screenMaxX = screen_x; 
        if (screen_x < screenMinX)
            screenMinX = screen_x; 
    }
    straddlesCentre = screenMinX < 0 && screenMaxX > 0;
    //  Don't draw if entirely outside beam area
    return screenMaxX >= -AVG::beamWidth/2 && screenMinX <= +AVG::beamWidth/2;
}

void DrawTransformedObject(uint8_t type)
{
    if (type == clock)
    {
        Game::DrawClockObject(screen_coords);
        return;
    }
    const uint8_t* pShape = BZROM::ShapeCodes(type);
    uint8_t shape = pgm_read_byte_near(pShape++);
    int16_t cX = 0, cY = 0;
    while (shape != 0xFF)
    {
        int16_t vertex = shape >> 3;
        int16_t* pScreenXY = screen_coords + 2*vertex;
        int16_t X = *pScreenXY++;
        int16_t Y = *pScreenXY;
        switch (shape & 0b00000111)
        {
            case 0:    // draw point at vertex
                // NOT IMPLEMENTED
                break;
            case 1:    // set intensity with vertex index value (overridden for saucer/logo)
                // NOT IMPLEMENTED
                break;
            case 2:    // move without drawing
                AVG::MoveBeam(X - cX, Y - cY);
                cX = X;
                cY = Y;
                break;
            case 3:    // move to center of screen, then move without drawing
                AVG::MoveBeamTo(0, 0);
                AVG::MoveBeam(X, Y);
                cX = X;
                cY = Y;
                break;
            case 4:    // draw to vertex with intensity=1 (h/w uses intensity from STAT)
                AVG::DrawBeam(X - cX, Y - cY);
                cX = X;
                cY = Y;
                break;
            default:
                break;
        }
        shape = pgm_read_byte_near(pShape++);
    }
}

int16_t view_pos_XX;
int16_t view_pos_ZZ;

bool Rotate(uint8_t type)
{
    // returns true if the object is visible
    view_pos_ZZ = MathBox::Function(MathBox::ROT_Z);
    // Handle cases where 2*view_pos_ZZ OVERFLOWS and changes sign.
    // Apparently not an issue with the 6502 version!
    if (view_pos_ZZ <= -16384 || view_pos_ZZ >= +16384)
    {
        //TRACE("Overflow %i\n", view_pos_ZZ);
        view_pos_ZZ = -1;   // overflow. skip it, either behind or a long way away (beyond far plane)
    }
    else
        view_pos_ZZ *= 2;
    if (view_pos_ZZ >= 0)   // not behind us
    {
        if (SHAPES_NEAR_PLANE(type) <= view_pos_ZZ && view_pos_ZZ <= SHAPES_FAR_PLANE) // between near & far plane
        {
            view_pos_XX = MathBox::Function(MathBox::ROT_X);
            // Another possible overflow!
            int16_t abs_pos_XX = abs(view_pos_XX);
            if (view_pos_XX <= -16384 || view_pos_XX >= +16384)
            {
                view_pos_XX = abs_pos_XX = 32767;
            }
            else
            {
                view_pos_XX *= 2;
                abs_pos_XX *= 2;
            }
            if (view_pos_ZZ > abs_pos_XX)    // 90deg POV, simple but crude, it's actually 45deg, DrawObject clips too
            {
                if (view_pos_ZZ <= (SHAPES_NEAR_PLANE(type) + 0x01FF))  // arbitrary "closeness", rough because of 90deg POV
                    nearClip = true;
                return true;
            }
        }
    }
    return false;
}

uint32_t DrawVisibleObstacles(uint32_t obstacles)
{
    // obstacles has a bit set for obstacles to check
    // returns a bitset of actaully visible obstacles
    // see https://6502disassembly.com/va-battlezone/Battlezone.html#SymVLGenerate

    uint32_t visible = 0;
    uint32_t mask = 1;
    // Start by setting R0/R1/R2/R3 to the player's position and facing.
    uint8_t theta = Game::plyr_facing >> 1;
    int16_t cosTheta = +Cos(theta);
    int16_t sinTheta = -Sin(theta);
    // see https://6502disassembly.com/va-battlezone/Battlezone.html#SymVLAddObstacles
    int idx = 0;
    uint8_t vis_obj_type;
    uint8_t vis_obj_face;

    do
    {
        vis_obj_type = pgm_read_byte_near(BZROM::obstacle_t_f + idx);
        if (vis_obj_type == 0xFF)
            break;  // end of obstacle list
        if (mask & obstacles)
        {
            vis_obj_face = pgm_read_byte_near(BZROM::obstacle_t_f + idx + 1);
            MathBox::R0 = cosTheta;
            MathBox::R1 = sinTheta;
            MathBox::R2 = Game::unit_pos_ZZ;
            MathBox::R3 = Game::unit_pos_XX;
            MathBox::R4 = pgm_read_int16_near(BZROM::obstacle_ZZ_pos + (idx / 2));
            MathBox::R5 = pgm_read_int16_near(BZROM::obstacle_XX_pos + (idx / 2));
            bool straddlesCentre;
            if (Rotate(vis_obj_type))
                if (TransformObject(vis_obj_type, vis_obj_face, view_pos_XX, view_pos_ZZ, straddlesCentre))
                {
                    visible |= mask;
                    DrawTransformedObject(vis_obj_type);
                }
        }
        mask <<= 1;
        idx += 2;
    } while (true);
    return visible;
}

bool DrawObject(uint8_t type, uint8_t facing, int16_t centreXX, int16_t centreZZ, int16_t& range, bool& visible, uint8_t& dirn)
{
    // returns true if the type is in the sights and fills in visible and dirn
    bool inSights = false;
    uint8_t theta = Game::plyr_facing >> 1;
    MathBox::R0 = +Cos(theta);
    MathBox::R1 = -Sin(theta);
    MathBox::R2 = Game::unit_pos_ZZ;
    MathBox::R3 = Game::unit_pos_XX;
    MathBox::R4 = centreZZ;
    MathBox::R5 = centreXX;
    range = 0;
    visible = false;
    if (Rotate(type))
        if (TransformObject(type, facing, view_pos_XX, view_pos_ZZ, inSights))
        {
            range = view_pos_ZZ;
            visible = true;
            DrawTransformedObject(type);
        }
    if (inSights)
        dirn = 0;
    else if (view_pos_ZZ < 0)
        dirn = 3;   // rear
    else if (view_pos_XX > 0)
        dirn = 1;   // left
    else if (view_pos_XX < 0)
        dirn = 2; // right
    return inSights;// Original is more complex, see https://6502disassembly.com/va-battlezone/Battlezone.html#SymCalcAngleToPlayer
}

}
