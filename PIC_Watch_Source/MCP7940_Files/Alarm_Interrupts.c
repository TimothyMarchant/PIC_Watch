//This file is special that it allows alarm0 to be configured to go off every minute to indicate that a new minute has past.
//It has no other function.  This makes it so that the microcontroller need not poll the RTC.
#include "I2C_Drivers_PIC16F15225.h"
#include "RTC_Addresses.h"
//starting values
//nonbinary time
unsigned char alarm0minutes[2]={0,0};
//these are the default control bytes for both alarms
//Alarm0 is set to go off on the matching of minutes.
#define Alarm0Control 0x10
//need this so that the methods below can call it.
void StartTransmission(unsigned char);
void ClearAlarm0Interrupt(void);
void updatealarm1(void);
//default options for control register
#define defaultcontrol 0x10
void updatealarmminutes(void){
    alarm0minutes[1]++;
    if (alarm0minutes[1]<10){
        return;
    }
    alarm0minutes[0]++;
    alarm0minutes[1]=0;
    if (alarm0minutes[0]==6){
        alarm0minutes[0]=0;
    }
}
void Init_Alarm0(void){
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
    alarm0minutes[0]=(temp&0x70)>>4;
    alarm0minutes[1]=temp&0x0F;
    updatealarmminutes();
    unsigned char BINAlarmTime=(alarm0minutes[0]<<4)|alarm0minutes[1];
    I2C_Restart();
    I2C_Write(RTC_Add);
    I2C_Write(ALM0Min);
    I2C_Write(BINAlarmTime);
    //write to hours register to get to weekday register which contains the interrupt flag.
    //it's faster to write one byte here than restart the bus to get to the other register.
    I2C_Write(0x00);
    //update interrupt flag.
    I2C_Write(Alarm0Control);
    I2C_Stop();
}
void ClearAlarm0Interrupt(void){
    StartTransmission(ALM0WKDay);
    I2C_Write(Alarm0Control);
    I2C_Stop();
}
void TurnOffInterrupt(void){
    StartTransmission(CONTROL);
    I2C_Write(0);
    I2C_Stop();
}
void TurnOnInterrupt(void){
    StartTransmission(CONTROL);
    I2C_Write(defaultcontrol);
    I2C_Stop();
}
