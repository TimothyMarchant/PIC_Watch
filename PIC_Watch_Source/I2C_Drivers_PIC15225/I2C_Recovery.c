#include <xc.h>
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000
#endif
#define SCL_TRI TRISC0
#define SDA_TRI TRISC1
#define SDA_LAT LATC1
#define SCL_LAT LATC0
//call to stop communication.
//the frequency of the microcontroller is 32mHz.
//We want to run I2C at approximately 100 kHz.
//need delays to be 40 microseconds between pulses
#define I2CDelay __delay_us(40)
//this method sends the byte 0x00 in an attempt to restore the bus from a hang.
//this was tested where I was able to get the SDA line to lock up.  
//Using this plus a reset fixed the hangup problem.
void bitbangrecovery(void){
    
    SSP1CON1bits.SSPEN=0;
    //make the SDA and SCL pins output to manually control the bus.
    SCL_TRI=0;
    SDA_TRI=0;
    SDA_LAT=0;
    //the I2C specification states to free the bus we need to send nine clock pulses on the SCL line.
    for (unsigned char i=0;i<9;i++){
        if (SCL_LAT){
            SCL_LAT=0;
        }
        else {
            SCL_LAT=1;
        }
        I2CDelay;
    }
    //stop condition to end early.
    SCL_LAT=0;
    I2CDelay;
    SCL_LAT=1;
    I2CDelay;
    SDA_LAT=1;
    I2CDelay;
}
void ForceReset(void){
    //bitbang recovery
    bitbangrecovery();
    //reset the device
    RESET();
}