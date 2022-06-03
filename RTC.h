#pragma once

// Talk to the DS3231/DS1307

// Undefine for no RTC attached
#define RTC_I2C_ADDRESS 0x68

class RTC
{
  public:
    RTC();
    void Setup();
    byte BCD2Dec(byte BCD);
    byte Dec2BCD(byte Dec);
    void ReadTime();
    byte ReadMinute();
    void WriteTime();
    void Seed(uint32_t& seed);

    byte m_Hour24;      // 0..23
    byte m_Minute;      // 0..59
};

extern RTC rtc;
