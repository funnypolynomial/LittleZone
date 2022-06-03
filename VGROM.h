#pragma once

// Vector Generator ROM. 2D objects
namespace VGROM
{
    const uint16_t BaseAddress = 0x0800;    // I can't explain why this is so, see the disassembly!
    const uint16_t* Base();

    uint16_t LandscapeSectionOffset(uint16_t Section); // Section 0..7
    uint16_t HorizonOffset();

    const int GlyphWidth  = 24;
    const int GlyphHeight = 32;
    uint16_t GlyphOffset(char Glyph);

    uint16_t GlyphOffset(uint8_t index);

    uint16_t ReticleOffset(bool Reticle2);
    uint16_t RadarOffset();
    uint16_t LifeOffset();
};
