#include "I2C_Drivers_PIC16F15225.h"
#include "RTC_Addresses.h"
#define Leapyearmask 0x20
void StartTransmission(unsigned char);
//year read as Tens year and ones year
void SetYear(unsigned char year[2]){
    unsigned char YEAR=year[1]|(year[0]<<4);
    StartTransmission(RTCYear);
    I2C_Write(YEAR);
    I2C_Stop();
}
void ReadYear(unsigned char year[2]){
    StartTransmission(RTCYear);
    I2C_Restart();
    I2C_Write(RTC_Add|0x01);
    unsigned char temp=I2C_Read();
    NACK();
    I2C_Stop();
    //max value of 9.
    year[0]=(temp&0xF0)>>4;
    year[1]=temp&0x0F;
    
}
_Bool getLeapyear(void){
    StartTransmission(RTCMTH);
    I2C_Restart();
    I2C_Write(RTC_Add|0x01);
    unsigned char temp=I2C_Read();
    NACK();
    I2C_Stop();
    temp=(temp&Leapyearmask)>>5;
    return (_Bool) temp;
}