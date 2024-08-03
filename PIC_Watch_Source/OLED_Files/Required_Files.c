#include "I2C_Drivers_PIC16F15225.h"
#include "OLED_Commands.h"
void ClearTextBuffer(void);
//these two are private
//send a single command
void Send_Command(unsigned char command){
    I2C_Start();
    I2C_Write(OLED_Address);
    I2C_Write(OLED_Inst);
    I2C_Write(command);
    I2C_Stop();
}
//send a single byte of data
void Send_Data(unsigned char data){
    I2C_Start();
    I2C_Write(OLED_Address);
    I2C_Write(OLED_Data);
    I2C_Write(data);
    I2C_Stop();
}
//set position in memory
void SetPosition(unsigned char Column,unsigned char page){
    I2C_Start();
    I2C_Write(OLED_Address);
    I2C_Write(OLED_Inst);
    I2C_Write(SetColumnAddress);
    //start from here, but make sure to reset upon going to the next line otherwise you can not modify anything before the selected column address.
    I2C_Write(Column);
    I2C_Write(ColumnEndAdd);
    I2C_Write(SetPageAddress);
    I2C_Write(page);
    I2C_Write(PageEndAdd);
    I2C_Stop();
}
void ClearDisplay(void){
    //start at top left.
    I2C_Start();
    I2C_Write(OLED_Address);
    I2C_Write(OLED_Inst);
    I2C_Write(0x2E);
    I2C_Write(0x20);
    I2C_Write(0x00);
    I2C_Write(0x21);
    I2C_Write(0x00);
    I2C_Write(127);
    I2C_Write(0x22);
    I2C_Write(0x00);
    I2C_Write(0x07);
    I2C_Write(0x40);
    I2C_Restart();
    //runs 1024 times.
    for (unsigned int i=0;i<1024;i++){
        I2C_Write(0x00);
    }
    I2C_Stop();
    //there is no need to reset the memory address since the address increments back to the original address.
    //clear the text buffer.
    ClearTextBuffer();
}
//sends DisplayOn command
void DisplayTurnOn(void){
    Send_Command(DisplayON);
}
//sends DisplayOff command
void DisplayTurnOff(void){
    Send_Command(DisplayOFF);
}
/*init_OLED
 *Intializes the SSD1306 oled screen.
 * This method returns void and takes in nothing.
 */
void init_OLED(void){
    I2C_Start();
    I2C_Write(OLED_Address);
    I2C_Write(OLED_Inst);
    for (unsigned char i=0;i<OLED_Init_Inst_Length;i++){
        I2C_Write(OLED_Init_Inst[i]);
    }
    I2C_Stop();
    //clear gabarage data on screen.
    ClearDisplay();
}