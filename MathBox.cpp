#include <Arduino.h>
#include "MathBox.h"
// This code is DERIVED from this MAME source file: https://github.com/mamedev/mame/blob/master/src/mame/machine/mathbox.cpp
// Which is
//   license:BSD-3-Clause
//   copyright-holders:Eric Smith
//   mathbox.c: math box simulation (Battlezone/Red Baron/Tempest)

/*
Copyright (c) 2022, Eric Smith
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

namespace MathBox {

int16_t R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, RA, RB;
int16_t result;
// Private registers:
int16_t RC, RD, RE, RF;


void Init()
{
    R0 = R1 = R2 = R3 = R4 = R5 = R6 = R7 = R8 = R9 = RA = RB = RC = RD = RE = RF = result = 0x0000;
}

int16_t Function(int8_t command)    // "go_w"
{
    int32_t mb_temp;  /* temp 32-bit multiply results */
    int16_t mb_q;     /* temp used in division */
    int msb;
    result = 0;
    switch (command)
    {
    case 0x0b:
        RF = (int16_t)0xffff;
        R4 -= R2;
        R5 -= R3;

    step_048:

        mb_temp = ((int32_t) R0) * ((int32_t) R4);
        RC = mb_temp >> 16;
        RE = mb_temp & 0xffff;

        mb_temp = ((int32_t) -R1) * ((int32_t) R5);
        R7 = mb_temp >> 16;
        mb_q = mb_temp & 0xffff;

        R7 += RC;

        /* rounding */
        RE = (RE >> 1) & 0x7fff;
        RC = (mb_q >> 1) & 0x7fff;
        mb_q = RC + RE;
        if (mb_q < 0)
            R7++;

        result = R7;

        if (RF < 0)
            break;

        R7 += R2;

        /* fall into command 12 */

    case 0x12:

        mb_temp = ((int32_t) R1) * ((int32_t) R4);
        RC = mb_temp >> 16;
        R9 = mb_temp & 0xffff;

        mb_temp = ((int32_t) R0) * ((int32_t) R5);
        R8 = mb_temp >> 16;
        mb_q = mb_temp & 0xffff;

        R8 += RC;

        /* rounding */
        R9 = (R9 >> 1) & 0x7fff;
        RC = (mb_q >> 1) & 0x7fff;
        R9 += RC;
        if (R9 < 0)
            R8++;
        R9 <<= 1;  /* why? only to get the desired load address? */

        result = R8;

        if (RF < 0)
            break;

        R8 += R3;

        R9 &= 0xff00;

        /* fall into command 13 */

    case 0x13:
        RC = R9;
        mb_q = R8;
        goto step_0bf;

    case 0x14:
        RC = RA;
        mb_q = RB;

    step_0bf:
        RE = R7 ^ mb_q;  /* save sign of result */
        RD = mb_q;
        if (mb_q >= 0)
            mb_q = RC;
        else
        {
            RD = - mb_q - 1;
            mb_q = - RC - 1;
            if ((mb_q < 0) && ((mb_q + 1) < 0))
                RD++;
            mb_q++;
        }

    /* step 0c9: */
        /* RC = abs (R7) */
        if (R7 >= 0)
            RC = R7;
        else
            RC = -R7;

        RF = R6;  /* step counter */

        do
        {
            RD -= RC;
            msb = ((mb_q & 0x8000) != 0);
            mb_q <<= 1;
            if (RD >= 0)
                mb_q++;
            else
                RD += RC;
            RD <<= 1;
            RD += msb;
        }
        while (--RF >= 0);

        if (RE >= 0)
            result = mb_q;
        else
            result = -mb_q;
        break;

    case 0x11:
        RF = 0x0000;  /* do everything in one step */
        goto step_048;
        //break; // never reached

    case 0x1c:
        /* window test? */
        do
        {
            RE = (R4 + R7) >> 1;
            RF = (R5 + R8) >> 1;
            if ((RB < RE) && (RF < RE) && ((RE + RF) >= 0))
            { R7 = RE; R8 = RF; }
            else
            { R4 = RE; R5 = RF; }
        }
        while (--R6 >= 0);

        result = R8;
        break;

    case 0x1d:
        R2 -= R0;
        if (R2 < 0)
            R2 = -R2;

        R3 -= R1;
        if (R3 < 0)
            R3 = -R3;

        /* fall into command 1e */

    case 0x1e:
        /* result = max (R2, R3) + 3/8 * min (R2, R3) */
        if (R3 >= R2)
        { RC = R2; RD = R3; }
        else
        { RD = R2; RC = R3; }
        RC >>= 2;
        RD += RC;
        RC >>= 1;
        result = RD = (RC + RD);
        break;

    default:
        break;
    }
    return result;
}

}
