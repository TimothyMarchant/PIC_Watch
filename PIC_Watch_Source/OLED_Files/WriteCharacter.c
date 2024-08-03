//There will be seperate modes for just text and graphics and text
#include "I2C_Drivers_PIC16F15225.h"
#include "OLED_Commands.h"
#include "ASCII_Table.h"
#include "TextBuffer.h"
void Send_Command(unsigned char);
void SetPosition(unsigned char,unsigned char);
void DisplayTurnOn(void);
void DisplayTurnOff(void);

//change the current position of the cursor.
//Line must between 0 and 7 and column is between 0 and 15.
void changecursorposition(unsigned char line,unsigned char column){
    //if invalid choice just do nothing.
    if (line>7||column>15){
        return;
    }
    //recall that the left hex digit is the line number and the right hex digit is the column number.
    CursorPos=(line<<4)|column;
}
//Magic numbers (need to shift the character value to the current position in the ASCII_Table)
#define tableshiftfactor 33
#define DotPatternLength 5
//Write the characters to the display.
void WriteDisplay(signed char character){
    //return blank text for invalid characters (or null/space).
    if (character<33){
        for (unsigned char i=0;i<8;i++){
            I2C_Write(0x00);
        }
        return;
    }
    CharLines Chara=ASCII_Table[character-tableshiftfactor];
    unsigned char charpattern[DotPatternLength]={Chara.Line0,Chara.Line1,
    Chara.Line2,Chara.Line3,Chara.Line4};
    //at this point draw each line
    for (unsigned char i=0;i<DotPatternLength;i++){
        I2C_Write(charpattern[i]);
    }
    //write blanks
    I2C_Write(0x00);
    I2C_Write(0x00);
    I2C_Write(0x00);
}
void sendcharacter(signed char character){
    if (CursorPos==128){
        CursorPos=0;
    }
    if (character=='\n'){
        CursorPos=((CursorPos+16)/16)<<4;
        if (CursorPos>=0x80){
            CursorPos=0;
        }
        return;
    }
    Text[CursorPos]=character;
    CursorPos++;
}
void Display(void){
    //turn off display while writing.  We don't want the screen to randomly have some text appear and not be finished.
    DisplayTurnOff();
    //display all current text
    I2C_Start();
    I2C_Write(OLED_Address);
    I2C_Write(OLED_Data);
    for (unsigned char i=0;i<128;i++){
        WriteDisplay(Text[i]);
    }
    I2C_Stop();
    //turn the screen back on.
    DisplayTurnOn();
}
//clear the Text array and reset the cursor
void ClearTextBuffer(void){
    CursorPos=0;
    for (unsigned char i=0;i<128;i++){
        Text[i]=0;
    }
}