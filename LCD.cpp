#include <Arduino.h>
#include "LCD.h"

// The XC4630d hairball, see below
//touch calibration data=> raw values correspond to orientation 1
#define XC4630_TOUCHX0 920
#define XC4630_TOUCHY0 950
#define XC4630_TOUCHX1 120 //170
#define XC4630_TOUCHY1 160 //200

extern int XC4630_width,XC4630_height;
extern byte XC4630_orientation;

extern void XC4630_command(unsigned char d);
extern void XC4630_data(unsigned char d);
extern void XC4630_areaset(int x1,int y1,int x2,int y2);
extern void XC4630_init();
extern void XC4630_rotate(int n);

extern int XC4630_touchrawx();
extern int XC4630_touchrawy();
extern int XC4630_touchx();
extern int XC4630_touchy();
extern int XC4630_istouch(int x1,int y1,int x2,int y2);

LCD lcd;

#ifdef SERIALIZE
#define SERIALISE_COMMENT(_c) if (_serialise) { Serial.print("; ");Serial.println(_c);}
#define SERIALISE_INIT(_w,_h,_s) if (_serialise) { Serial.print(_w);Serial.print(',');Serial.print(_h);Serial.print(',');Serial.println(_s);}
#define SERIALISE_BEGINFILL(_x,_y,_w,_h) if (_serialise) { Serial.print(_x);Serial.print(',');Serial.print(_y);Serial.print(',');Serial.print(_w);Serial.print(',');Serial.println(_h);}
#define SERIALISE_FILLCOLOUR(_len,_colour) if (_serialise) { Serial.print(_len);Serial.print(',');Serial.println(_colour);}
#define SERIALISE_FILLBYTE(_len,_colour) if (_serialise) { Serial.print(_len);Serial.print(',');Serial.println(_colour?0xFFFF:0x0000);}
#else
#define SERIALISE_COMMENT(_c)
#define SERIALISE_INIT(_w,_h,_s)
#define SERIALISE_BEGINFILL(_x,_y,_w,_h)
#define SERIALISE_FILLCOLOUR(_len,_colour)
#define SERIALISE_FILLBYTE(_len,_colour)
#endif

#define LCD_RD   B00000001
#define LCD_WR   B00000010
#define LCD_RS   B00000100
#define LCD_CS   B00001000
#define LCD_RST  B00010000

// optimised code, specific to Uno
#define ToggleDataWR PORTC = B00010101; PORTC = B00010111; // keeps RST, RS & RD HIGH 
#define FastData(d) FastData2((d) & B11111100, (d) & B00000011);
// Preserves 10,11,12 & 13 on B
#define FastData2(h, l) PORTD = (h); PORTB = (PORTB & B11111100) | (l); ToggleDataWR;


// Preserves 10,11,12 & 13 on B
#define FastCmdByte(c) PORTC = LCD_RST | LCD_RD; PORTD = c & B11111100; PORTB = (PORTB & B11111100) | (c & B00000011);PORTC |= LCD_WR; PORTC |= LCD_RS;
#define FastCmd(c) FastCmdByte(0); FastCmdByte(c);

void LCD::init()
{
  // reset
  delay(250);
  pinMode(A4, OUTPUT);
  digitalWrite(A4, LOW);
  delay(100);
  digitalWrite(A4, HIGH);
  delay(500);


  XC4630_init();
  ChipSelect(true);

#ifdef ROTATION_USB_LEFT
  XC4630_rotate(2);
#else
  XC4630_rotate(4);
#endif

  SERIALISE_COMMENT("*** START");
  SERIALISE_INIT(XC4630_width, XC4630_height, 1);
}

void LCD::ChipSelect(bool select)
{
  if (select)
    PORTC &= ~LCD_CS;
  else
    PORTC |= LCD_CS;
}


unsigned long LCD::beginFill(int x, int y, int w, int h)
{
  SERIALISE_BEGINFILL(x, y, w, h);
#ifdef ROTATION_USB_LEFT
  // rotation = 2 case
  int x2 = x + w - 1;
  int y2 = y + h - 1;

  int tmp = x;
  int tmp2 = x2;
  x = y;
  x2 = y2;
  y = _width - tmp2 - 1;
  y2 = _width - tmp - 1;

  FastCmd(0x50);               //set x bounds
  FastData(x >> 8);
  FastData(x);
  FastCmd(0x51);               //set x bounds
  FastData(x2 >> 8);
  FastData(x2);
  FastCmd(0x52);               //set y bounds
  FastData(y >> 8);
  FastData(y);
  FastCmd(0x53);               //set y bounds
  FastData(y2 >> 8);
  FastData(y2);

  FastCmd(0x20);               //set x pos
  FastData(x >> 8);
  FastData(x);
  FastCmd(0x21);               //set y pos
  FastData(y2 >> 8);
  FastData(y2);

  FastCmd(0x22);  // Write Data to GRAM
#else
  // rotation = 4 case
  int x2 = x + w - 1;
  int y2 = y + h - 1;

  int tmp = x;
  int tmp2 = x2;
  x = XC4630_height - y2 - 1;
  x2 = XC4630_height - y - 1;
  y = tmp;
  y2 = tmp2;

  FastCmd(0x50);               //set x bounds
  FastData(x >> 8);
  FastData(x);
  FastCmd(0x51);               //set x bounds
  FastData(x2 >> 8);
  FastData(x2);
  FastCmd(0x52);               //set y bounds
  FastData(y >> 8);
  FastData(y);
  FastCmd(0x53);               //set y bounds
  FastData(y2 >> 8);
  FastData(y2);

  FastCmd(0x20);               //set x pos
  FastData(x2 >> 8);
  FastData(x2);
  FastCmd(0x21);               //set y pos
  FastData(y >> 8);
  FastData(y);

  FastCmd(0x22);  // Write Data to GRAM
#endif
  unsigned long count = w;
  count *= h;
  return count;
}

void LCD::fillColour(unsigned long count, word colour)
{
  SERIALISE_FILLCOLOUR(count, colour);

  // fill with full 16-bit colour
  byte h1 = (colour >> 8) & B11111100;
  byte l1 = (colour >> 8) & B00000011;
  byte h2 =  colour       & B11111100;
  byte l2 =  colour       & B00000011;
  while (count--)
  {
    FastData2(h1, l1);
    FastData2(h2, l2);
  }
}

void LCD::fillByte(unsigned long count, byte colour)
{
  SERIALISE_FILLBYTE(count, colour);

  // fill with just one byte, i.e. 0/black or 255/white, or other, for pastels
  PORTD = colour & B11111100;
  PORTB = (PORTB & B11111100) | (colour & B00000011);
  while (count--)
  {
    ToggleDataWR;
    ToggleDataWR;
  }
}

void LCD::OneWhite()
{
  SERIALISE_FILLBYTE(1, 0xFF);

  PORTD = B11111100;
  PORTB = (PORTB & B11111100) | B00000011;
  ToggleDataWR;
  ToggleDataWR;
}

void LCD::OneBlack()
{
  SERIALISE_FILLBYTE(1, 0x00);

  // fill with just one byte, i.e. 0/black or 255/white, or other, for pastels
  PORTD = 0x00;
  PORTB = PORTB & B11111100;
  ToggleDataWR;
  ToggleDataWR;
}

bool LCD::isTouch(int x, int y, int w, int h)
{
  return XC4630_istouch(x, y, x + w, y + h);
}

bool LCD::getTouch(int& x, int& y)
{
  x = XC4630_touchx();
  y = XC4630_touchy();
  return x >= 0 && y >= 0;
}

//=================================================================
// **** This code is not mine, it's from Jaycar, warnings and all. But it does initialise the LCD ****
// See  https://www.jaycar.co.nz/rfid-keypad
// Simplified and cleaned up. Converted initialisation code, which is the critical bit, from code to data, saved ~1k


//v3 was supplier change IC is UC8230 (SPFD5408 compatible?) Working with all tests

//basic 'library' for Jaycar XC4630 240x320 touchscreen display (v1/2/3 boards)
//include this in your sketch folder, and reference with #include "XC4630d.c"
//microSD card slot can be accessed via Arduino's SD card library (CS=10)
//supports: Mega, Leonaro, Uno with hardware optimizations (Uno fastest)
//supports: all other R3 layout boards via digitalWrite (ie slow)
//colours are 16 bit (binary RRRRRGGGGGGBBBBB)
//functions:
// XC4630_init() - sets up pins/registers on display, sets orientation=1
// XC4630_clear(c) - sets all pixels to colour c (can use names below)
// XC4630_rotate(n) -  sets orientation (top) 1,2,3 or 4
// typical setup code might be:
// XC4630_touchx() returns pixel calibrated touch data, <0 means no touch
// XC4630_touchy() returns pixel calibrated touch data, <0 means no touch
// XC4630_istouch(x1,y1,x2,y2) is area in (x1,y1) to (x2,y2) being touched?

// Low level functions:
// XC4630_touchrawx() returns raw x touch data (>~900 means no touch)
// XC4630_touchrawy() returns raw y touch data (>~900 means no touch)
// XC4630_areaset(x1,y1,x2,y2) -  set rectangular painting area, only used for raw drawing in other functions
// XC4630_data(d) and XC4630_command - low level functions

//global variables to keep track of orientation etc, can be accessed in sketch
int XC4630_width,XC4630_height;
byte XC4630_orientation;

// BUT NOTE the LCD code is optimised for Uno!
//defines for LCD pins, dependent on board
//For Mega Board
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define XC4630SETUP   "MEGA"
#define XC4630RSTOP   DDRF|= 16
#define XC4630RST0    PORTF&= 239
#define XC4630RST1    PORTF|= 16
#define XC4630CSOP    DDRF|=8
#define XC4630CS0     PORTF&=247
#define XC4630CS1     PORTF|=8
#define XC4630RSOP    DDRF|=4
#define XC4630RS0     PORTF&=251
#define XC4630RS1     PORTF|=4
#define XC4630WROP    DDRF|=2
#define XC4630WR0     PORTF&=253
#define XC4630WR1     PORTF|=2
#define XC4630RDOP    DDRF|=1
#define XC4630RD0     PORTF&=254
#define XC4630RD1     PORTF|=1
#define XC4630dataOP  DDRE=DDRE|56;DDRG=DDRG|32;DDRH=DDRH|120;
#define XC4630data(d) PORTE=(PORTE&199)|((d&12)<<2)|((d&32)>>2);PORTG=(PORTG&223)|((d&16)<<1);PORTH=(PORTH&135)|((d&192)>>3)|((d&3)<<5);
#endif

//For Leonardo Board
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
#define XC4630SETUP   "LEO"
#define XC4630RSTOP   DDRF |= 2
#define XC4630RST0    PORTF &= 253
#define XC4630RST1    PORTF |= 2
#define XC4630CSOP    DDRF |= 16
#define XC4630CS0     PORTF &= 239
#define XC4630CS1     PORTF |= 16
#define XC4630RSOP    DDRF |= 32
#define XC4630RS0     PORTF &= 223
#define XC4630RS1     PORTF |= 32
#define XC4630WROP    DDRF |= 64
#define XC4630WR0     PORTF &= 191
#define XC4630WR1     PORTF |= 64
#define XC4630RDOP    DDRF |= 128
#define XC4630RD0     PORTF &= 127
#define XC4630RD1     PORTF |= 128
#define XC4630dataOP  DDRB=DDRB|48;DDRC=DDRC|64;DDRD=DDRD|147;DDRE=DDRE|64;
#define XC4630data(d) PORTB=(PORTB&207)|((d&3)<<4);PORTC=(PORTC&191)|((d&32)<<1);PORTD=(PORTD&108)|((d&4)>>1)|((d&8)>>3)|((d&16))|((d&64)<<1);PORTE=(PORTE&191)|((d&128)>>1);
#endif

//for Uno board
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
#define XC4630SETUP   "UNO"
#define XC4630RSTOP   DDRC|=16
#define XC4630RST0    PORTC&=239
#define XC4630RST1    PORTC|=16
#define XC4630CSOP    DDRC|=8
#define XC4630CS0     PORTC&=247
#define XC4630CS1     PORTC|=8
#define XC4630RSOP    DDRC|=4
#define XC4630RS0     PORTC&=251
#define XC4630RS1     PORTC|=4
#define XC4630WROP    DDRC|=2
#define XC4630WR0     PORTC&=253
#define XC4630WR1     PORTC|=2
#define XC4630RDOP    DDRC|=1
#define XC4630RD0     PORTC&=254
#define XC4630RD1     PORTC|=1
#define XC4630dataOP  DDRD |= 252;DDRB |= 3;
#define XC4630data(d) PORTD=(PORTD&3)|(d&252); PORTB=(PORTB&252)|(d&3); 
#endif

//default that will work for any other board
#ifndef XC4630SETUP
#define XC4630SETUP   "DEFAULT"
#define XC4630RSTOP   pinMode(A4,OUTPUT)
#define XC4630RST0    digitalWrite(A4,LOW)
#define XC4630RST1    digitalWrite(A4,HIGH)
#define XC4630CSOP    pinMode(A3,OUTPUT)
#define XC4630CS0     digitalWrite(A3,LOW)
#define XC4630CS1     digitalWrite(A3,HIGH)
#define XC4630RSOP    pinMode(A2,OUTPUT)
#define XC4630RS0     digitalWrite(A2,LOW)
#define XC4630RS1     digitalWrite(A2,HIGH)
#define XC4630WROP    pinMode(A1,OUTPUT)
#define XC4630WR0     digitalWrite(A1,LOW)
#define XC4630WR1     digitalWrite(A1,HIGH)
#define XC4630RDOP    pinMode(A0,OUTPUT)
#define XC4630RD0     digitalWrite(A0,LOW)
#define XC4630RD1     digitalWrite(A0,HIGH)
#define XC4630dataOP  for(int i=2;i<10;i++){pinMode(i,OUTPUT);}
#define XC4630data(d) for(int i=2;i<10;i++){digitalWrite(i,d&(1<<(i%8)));}
#endif

void XC4630_command(unsigned char d)
{   
  XC4630RS0;        //RS=0 => command
  XC4630data(d);    //data
  XC4630WR0;        //toggle WR
  XC4630WR1;
  XC4630RS1;
}

void XC4630_data(unsigned char d)
{
  XC4630data(d);    //data
  XC4630WR0;        //toggle WR
  XC4630WR1;
}


void XC4630_areaset(int x1,int y1,int x2,int y2)
{
  if(x2<x1){int i=x1;x1=x2;x2=i;}   //sort x
  if(y2<y1){int i=y1;y1=y2;y2=i;}   //sort y
  int t1,t2,t3,t4,xs,ys;
  //for UC8230, need to internally remap x and y- pixel write direction?
  xs=x1;
  ys=y1;
  t1=x1;
  t2=x2;
  t3=y1;
  t4=y2;
  switch(XC4630_orientation){
    case 1:
      x1=XC4630_width-1-t2;
      x2=XC4630_width-1-t1;
      y1=XC4630_height-1-t4;
      y2=XC4630_height-1-t3;
      xs=x2;
      ys=y2;    
      break;
    case 2:
      x1=t3;
      x2=t4;
      y1=XC4630_width-1-t2;
      y2=XC4630_width-1-t1;
      xs=x1;
      ys=y2;    
    break;
    case 3:
      xs=x1;
      ys=y1;    
      break;
    case 4:
      x1=XC4630_height-1-t4;
      x2=XC4630_height-1-t3;
      y1=t1;
      y2=t2;
      xs=x2;
      ys=y1;    
      break;
  }
  XC4630CS0;

  XC4630_command(0);               //hi byte   
  XC4630_command(80);               //set x bounds  
  XC4630_data(x1>>8);
  XC4630_data(x1);
  XC4630_command(0);               //hi byte   
  XC4630_command(81);               //set x bounds  
  XC4630_data(x2>>8);
  XC4630_data(x2);
  XC4630_command(0);               //hi byte   
  XC4630_command(82);               //set y bounds
  XC4630_data(y1>>8);
  XC4630_data(y1);
  XC4630_command(0);               //hi byte   
  XC4630_command(83);               //set y bounds  
  XC4630_data(y2>>8);
  XC4630_data(y2);

  XC4630_command(0);               //hi byte   
  XC4630_command(32);               //set x pos
  XC4630_data(xs>>8);
  XC4630_data(xs);
  XC4630_command(0);               //hi byte   
  XC4630_command(33);               //set y pos
  XC4630_data(ys>>8);
  XC4630_data(ys);
  
  XC4630_command(0);               //hi byte   
  XC4630_command(34);               //drawing data to follow
  XC4630CS1;
}

//"from misc.ws LCD_ID_reader- gives a successful init"
const byte initialisation[] PROGMEM =
{
  229, 128, 0,
  0, 0, 1,
  1, 0, 0,
  2, 7, 0,
  3, 16, 0,//  "0,  3, 0, 48,"
  4, 0, 0,
  8, 2, 2,
  9, 0, 0,
  10, 0, 0,
  12, 0, 0,
  13, 0, 0,
  15, 0, 0,
  16, 0, 0,
  17, 0, 0,
  18, 0, 0,
  19, 0, 0,
  16, 23, 176,
  17, 0, 55,
  18, 1, 56,
  19, 23, 0,
  41, 0, 13,
  32, 0, 0,
  33, 0, 0,
  48, 0, 1,
  49, 6, 6,
  50, 3, 4,
  51, 2, 2,
  52, 2, 2,
  53, 1, 3,
  54, 1, 29,
  55, 4, 4,
  56, 4, 4,
  57, 4, 4,
  60, 7, 0,
  61, 10, 31,
  80, 0, 0,
  81, 0, 239,
  82, 0, 0,
  83, 1, 63,
  96, 39, 0,
  97, 0, 1,
  106, 0, 0,
  144, 0, 16,
  146, 0, 0,
  147, 0, 3,
  149, 1, 1,
  151, 0, 0,
  152, 0, 0,
  7, 0, 33,
  7, 0, 49,
  7, 1, 115,
  255, // end!
};

void XC4630_init(){
  //set variable defaults
  XC4630_width=240;
  XC4630_height=320;
  XC4630_orientation=1;
  XC4630dataOP;
  XC4630RSTOP;
  XC4630CSOP;
  XC4630RSOP;
  XC4630WROP;
  XC4630RDOP;
  XC4630RST1;
  XC4630CS1;
  XC4630RS1;
  XC4630WR1;
  XC4630RD1;
  delay(50); 
  XC4630RST0;
  delay(150);
  XC4630RST1;
  delay(150);
  XC4630CS0;

  const byte* pInit = initialisation;
  while (true)
  {
    byte cmd = pgm_read_byte_near(pInit++);
    if (cmd == 255)
      break;
    XC4630_command(0);
    XC4630_command(cmd);
    XC4630_data(pgm_read_byte_near(pInit++));
    XC4630_data(pgm_read_byte_near(pInit++));
  }
        
  XC4630CS1;
}

void XC4630_rotate(int n){
  XC4630CS0;
  switch(n){
    case 1:
    XC4630_command(0);  XC4630_command(3);XC4630_data(16);XC4630_data(0);    //0>1
    XC4630_width=240;
    XC4630_height=320;
    XC4630_orientation=1;
    break;
    case 2:
    XC4630_command(0);  XC4630_command(3);XC4630_data(16);XC4630_data(24);    //0>1
    XC4630_width=320;
    XC4630_height=240;
    XC4630_orientation=2;
    break;
    case 3:
    XC4630_command(0);  XC4630_command(3);XC4630_data(16);XC4630_data(48);    //0>1
    XC4630_width=240;
    XC4630_height=320;
    XC4630_orientation=3;
    break;
    case 4:
    XC4630_command(0);  XC4630_command(3);XC4630_data(16);XC4630_data(40);    //0>1
    XC4630_width=320;
    XC4630_height=240;
    XC4630_orientation=4;
    break;
  }
  XC4630CS1;
}


/*
 * https://www.instructables.com/id/4-Wire-Touch-Screen-Interfacing-with-Arduino/
          A3 INPUT
          |
          |
Gnd D8----+----A2 +5V
          |
          |
          D9 TriState
*/          
int XC4630_touchrawx(){           //raw analog value
  int x;
  // voltage across X-axis
  pinMode(8,OUTPUT);
  digitalWrite(8,LOW);
  pinMode(A2,OUTPUT);
  digitalWrite(A2,HIGH);

  // tri-state
  pinMode(9,INPUT_PULLUP);
  
  // read voltage divider on A3
  pinMode(A3,INPUT);
  analogRead(A3);                 // discard first result after pinMode change
  delayMicroseconds(30);
  x=analogRead(A3);
  
  // restore
  pinMode(A3,OUTPUT);
  digitalWrite(A3,HIGH);
  pinMode(9,OUTPUT);
//Serial.print("x=");Serial.println(x);  
  return(x);  
}

int XC4630_touchrawy(){           //raw analog value
  int y;
  pinMode(9,OUTPUT);
  pinMode(A3,OUTPUT);
  digitalWrite(9,LOW);            //doesn't matter if this changes
  digitalWrite(A3,HIGH);          //this is normally high between screen commands
  pinMode(A2,INPUT_PULLUP);       //this is normally high between screen commands
  pinMode(8,INPUT_PULLUP);        //doesn't matter if this changes
  analogRead(A2);                 //discard first result after pinMode change
  delayMicroseconds(30);
  y=analogRead(A2);
  pinMode(A2,OUTPUT);
  digitalWrite(A2,HIGH);          //restore output state from above
  pinMode(8,OUTPUT);
//Serial.print("y=");Serial.println(y);  
  return(y);  
}

int XC4630_touchx(){
  int x,xc;
  xc=-1;      //default in case of invalid orientation
  switch(XC4630_orientation){
    case 1:
    x=XC4630_touchrawx();
    xc=map(x,XC4630_TOUCHX0,XC4630_TOUCHX1,0,XC4630_width-1);
    break;
    case 2:
    x=XC4630_touchrawy();
    xc=map(x,XC4630_TOUCHY0,XC4630_TOUCHY1,0,XC4630_width-1);
    break;
    case 3:
    x=XC4630_touchrawx();
    xc=map(x,XC4630_TOUCHX1,XC4630_TOUCHX0,0,XC4630_width-1);
    break;
    case 4:
    x=XC4630_touchrawy();
    xc=map(x,XC4630_TOUCHY1,XC4630_TOUCHY0,0,XC4630_width-1);
    break;
  }
  if(xc>XC4630_width-1){xc=-1;}         //off screen
  return xc;
}

int XC4630_touchy(){
  int y,yc;
  yc=-1;      //default in case of invalid orientation
  switch(XC4630_orientation){
    case 1:
    y=XC4630_touchrawy();
    yc=map(y,XC4630_TOUCHY0,XC4630_TOUCHY1,0,XC4630_height-1);
    break;
    case 2:
    y=XC4630_touchrawx();
    yc=map(y,XC4630_TOUCHX1,XC4630_TOUCHX0,0,XC4630_height-1);
    break;
    case 3:
    y=XC4630_touchrawy();
    yc=map(y,XC4630_TOUCHY1,XC4630_TOUCHY0,0,XC4630_height-1);
    break;
    case 4:
    y=XC4630_touchrawx();
    yc=map(y,XC4630_TOUCHX0,XC4630_TOUCHX1,0,XC4630_height-1);
    break;
  }
  if(yc>XC4630_height-1){yc=-1;}         //off screen
  return yc;
}

int XC4630_istouch(int x1,int y1,int x2,int y2){    //touch occurring in box?
  int x,y;
  x=XC4630_touchx();
  if(x<x1){return 0;}
  if(x>x2){return 0;}
  y=XC4630_touchy();
  if(y<y1){return 0;}
  if(y>y2){return 0;}
  return 1;
}
