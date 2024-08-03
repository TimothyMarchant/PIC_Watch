#include <xc.h>
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000
#endif
#include "ADC_Drivers.h"
#define TempEquation(ADC) ADC/499.5 //This is the linear equation for determing the current temperature (this converts it to a voltage). Ref voltage=2.048
//Temp=(ADC-10)/5 approximately
unsigned char pinnumber;
volatile unsigned char * LATxREG;
void Temp_Init(volatile unsigned char* PinReg,unsigned char PinNum){
    //assign a pin to turn the sensor off and on.
    LATxREG=PinReg;
    pinnumber=PinNum;
    Init_ADC();
}
signed int GetTemp(void){
    *LATxREG=(0x01<<pinnumber)|(*LATxREG);
    //need to wait some time before reading temperature sensor after turning it on.
    __delay_ms(5);
    unsigned int ADCTemp=Convert_Analog();
    //shut down sensor to save power.
    *LATxREG=(~(0x01<<pinnumber))&(*LATxREG);
    //this formula was derived from the math below, except done in a different order.  499.5 was rounded up to 500 to simplify (it's a very small difference).
    //Doing it this way saves a ton of space (saves about 20% actually).
    signed int temperature=((ADCTemp-250)+3)/5;
    /*float temperature= TempEquation(ADCTemp);
    //from the datesheet 0 degrees is 500 milivolts, and for each 10 milivolts is one degree celsius.
    temperature-=0.500;
    //multiply by 100 to display the actual temperature up to 2 significant figures.
    temperature*=1000;
    //return temperature as an integer, since floats are much harder to work with.*/
    return (signed int) temperature;
}
