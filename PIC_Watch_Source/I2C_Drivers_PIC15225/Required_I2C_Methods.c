#include <xc.h>
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000
#endif
#define error 2
#define SCL_TRI TRISC0
#define SDA_TRI TRISC1
#define SDA_LAT LATC1
#define SCL_LAT LATC0
#define SDA_PORT PORTCbits.RC1
#define SCL_PORT PORTCbits.RC0
void ClearTimer1cnt(void);
void TurnOffInterrupts(void);
void TurnOnInterrupts(void);
void ForceReset(void);
void I2C_Init(void){
    SSP1CON3=0x00;
    SSP1CON1=0b00001000;
    SSP1STATbits.SMP=1;
    SSP1ADD=0x4F;
    //map pins for I2C
    //sda rc1
    RC1PPS=0x08;
    SSP1DATPPS=0x11;
    //scl rc0
    RC0PPS=0x07;
    SSP1CLKPPS=0x10;
    //this last part is to prevent issues found the errata sheet
    SSP1CON1bits.SSPEN=1;
    //need to delay at least 250 ns and 6 instruction cycles.  This does more than that
    __delay_us(250);
    PIR1bits.SSP1IF=0;
    SSP1CON3=0x00;
}

_Bool I2C_Wait(void){
    //have a time out.
    TMR1ON=1;
    while (((SSP1STAT&0x04)|(0b00011111&SSP1CON2)));
    //turn off timer after done waiting.
    TMR1ON=0;
    //clear out timer count; we don't want it to have the value saved and cause a reset when there are no problems.
    ClearTimer1cnt();
    return 1;
}
void I2C_Start(void){
    //turn off interupts while in I2C communication.
    TurnOffInterrupts();
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
    //just wait a little longer to insure RC0 and RC1 are HIGH.
    __delay_us(500);
    //for some reason I've found that when the SDA line gets stuck, but SCL is stuck high, I2C_Wait() doesn't block and use the timeout implemented.
    //The timeout does work (or at least will run the required methods for a timeout), but while debugging I could only get the bus stuck by resetting in MPLAB IPE.
    //This so far is the only thing I've found that fixes a stuck bus.  I'm just going to assume the timeout works, since all my attempts to stop the bus don't seem to be working too well.
    if (SDA_PORT==0){
        ForceReset();
    }
    TurnOnInterrupts();
}

unsigned char I2C_Write(unsigned char data){
    I2C_Wait();
    SSP1BUF=data;
    I2C_Wait();
    return SSP1CON2bits.ACKSTAT;
}
