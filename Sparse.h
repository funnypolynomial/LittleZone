#pragma once

// Sparse pixel representation, the key to making this work on an Arduino
namespace Sparse
{
  void Clear(int originY);
  void Line(int x0, int y0, int x1, int y1, int minX, int maxX, byte dotPattern = 0x00);
  void Paint(int originX, int originY, int minRow, int maxRow, byte*& pRowStart, word fg);
  void PaintHorizon(int originX, int originY, int minRow, int maxRow, byte*& pRowStart, word fg, word bg);
  extern uint16_t highWater;
};
