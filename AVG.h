#pragma once

namespace AVG   // Analog Vector Graphics
{
    const int16_t beamWidth = 1024; 
    const int16_t beamHeight = 768; 

    extern byte STAT;
    void Run(uint16_t offset, int16_t scalePercent = 100);

    void MoveBeamTo(int16_t X, int16_t Y);
    void MoveBeam(int16_t dX, int16_t dY);
    void DrawBeam(int16_t dX, int16_t dY);

    void ROMStr(const uint8_t* pgmStr, int16_t scalePercent = 100, int16_t xOverride = -1, int16_t yDeltaOverride = 0);
    // read a char from a pointer to PROGMEM or RAM
    typedef char (*CharReader)(const char*);
    char ProgMemCharReader(const char* a);
    char StdCharReader(const char* a);
    const char* Str(const char* pStr, CharReader charReader, int16_t scalePercent = 100, int16_t x = -1, int16_t y = -1);
};

