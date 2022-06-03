#include "arduino.h"
#include "SoftwareI2C.h"
#include "Pins.h"
#include "RTC.h"

// real time clock
RTC rtc;

#ifdef RTC_I2C_ADDRESS
SoftwareI2C softWire;
#endif

RTC::RTC():
  m_Hour24(0x12),
  m_Minute(0)
{
}

#ifdef RTC_I2C_ADDRESS
void RTC::Setup()
{
  softWire.begin(PIN_RTC_SDA, PIN_RTC_SCL);
  ReadTime();
}

void RTC::ReadTime()
{
    // just the minutes and hours
    // from register 1
  softWire.beginTransmission(RTC_I2C_ADDRESS);
  softWire.write((byte)0x01);
  softWire.endTransmission();

  softWire.requestFrom(RTC_I2C_ADDRESS, 2);

  m_Minute = BCD2Dec(softWire.read());
  byte Register2 = softWire.read();
  if (Register2 & 0x40)  // 12/24 hr
  {
    // 12 hr mode, bit 6=PM
    m_Hour24 = BCD2Dec(Register2 & 0x3F);
    if (Register2 & 0x20)
    {
      m_Hour24 += 12;
      if (m_Hour24 > 23) m_Hour24 = 0;
    }
  }
  else
  {
    // 24 hour mode
    m_Hour24 = BCD2Dec(Register2 & 0x3F);
  }
}
byte RTC::ReadMinute()
{
  // from register 01
  softWire.beginTransmission(RTC_I2C_ADDRESS);
  softWire.write(0x01);
  softWire.endTransmission();

  softWire.requestFrom(RTC_I2C_ADDRESS, 1);

  return BCD2Dec(softWire.read());
}

void RTC::WriteTime()
{
  softWire.beginTransmission(RTC_I2C_ADDRESS);
  softWire.write((byte)0x00);
  softWire.write(Dec2BCD(0));
  softWire.write(Dec2BCD(m_Minute));
  softWire.write(Dec2BCD(m_Hour24));  // 24 hr mode
  softWire.endTransmission();
}

void RTC::Seed(uint32_t& seed)
{
  // 7 bytes starting at register 0
  softWire.beginTransmission(RTC_I2C_ADDRESS);
  softWire.write((byte)0x00);
  softWire.endTransmission();
  softWire.requestFrom(RTC_I2C_ADDRESS, 7);
  for (int i = 0; i < 7; i++)
  {
    seed <<= 4;
    seed |= softWire.read();
  }
}

#else
void RTC::Setup()
{
  m_Hour24 = 12
  m_Minute = 0;
}

void RTC::ReadTime( )
{
}

byte RTC::ReadMinute()
{
  return m_Minute;
}

void RTC::WriteTime()
{
}

void RTC::Seed(uint32_t& )
{
}

#endif
byte RTC::BCD2Dec(byte BCD)
{
  return (BCD / 16 * 10) + (BCD & 0x0F);
}

byte RTC::Dec2BCD(byte Dec)
{
  return (Dec / 10 * 16) + (Dec % 10);
}
