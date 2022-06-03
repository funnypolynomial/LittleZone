#pragma once

#define pgm_read_int16_near(address_short)  (int16_t)(pgm_read_word_near(address_short))

// Battlezone ROM. Strings, 3D Shapes etc
namespace BZROM
{
    // Shapes
    const uint8_t* ShapeCodes(uint8_t shapeIdx);
    const int16_t* ShapeVertices(uint8_t shapeIdx);

    // Sine table
    extern const int16_t sine_tab[];

    // Obstacles
    extern const uint16_t obstacle_ZZ_pos[];
    extern const uint16_t obstacle_XX_pos[];
    extern const uint8_t  obstacle_t_f[];

    // Pre-defined strings
    extern const uint8_t ScoreStr[];
    extern const uint8_t HighScoreStr[];
    extern const uint8_t AtariStr[];
    extern const uint8_t HighScoresStr[];
    extern const uint8_t GameOverStr[];
    extern const uint8_t PressStartStr[];
    extern const uint8_t EnemyToStr[];
    extern const uint8_t LeftStr[];
    extern const uint8_t RightStr[];
    extern const uint8_t RearStr[];
    extern const uint8_t EnemyInRangeStr[];
    extern const uint8_t MotionBlockedStr[];
};

