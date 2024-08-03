#include <xc.h>
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000
#endif
#define error 2
#define SCL_TRI TRISA4
#define SDA_TRI TRISA5
void I2C_Init(void){
    SSP1CON3=0x00;
    SSP1CON1=0b00001000;
    SSP1STATbits.SMP=1;
    SSP1ADD=0x4F;
    //map pins for I2C
    
    //sda
    RC1PPS=0x08;
    SSP1DATPPS=0x11;
    //scl
    RC0PPS=0x07;
    SSP1CLKPPS=0x10;
    //this last part is to prevent issues found the errata sheet
    SSP1CON1bits.SSPEN=1;
    //need to delay at least 250 ns and 6 instruction cycles.  This does more than that
    __delay_us(250);
    PIR1bits.SSP1IF=0;
    SSP1CON3=0x00;
}
//call to stop communication.
void bitbangrecovery(void){
    
}
void ForceReset(void){
    //bitbang recovery
    
    //reset the device
    RESET();
}
_Bool I2C_Wait(void){
    while (((SSP1STAT&0x04)|(0b00011111&SSP1CON2)));
    return 1;
}
void I2C_Start(void){
    //turn off interupts while in I2C communication.
    GIE=0;
    I2C_Wait();
    SSP1CON2bits.SEN=1;
}
void I2C_Restart(void){
    I2C_Wait();
    SSP1CON2bits.RSEN=1;
}
void I2C_Stop(void){
    I2C_Wait();
    SSP1CON2bits.PEN=1;
    I2C_Wait();
    GIE=1;
}

unsigned char I2C_Write(unsigned char data){
    I2C_Wait();
    SSP1BUF=data;
    I2C_Wait();
    return SSP1CON2bits.ACKSTAT;
}
