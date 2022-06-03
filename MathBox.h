#pragma once

namespace MathBox
{
    enum Commands {ROT_Z = 0x0B, SCREEN_X = 0x11, ROT_X = 0x12, DIVIDE_B7 = 0x14, CALC_DIST = 0x1D, CALC_HYPOT = 0x1E};

    void Init();

    int16_t Function(int8_t command);

    extern int16_t R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, RA, RB;
    extern int16_t result;
};
