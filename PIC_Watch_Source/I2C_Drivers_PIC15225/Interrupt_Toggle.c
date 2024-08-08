#include <xc.h>
//disable all interrupts excluding Timer1.  We do not want to interrupt mid transmission; except for a timeout on I2C.
void TurnOffInterrupts(void){
    IOCCP=0;
    
}
void TurnOnInterrupts(void){
    IOCCP = 0x3C;
    //RA1
    //IOCAN=0x02;
}
