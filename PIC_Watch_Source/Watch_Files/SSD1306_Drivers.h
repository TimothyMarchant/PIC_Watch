//.h file for SSD1306 drivers
#ifndef Required_Files_h
#define Required_Files_h
void Send_Command(unsigned char command);
void Send_Data(unsigned char data);
void SetPosition(unsigned char x,unsigned char y);
void ClearDisplay(void);
void init_OLED(void);
void DisplayTurnOn(void);
void DisplayTurnOff(void);
#endif
#ifndef WriteCharacter_h
#define WriteCharacter_h
void ClearTextBuffer(void);
void changecursorposition(unsigned char line,unsigned char column);
void sendcharacter(signed char character);
void Display(void);
#endif
#ifndef AddressDisplay_h
#define AddressDisplay_h
void WriteLine(unsigned char Line);
void Randomsendcharacter(signed char character,unsigned char bufferindex);
void DisplaySingleCharacter(unsigned char bufferindex);
#endif
