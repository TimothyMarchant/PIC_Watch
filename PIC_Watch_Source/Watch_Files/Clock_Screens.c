//this file has everything for updating the watch display.  It handles very little logic.
#include "CONFIG.h"
#include "I2C_Drivers_PIC16F15225.h"
#include "DigitToASCII_Table.h"
#include "SSD1306_Drivers.h"
#include "MCP9700_Drivers.h"
#include "MCP7940_Drivers.h"
#include "ADC_Drivers.h"
void updateCursor(unsigned char);
void GetLeapYearStatus(void);
unsigned char Time[4]={0};
unsigned char Date[4]={0};
unsigned char Year[2]={0,0};

void UpdateTimeArray(void){
    ReadTime(Time);
}
void UpdateDateArray(void){
    ReadDate(Date);
}
void UpdateYearArray(void){
    ReadYear(Year);
}
void SendNumber(unsigned int);
void printtemp(void){
    changecursorposition(5,7);
    signed int temperature=GetTemp();
    if (temperature>=0){
        SendNumber((unsigned) temperature);
    }
    else {
        sendcharacter('-');
        temperature*=(-1);
        SendNumber((unsigned) temperature);
    }
    sendcharacter('C');
    LATA5=1;
    //for debugging purposes.  For some reason the temperature gets incorrect readings (for example <10 degrees celsius in a bedroom)
    sendcharacter('\n');
    SendNumber(Convert_Analog());
    sendcharacter('A');
    sendcharacter('D');
    sendcharacter('C');
    LATA5=0;
}
//these update methods exist solely to modify only one line of the display.
#define TemperatureLine 4
void UpdateTemperature(void){
    printtemp();
    WriteLine(TemperatureLine);
}
#define TimeLine 2
void UpdateDisplayTime(void){
    UpdateTimeArray();
    changecursorposition(TimeLine,5);
    sendcharacter(DigitToASCII[Time[0]]);
    sendcharacter(DigitToASCII[Time[1]]);
    sendcharacter(':');
    sendcharacter(DigitToASCII[Time[2]]);
    sendcharacter(DigitToASCII[Time[3]]);
    WriteLine(TimeLine);
}
#define DateLine 3
void UpdateDisplayDate(void){
    UpdateDateArray();
    changecursorposition(DateLine,5);
    sendcharacter(DigitToASCII[Date[0]]);
    sendcharacter(DigitToASCII[Date[1]]);
    sendcharacter('/');
    sendcharacter(DigitToASCII[Date[2]]);
    sendcharacter(DigitToASCII[Date[3]]);
    WriteLine(DateLine);
}
//state 1.  This is the main thing to display.
void DisplayTime(void){
    UpdateTimeArray();
    UpdateDateArray();
    UpdateYearArray();
    ClearTextBuffer();
    //print the time in the center of the screen. 
    changecursorposition(2,5);
    sendcharacter(DigitToASCII[Time[0]]);
    sendcharacter(DigitToASCII[Time[1]]);
    sendcharacter(':');
    sendcharacter(DigitToASCII[Time[2]]);
    sendcharacter(DigitToASCII[Time[3]]);
    //print date
    changecursorposition(3,5);
    sendcharacter(DigitToASCII[Date[0]]);
    sendcharacter(DigitToASCII[Date[1]]);
    sendcharacter('/');
    sendcharacter(DigitToASCII[Date[2]]);
    sendcharacter(DigitToASCII[Date[3]]);
    changecursorposition(4,5);
    sendcharacter('2');
    sendcharacter('0');
    sendcharacter(DigitToASCII[Year[0]]);
    sendcharacter(DigitToASCII[Year[1]]);
    //print temperature.
    //printtemp();
    Display();
}

//State 2.  Settings '*' indicates the current position.  Starts at line 0.
//final size is 75.
const signed char SettingsText[52]={
'S','e','t','t','i','n','g','\n',
'*','C','h','a','n','g','e',' ','T','i','m','e','\n',
' ','C','h','a','n','g','e',' ','D','a','t','e','\n',
' ','C','h','a','n','g','e',' ','Y','e','a','r','\n',
' ','E','x','i','t'};
/*
' ','O','t','h','e','r',' ','S','e','t','t','i','n','g','s','\n',
' ','T','i','m','e','r','\n',
' ','E','x','i','t'};*/
void DisplaySetting(void){
    ClearTextBuffer();
    changecursorposition(0,5);
    for (unsigned char i=0;i<52;i++){
        sendcharacter(SettingsText[i]);
    }
    Display();
}
#define Option1 0x10
#define Option2 0x20
#define Option3 0x30
#define Option4 0x40
#define Option5 0x50
#define Option6 0x60
//update '*' position
void updateSettingPos(_Bool down,unsigned char settingcursor){
    unsigned char temp;
    unsigned char temp2;
    switch (settingcursor){
        case 1:
            temp=Option2;
            temp2=Option1;
            break;
        case 2:
            temp2=Option2;
            if (down){
                temp=Option1;
            }
            else {
                temp=Option3;
            }
            break;
        case 3:
            temp2=Option3;
            if (down){
                temp=Option2;
            }
            else {
                temp=Option4;
            }
            break;
        case 4:
            temp2=Option4;
            if (down){
                temp=Option3;
            }
            else {
                temp=Option5;
            }
            break;
        case 5:
            temp2=Option5;
            if (down){
                temp=Option4;
            }
            else {
                temp=Option6;
            }
            break;
        case 6:
            temp2=Option6;
            if (down){
                temp=Option5;
            }
            break;
    }
    Randomsendcharacter(' ',temp);
    DisplaySingleCharacter(temp);
    Randomsendcharacter('*',temp2);
    DisplaySingleCharacter(temp2);
}
const signed char TimeDisplayText[30]={'T','i','m','e','\n',
'\n',
'\n',
' ',' ',' ',' ',' ','0','0',':','0','0','\n',
' ',' ',' ',' ',' ','C','o','n','f','i','r','m'};
//state 3. 
void ChangeTimeDisplay(void){
    ClearTextBuffer();
    changecursorposition(0,5);
    for (unsigned char i=0;i<30;i++){
        sendcharacter(TimeDisplayText[i]);
    }
    Display();
    updateCursor(0);
}
void updateCursor(unsigned char Cursor){
    switch (Cursor){
        case 0:
            Randomsendcharacter('*',0x25);
            Randomsendcharacter(' ',0x26);
            break;
        case 1:
            Randomsendcharacter(' ',0x25);
            Randomsendcharacter('*',0x26);
            Randomsendcharacter(' ',0x28);
            break;
        case 2:
            Randomsendcharacter(' ',0x26);
            Randomsendcharacter('*',0x28);
            Randomsendcharacter(' ',0x29);
            break;
        case 3:
            Randomsendcharacter(' ',0x28);
            Randomsendcharacter('*',0x29);
            Randomsendcharacter(' ',0x44);
            DisplaySingleCharacter(0x44);
            break;
        case 4:
            Randomsendcharacter(' ',0x29);
            Randomsendcharacter('*',0x44);
            DisplaySingleCharacter(0x44);
            break;
    }
    //rewrite top line
    WriteLine(0x02);
}
void UpdateDigit(unsigned char Cursor, signed char number){
    switch (Cursor){
        case 0:
            Randomsendcharacter(number,0x35);
            break;
        case 1:
            Randomsendcharacter(number,0x36);
            break;
        case 2:
            Randomsendcharacter(number,0x38);
            break;
        case 3:
            Randomsendcharacter(number,0x39);
            break;
    }
    //rewrite top line
    WriteLine(0x03);
}
const signed char DateDisplayText[30]={'D','a','t','e','\n',
'\n',
'\n',
' ',' ',' ',' ',' ','0','1','/','0','1','\n',
' ',' ',' ',' ',' ','C','o','n','f','i','r','m'};
//state 4 save dates
void DateMenuDisplay(void){
    ClearTextBuffer();
    changecursorposition(0,5);
    for (unsigned char i=0;i<30;i++){
        sendcharacter(DateDisplayText[i]);
    }
    Display();
    updateCursor(0);
}
//State 5 saves the year
const signed char YearDisplayText[30]={'Y','e','a','r','\n',
'\n',
'\n',
' ',' ',' ',' ',' ','2','0',':','0','0','\n',
' ',' ',' ',' ',' ','C','o','n','f','i','r','m'};
void YearMenuDisplay(void){
    ClearTextBuffer();
    changecursorposition(0,5);
    for (unsigned char i=0;i<30;i++){
        sendcharacter(YearDisplayText[i]);
    }
    Display();
    updateCursor(2);
}
//state 6.  Other settings
void OtherSettingsDisplay(void){
    
}
void TimerDisplay(void){
    
}