#include "RTC_Addresses.h"
#include "I2C_Drivers_PIC16F15225.h"
#define DefaultDate 31
#define DefaultMonth 7
//year is two digits.  Certainly it is not before 2000 and not after 2099 (If this code block even still exists at that point!)
#define DefaultYear 2024
#define TurnOn 0x80
void Init_Alarm(_Bool);
//this method exists so I don't have to write the same three method calls.
void StartTransmission(unsigned char address){
    I2C_Start();
    I2C_Write(RTC_Add);
    I2C_Write(address);
}
void TurnOffClock(void){
    StartTransmission(RTCSec);
    //if you're turning off the clock then the remaining seconds aren't exactly important.
    I2C_Write(0x00);
    I2C_Stop();
}
void TurnOnClock(void){
    StartTransmission(RTCSec);
    //turn on the clock.
    I2C_Write(TurnOn);
    I2C_Stop();
}
void MCP7940_Init(void){
    //Init_Alarm(1);
    StartTransmission(RTCSec);
    //start the timer
    I2C_Write(TurnOn);
    I2C_Stop();

    
}
void ChangeControl(unsigned char newcontrolbyte){
    StartTransmission(CONTROL);
    I2C_Write(newcontrolbyte);
    I2C_Stop();
}
//there are only 4 elements  Read the time as {Tens hour, Ones hour, Tens minute, Ones minute}
void SetTime(unsigned char Time[4]){
    unsigned char minutes=Time[3]|(Time[2]<<4);
    unsigned char hours=Time[1]|(Time[0]<<4);
    StartTransmission(RTCSec);
    //clear seconds register and turn off clock.
    I2C_Write(0x00);
    I2C_Write(minutes);
    I2C_Write(hours);
    I2C_Restart();
    I2C_Write(RTC_Add);
    I2C_Write(RTCSec);
    //start the timer
    I2C_Write(TurnOn);
    I2C_Stop();
}
//Date is read as {Date tens, date ones, Month tens, month ones};
void SetDate(unsigned char Date[4]){
    unsigned char month=Date[1]|(Date[0]<<4);
    unsigned char date=Date[3]|(Date[2]<<4);
    StartTransmission(RTCDate);
    I2C_Write(date);
    I2C_Write(month);
    I2C_Stop();
}

//Read Time minutes and hours
void ReadTime(unsigned char Time[4]){
    StartTransmission(RTCMin);
    I2C_Restart();
    I2C_Write(RTC_Add|0x01);
    unsigned char minutes=I2C_Read();
    ACK();
    unsigned char hours=I2C_Read();
    NACK();
    I2C_Stop();
    Time[0]=(hours&0xF0)>>4;
    Time[1]=hours&0x0F;
    Time[2]=(minutes&0xF0)>>4;
    Time[3]=minutes&0x0F;

}
//Read date and month
void ReadDate(unsigned char Date[4]){
    StartTransmission(RTCDate);
    I2C_Restart();
    I2C_Write(RTC_Add|0x01);
    unsigned char BINdate=I2C_Read();
    ACK();
    unsigned char Month=I2C_Read();
    NACK();
    I2C_Stop();
    Date[2]=(BINdate&0xF0)>>4;
    Date[3]=BINdate&0x0F;
    //month tens place is at most 1.
    Date[0]=(Month&0x10)>>4;
    Date[1]=Month&0x0F;
}
