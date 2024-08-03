//This file is special that it allows alarm0 to be configured to go off every minute to indicate that a new minute has past.
//It has no other function.  This makes it so that the microcontroller need not poll the RTC.
#include "I2C_Drivers_PIC16F15225.h"
#include "RTC_Addresses.h"
//starting values
//nonbinary time
unsigned char alarm0minutes[2]={0,1};
//this exists to make the switch from daylight savings easier.
__bit DaylightSavings=0;
#define startDaylightDate 0
#define startDaylightMonth 0
#define endDaylightDate 0
#define endDaylightMonth 0
//these are the default control bytes for both alarms
//Alarm0 is set to go off on the matching of minutes.
#define Alarm0Control 0x90
//Alarm1 is set to go off on the matching of the date.
//plan to use for datelight savings.
#define Alarm1Control 0xF0
//need this so that the methods below can call it.
void StartTransmission(unsigned char);
void ClearAlarm0Interrupt(void);
void updatealarm1(void);
//default options for control register
#define defaultcontrol 0x18
void updatealarmminutes(void){
    alarm0minutes[0]++;
    if (alarm0minutes[0]<10){
        return;
    }
    alarm0minutes[1]++;
    alarm0minutes[0]=0;
    if (alarm0minutes[1]==6){
        alarm0minutes[1]=0;
    }
}
void Init_Alarm0(_Bool Output){
    StartTransmission(ALM0WKDay);
    I2C_Write(Alarm0Control);
    I2C_Restart();
    I2C_Write(RTC_Add);
    I2C_Write(CONTROL);
    I2C_Write(defaultcontrol);
    I2C_Stop();

}
void updatealarm0(void){
    //move memory pointer to minutes register
    StartTransmission(RTCMin);
    I2C_Restart();
    //read the minutes register.
    I2C_Write(RTC_Add|0x01);
    unsigned char temp=I2C_Read();
    NACK();
    //convert the binary time into something more useful.
    alarm0minutes[0]=temp&0xF0;
    alarm0minutes[1]=temp&0x0F;
    I2C_Restart();
    I2C_Write(RTC_Add);
    I2C_Write(ALM0Min);
    unsigned char BINAlarmTime=(alarm0minutes[0]<<4)|alarm0minutes[1];
    I2C_Write(BINAlarmTime);
    I2C_Stop();
    ClearAlarm0Interrupt();
}
void ClearAlarm0Interrupt(void){
    StartTransmission(ALM0WKDay);
    I2C_Write(Alarm0Control);
    I2C_Stop();
}
void InitAlarm1(void){
    updatealarm1();
}
void ClearAlarm1Interrupt(void){
    StartTransmission(ALM1WKDay);
    I2C_Write(Alarm1Control);
    I2C_Stop();
}
//update date upon reaching 0x00
void updatealarm1(void){
    StartTransmission(ALM1Sec);
    I2C_Write(0x00);
    I2C_Write(0x00);
    //2am
    I2C_Write(0x02);
    I2C_Write(Alarm1Control);
    //end of daylight savings
    if (DaylightSavings){
    I2C_Write(0x00);
    }
    //start of daylight savings.
    else {
        I2C_Write(0);
    }
}