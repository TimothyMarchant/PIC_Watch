#include <xc.h>
unsigned char timer1cnt = 0;
void IOC(void){
    IOCAN=0x00;
    IOCCP = 0x3C;
    IOCIE = 1;
    
    //RA1
    //IOCAN=0x02;
}
void Timer0(void){
    
}
//the exact time of timer1 is not time critical.  It will simply be used as a timeout for I2C.
//timer increments only on multiples of FOSC (Hfintz based on the prescaler.  The clock rate is 32MHz
//Then the time for a single increment to the timer is FOSC/prescaler*4
//for timer1=0 to 0xFFFF then the delay is equal to (FOSC/prescaler*4)*0xFFFF.
//we need only to make the delay long enough to really make sure the bus is hanging.
void Timer1(void){
    TMR1IE=1;
    //1:4 prescaler. Asychronous.
    T1CON=0x26;
    T1GCONbits.GE=0;
    //FOSC/4. for now.
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
    //Timer0();
    Timer1();
    PEIE = 1;
    GIE = 1;
}
