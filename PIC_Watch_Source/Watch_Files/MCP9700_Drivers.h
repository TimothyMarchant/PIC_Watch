//header file for MCP9700 temperature sensor
#ifndef _Temperature_h
#define _Temperature_h
void Temp_Init(volatile unsigned char* PinReg,unsigned char PinNum);
signed int GetTemp(void);
#endif