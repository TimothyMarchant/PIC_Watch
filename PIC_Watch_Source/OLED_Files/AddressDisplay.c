//this file is for only modifying specific locations on the screen (to reduce the number of bytes to be sent over I2C).
//
#include "TextBuffer.h"
#include "I2C_Drivers_PIC16F15225.h"
#include "OLED_Commands.h"
#include "ASCII_Table.h"
void Send_Command(unsigned char);
void DisplayTurnOn(void);
void DisplayTurnOff(void);
void SetPosition(unsigned char,unsigned char);
void WriteDisplay(signed char);
void changecursorposition(unsigned char,unsigned char);
void sendcharacter(signed char);
//write only one line.  Line is the specified line to be written.  Line is from 0 to 7.
//this is useful when we only need to update one line and not the entire screen.
void WriteLine(unsigned char Line){
    SetPosition(0x00,Line);
    unsigned char position=Line<<4;
    I2C_Start();
    I2C_Write(OLED_Address);
    I2C_Write(OLED_Data);
    for (unsigned char i=position;i<(position+16);i++){
        WriteDisplay(Text[i]);
    }
    I2C_Stop();
    //reset position to be at the start to prevent any issues when we print the entire screen.
    SetPosition(0x00,0x00);
}
//send a character at a specified position.  
void Randomsendcharacter(signed char character,unsigned char bufferindex){
    changecursorposition((bufferindex&0x70)>>4,bufferindex&0x0F);
    sendcharacter(character);
}

//Meant for updating a single position.  Makes most sense in some sort of selection menu.
void DisplaySingleCharacter(unsigned char bufferindex){
    unsigned char column=(bufferindex&0x0F)*8;
    SetPosition(column,(bufferindex&0x70)>>4);
    I2C_Start();
    I2C_Write(OLED_Address);
    I2C_Write(OLED_Data);
    WriteDisplay(Text[bufferindex]);
    I2C_Stop();
    SetPosition(0x00,0x00);
}