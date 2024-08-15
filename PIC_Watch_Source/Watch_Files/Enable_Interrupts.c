#include <xc.h>
unsigned char timer1cnt = 0;
#define ButtonINTPIN 0x3C
#define AlarmINTPIN 0x04
#define AlarmINTPIN_TRI TRISA2
void IOC(void){
    //button interrupt pins.
    IOCCP = ButtonINTPIN;
    //RA2 alarm interrupt pin.
    IOCAN2=1;
    IOCAF2=0;
    AlarmINTPIN_TRI=1;
    IOCIE = 1;
}
//the exact time of timer1 is not time critical.  It will simply be used as a timeout for I2C.
void Timer1(void){
    TMR1IE=1;
    //1:4 prescaler. Asychronous.
    T1CON=0x26;
    T1GCONbits.GE=0;
    //FOSC/4 clock source.
    T1CLKbits.CS=0x01;
    TMR1=0;
}
void IncrementTimer1cnt(void){
    timer1cnt++;
}
void ClearTimer1cnt(void){
    timer1cnt=0;
}
unsigned char GetTimer1cnt(void){
    return timer1cnt;
}
void EnableInterrupts(void){
    IOC();
    Timer1();
    PEIE = 1;
    GIE = 1;
}
