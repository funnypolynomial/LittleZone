#include "Arduino.h"
#include "Game.h"
#include "Sparse.h"

namespace Sparse {
/*
  Sparse pixels! The key to making this work on an Arduino.

  Each row is a contiguous block of bytes representing runs of horizontal pixels. The runs are sorted from left to right (but they may grow into each other; Draw() handles this).  
  All the run data is in a single array of bytes, pool[]. rows[r] stores the number of bytes in row r.
  Setting a pixel at (row, col) is a matter of finding the start of the row block by adding values in rows then scanning across the block to find where col belongs and 
  either inserting a new single pixel entry, or extending an existing run. Adding a byte to a run means shuffling up all the subsequent bytes.
  Drawing is a matter of scanning all the rows and run data and sending bands of black/white pixel data to the LCD.

  There are two variations of the algorithm, selected by ENABLE_SPARSE_WIDE
  sparse-A
  A run is encoded as 1 or 2 bytes. The low 7-bits of first byte is the start column. 
  If the high bit is clear, this is a 1-byte representation and the run length is 1.
  If the high bit is set, this is a 2-byte representation and the run length is the following byte.
  Columns are limited to 0..127 so the ship is drawn in two halves. This causes some "tearing" but it does require a modest amount of RAM. 
  
  sparse-B (WIDE)
  This is a response to the tearing effect.
  A run is encoded as 1, 2 or 3 bytes.  The first byte is the start column.
  If the second byte is greater than the column, or the first byte is the last in a row block, this is a 1-byte representation and the run length is 1.
  If the second byte is less than or equal to the column, this is a 2-byte representation and the second byte is the run length.
  If the second byte is NUL (0x00) this is a 3-byte representation and the run length is the third byte.
  Columns are limited to 0..254 but that is not an issue here, so the whole ship can be drawn at once, but the pool does need to be larger. 

  Consider this triangle:
      012345678901234567890
    0|      ***
    1|     *   ***
    2|    *       ***
    3|   *           ***
    4|  *      ******
    5| ********

  Sparse-A would encode this as:
    rows  pool bytes
    [2]   {134,3},
    [3]            {5}, {137,3},
    [3]                          {4}, {140,3},
    [3]                                       {3}, {143,3},
    [3]                                                     {2}, {137,6},
    [2]                                                                   {129,8},
*/

// Special byte values
#define INF 0xFFFF
#define NUL 0x00

// Drawn in full
#define PAINT_WINDOW_ORIGIN_X 0
#define PAINT_WINDOW_ORIGIN_Y 0
const int SPARSE_ROWS = SCREEN_LOWER_HEIGHT;
#if SCREEN_WIDTH <= 255
const int SPARSE_COLS = SCREEN_WIDTH;
#else
const int SPARSE_COLS = 255;
#endif
const int SPARSE_POOL_SIZE = 1050;

// the x at _ptr in the pool
#define SPARSE_GET_X(_ptr)      (*_ptr)
// the number of pixels in the run at _ptr in the pool
#define SPARSE_GET_LEN(_last, _ptr)    (((_last) || *_ptr < *(_ptr+1)) ? 1 : ((*(_ptr+1) == NUL) ? *(_ptr+2) : *(_ptr+1)))
// number of bytes in the entry at _ptr in the pool
#define SPARSE_GET_SIZE(_last, _ptr)   (((_last) || *_ptr < *(_ptr+1)) ? 1 : ((*(_ptr+1) == NUL) ? 3 : 2))

byte rows[SPARSE_ROWS]; // number of bytes in row's sparse representation
byte pool[SPARSE_POOL_SIZE]; // representations go here ("values", "cols"!)
uint16_t pool_top;    // index to next free byte
byte* cachePtr = NULL;
uint16_t cacheY = INF;
uint16_t cacheX = INF;
bool cacheLast = false;
const int numMidRows = 4;
uint16_t sumToMidRow[numMidRows]; // cache the sum of the rows up to n*SPARSE_ROWS/numMidRows
uint16_t highWater = 0;
uint16_t rowOffset = 0;
#ifdef DEBUG  
#define SET_HIGHWATER(_rhs) highWater = _rhs
#else
#define SET_HIGHWATER(_rhs)
#endif

static bool Insert(byte y, byte* pValue, byte value, byte value2 = 0)
{
  // inserts a new byte *at* pValue. Bytes above are shuffled
  // does most of the work, shuffles up the pool to make room for the new value
  // true if there was room
  // Also handles two bytes, if value2 != 0

  if (value2)
  {
    // special case, insert NUL & 2
    if (pool_top < SPARSE_POOL_SIZE - 1)
    {
      // there's room for 2 bytes
      // shuffle pool bytes up
      memmove(pValue + 2, pValue, (pool + pool_top) - pValue); // ***THIS FIX NOT IN ElitePetite***
      *pValue = value;
      *(pValue + 1) = value2;
      pool_top += 2;
      rows[y] += 2;
      for (int midRow = 1; midRow < numMidRows; midRow++)
        if (y < midRow*SPARSE_ROWS/numMidRows)
          sumToMidRow[midRow] += 2;
      SET_HIGHWATER(max(highWater, pool_top));
      return true;
    }
  }
  else if (pool_top < SPARSE_POOL_SIZE)
  {
    // there's room
    // shuffle pool bytes up
    memmove(pValue + 1, pValue, (pool + pool_top) - pValue);
    *pValue = value;
    pool_top++;
    rows[y]++;
    for (int midRow = 1; midRow < numMidRows; midRow++)
      if (y < midRow*SPARSE_ROWS/numMidRows)
        sumToMidRow[midRow]++;
    SET_HIGHWATER(max(highWater, pool_top));
    return true;
  }
  SET_HIGHWATER(SPARSE_POOL_SIZE);
  return false;
}

static void Pixel(byte x, byte y)
{
  // set the pixel in the sparse data
  y -= rowOffset;
#ifndef SCREEN_FULL_SIZE // avoid a warning
  if (y >= SPARSE_ROWS || x >= SPARSE_COLS)
    return;
#endif

  if (cacheY == y && cacheX == x)
  {
    // cache the last location to quickly append to horizontal sequences
    byte* thisPtr = cachePtr;
    byte bytes = SPARSE_GET_SIZE(cacheLast, thisPtr);
    x = SPARSE_GET_X(thisPtr);
    cacheLast = false;
    if (bytes == 1)
    {
      if (x >= 2) // 2 bytes will work
      {
        if (!Insert(y, thisPtr + 1, 2))  // len=2
          cacheY = INF;
      }
      else // need 3
      {
        if (!Insert(y, thisPtr + 1, NUL, 2))  // len=2, as 3 bytes ***THIS FIX NOT IN ElitePetite***
          cacheY = INF;
      }
    }
    else if (bytes == 2)
    {
      thisPtr++;
      if (x > *thisPtr)
        (*thisPtr)++;   // len++
      else if (Insert(y, thisPtr++, NUL)) // need to go to 3 bytes
        (*thisPtr)++;   // len++
      else
        cacheY = INF;
    }
    else // 3 bytes
    {
      thisPtr += 2;
      (*thisPtr)++;   // len++
    }
    cacheX++;
    return;
  }

  byte* thisPtr = pool;
  byte* rowPtr = rows;
  int startRow = 0;
  // check short-cuts
  for (int midRow = numMidRows-1; midRow; midRow--)
    if (y >= midRow*SPARSE_ROWS/numMidRows)
    {
      startRow = midRow*SPARSE_ROWS/numMidRows;
      thisPtr += sumToMidRow[midRow];
      rowPtr  += startRow;
      break;
    }
  // add up the bytes per row to find the start of the data for row y
  for (int row = startRow; row < y; row++, rowPtr++)
    thisPtr += *rowPtr;

  byte bytesInRow = *rowPtr;
  while (bytesInRow && ((int)x - (int)(SPARSE_GET_X(thisPtr) + SPARSE_GET_LEN(bytesInRow == 1, thisPtr))) >= 1)
  {
    byte bytes = SPARSE_GET_SIZE(bytesInRow == 1, thisPtr);
    bytesInRow -= bytes;
    thisPtr += bytes;
  }
  cacheY = INF;
  if (!bytesInRow) // append. got to the end without finding a place to insert/update
  {
    Insert(y, thisPtr, x);
    cacheX = x + 1;
    cacheY = y;
    cachePtr = thisPtr;
    cacheLast = true;
  }
  // thisPtr is an item with an x larger than ours, or within 1 of ours
  else if (x == SPARSE_GET_X(thisPtr) - 1)  // expand thisPtr left
  {
    byte bytes = SPARSE_GET_SIZE(bytesInRow == 1, thisPtr);
    (*thisPtr)--;     // x--
    if (bytes == 1)
    {
      if (x >= 2) // 2 bytes will work
        Insert(y, thisPtr + 1, 2);  // len=2
      else // need 3
        Insert(y, thisPtr + 1, NUL, 2);  // len=2, as 3 bytes
    }
    else if (bytes == 2)
    {
      thisPtr++;
      if (x > *thisPtr)
        (*thisPtr)++;   // len++
      else if (Insert(y, thisPtr++, NUL)) // need to go to 3 bytes
        (*thisPtr)++;   // len++
    }
    else // 3 bytes
    {
      thisPtr += 2;
      (*thisPtr)++;   // len++
    }
  }
  else if (x == (SPARSE_GET_X(thisPtr) + SPARSE_GET_LEN(bytesInRow == 1, thisPtr))) // expand thisPtr right
  {
    byte bytes = SPARSE_GET_SIZE(bytesInRow == 1, thisPtr);
    x = SPARSE_GET_X(thisPtr);
    if (bytes == 1)
    {
      if (x >= 2) // 2 bytes will work
        Insert(y, thisPtr + 1, 2);  // len=2
      else // need 3
        Insert(y, thisPtr + 1, NUL, 2);  // len=2, as 3 bytes  ***THIS FIX NOT IN ElitePetite***
    }
    else if (bytes == 2)
    {
      thisPtr++;
      if (x > *thisPtr)
        (*thisPtr)++;   // len++
      else if (Insert(y, thisPtr++, NUL)) // need to go to 3 bytes
        (*thisPtr)++;   // len++
    }
    else // 3 bytes
    {
      thisPtr += 2;
      (*thisPtr)++;   // len++
    }
  }
  else if (x < SPARSE_GET_X(thisPtr))     // insert before thisPtr
  {
    Insert(y, thisPtr, x);
    cacheX = x + 1;
    cacheY = y;
    cachePtr = thisPtr;
    cacheLast = bytesInRow == 1;
  }
}


void Clear(int originY)
{
  // prepare for another render
  rowOffset = originY;
  memset(rows, 0x00, sizeof(rows));
  memset(sumToMidRow, 0x00, sizeof(sumToMidRow));
  pool_top = 0;
}

void Line(int x0, int y0, int x1, int y1, int minX, int maxX, byte dotPattern)
{
  // Draw a line {x0, y0} to {x1, y1}. Clipped to minX..maxX
  // Always drawn left-to-right
  // Results in a series of calls to Pixel()
  // dotPattern drives dot pattern, use 0x00 for solid line (faster than 0xFF?)
  int dx, dy;
  int     sy;
  int er, e2;
  byte dotMask = 0x80;
  if (x0 > x1)
  {
    // ensure x0 <= x1;
    dx = x0; x0 = x1; x1 = dx;
    dy = y0; y0 = y1; y1 = dy;
  }

  dx = x1 - x0;
  dy = (y1 >= y0) ? y0 - y1 : y1 - y0;
  sy = (y0 <  y1) ? 1       : -1;
  er = dx + dy;

  while (1)
  {
    bool dot = true;
    if (minX <= x0 && x0 < maxX)
    {
      if (dotPattern)
      {
        if (dotMask & dotPattern)
          Pixel(x0 - minX, y0);
        else
          dot = false;
        dotMask >>= 1;
        if (!dotMask)
          dotMask = 0x80;
      }
      else
        Pixel(x0 - minX, y0);
    }
    else if (x0 > maxX)   // the rest is clipped, get out
      return;
    if ((x0 == x1) && (y0 == y1)) // done!
    {
      if (!dot) // add final dot if missing
        Pixel(x0 - minX, y0);
      return;
    }
    e2 = 2 * er;
    if (e2 >= dy)
    {
      er += dy;
      x0++;
    }
    if (e2 <= dx)
    {
      er += dx;
      y0 += sy;
    }
  }
}

void Paint(int originX, int originY, int minRow, int maxRow, byte*& pRowStart, word fg)
{
  // Paint the sparse pixels. Left edge is inset into window by originX.
  // Rows painted are minRow..maxRow
  // pRowStart optionally points to the start in the pool, updated to the end

  LCD_BEGIN_FILL(PAINT_WINDOW_ORIGIN_X + originX, originY + PAINT_WINDOW_ORIGIN_Y + minRow, SPARSE_COLS, maxRow - minRow);
  if (!pRowStart)
    pRowStart = pool;
  byte* pRow = rows + minRow;
  for (int row = minRow; row < maxRow; row++, pRow++)
  {
    byte* pValue = pRowStart;
    byte rowLen = *pRow;
    pRowStart += rowLen;
    byte prevX = 0;
    if (rowLen)
    {
      while (rowLen)
      {
        byte x = SPARSE_GET_X(pValue);
        byte len = SPARSE_GET_LEN(rowLen == 1, pValue);
        byte size = SPARSE_GET_SIZE(rowLen == 1, pValue);
        if (prevX < x)
        {
          LCD_FILL_BYTE(x - prevX, 0x00);
          LCD_FILL_COLOUR(len, fg);
          prevX = x + len;
        }
        else if (prevX < x + len)
        {
          // deal with overlaps
          byte extra = x + len - prevX;
          LCD_FILL_COLOUR(extra, fg);
          prevX += extra;
        }
        rowLen -= size;
        pValue += size;
      }
      if (prevX < SPARSE_COLS)
      {
        LCD_FILL_BYTE(SPARSE_COLS - prevX, 0x00);
      }
    }
    else
    {
      // blank row
      LCD_FILL_BYTE(SPARSE_COLS, 0x00);
    }
  }
}

// solid (dark) colour
//#define BACKGROUND_FILL(_count, _colour) LCD_FILL_COLOUR(_count, _colour)
// alternating dots of colour
#define BACKGROUND_FILL(_count, _colour) { long ctr = _count; while (ctr--) if (ctr % 2) LCD_ONE_BLACK(); else LCD_FILL_COLOUR(1, _colour); }


void PaintHorizon(int originX, int originY, int minRow, int maxRow, byte*& pRowStart, word fg, word bg)
{
  // AS ABOVE but background is special case, colour or dotted.
  // The assumption is it's faster to clone this, rather than make the other version have conditional tests. Code is cheap
  // Note: it seems to make very little difference to the speed.
  // Paint the sparse pixels. Left edge is inset into window by originX.
  // Rows painted are minRow..maxRow
  // pRowStart optionally points to the start in the pool, updated to the end

  LCD_BEGIN_FILL(PAINT_WINDOW_ORIGIN_X + originX, originY + PAINT_WINDOW_ORIGIN_Y + minRow, SPARSE_COLS, maxRow - minRow);
  if (!pRowStart)
    pRowStart = pool;
  byte* pRow = rows + minRow;
  for (int row = minRow; row < maxRow; row++, pRow++)
  {
    byte* pValue = pRowStart;
    byte rowLen = *pRow;
    pRowStart += rowLen;
    byte prevX = 0;
    if (rowLen)
    {
      while (rowLen)
      {
        byte x = SPARSE_GET_X(pValue);
        byte len = SPARSE_GET_LEN(rowLen == 1, pValue);
        byte size = SPARSE_GET_SIZE(rowLen == 1, pValue);
        if (prevX < x)
        {
          BACKGROUND_FILL(x - prevX, bg);
          LCD_FILL_COLOUR(len, fg);
          prevX = x + len;
        }
        else if (prevX < x + len)
        {
          // deal with overlaps
          byte extra = x + len - prevX;
          LCD_FILL_COLOUR(extra, fg);
          prevX += extra;
        }
        rowLen -= size;
        pValue += size;
      }
      if (prevX < SPARSE_COLS)
      {
        BACKGROUND_FILL(SPARSE_COLS - prevX, bg);
      }
    }
    else
    {
      // blank row
      BACKGROUND_FILL(SPARSE_COLS, bg);
    }
  }
}

}
