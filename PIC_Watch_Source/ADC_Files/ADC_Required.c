//We are using ADCON0
#include <xc.h>
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000
#endif
void Init_ADC(void){
    //voltage reference is 2.048
    FVRCON=0x02;
    //do not turn on the module yet just set the external channel to be RC2
    ADCON0=0;
    //using FOSC/4 with right justifcation and the reference voltage is FVR internal.
    ADCON1=0b11100011;
    //do not automatically convert.
    ADACT=0x00;
    //turn on ADC0.
    
    ADCON0bits.ON=1;
}
//convert an analog signal connected to RC2.  Only do this upon being called.  We don't need to poll, since in this specific project we need only to update
//the temperature sensor reading every so often (around a minute).
unsigned int Convert_Analog(void){
    //turn on FVR
    FVRCONbits.FVREN=1;
    //wait for FVR to be ready.
    while (!FVRCONbits.FVRRDY);
    //wait a little longer for stability.
    __delay_ms(50);
    ADCON0bits.GO=1;
    //wait for conversion
    while (ADCON0bits.GO);
    __delay_ms(1);
    //conversion is complete
    //turn off FVR to conserve power.  Even if its just for a microsecond.
    FVRCONbits.FVREN=0;
    unsigned int ADC=ADRESH;
    ADC=ADC<<8;
    ADC=ADC|ADRESL;
    //turn off interval FVR
    
    return ADC;
    
}