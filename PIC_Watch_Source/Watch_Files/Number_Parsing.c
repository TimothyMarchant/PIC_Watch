//this file is meant for parsing numbers and sending them to the display.
#include "DigitToASCII_Table.h"
#include "SSD1306_Drivers.h"
//max value of an int is 64 k something.
void SendNumber(unsigned int number){
    unsigned char numbers[5]={0};
    unsigned int multiplier=10000;
    for (unsigned char i=0;i<5;i++){
        numbers[i]=((number/multiplier)%10);
        multiplier/=10;
    }
    _Bool NonZeroprinted=0;
    for (unsigned char i=0;i<5;i++){
        //always print the last digit.
        if (numbers[i]!=0||NonZeroprinted||i==4){
            sendcharacter(DigitToASCII[numbers[i]]);
            NonZeroprinted=1;
        }
    }
}