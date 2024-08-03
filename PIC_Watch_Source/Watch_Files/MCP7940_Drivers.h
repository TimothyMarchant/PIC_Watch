//driver methods for the MCP7940 RTC.
#ifndef Required_RTC_h
#define Required_RTC_h
void MCP7940_Init(void);
void SetTime(unsigned char Time[4]);
void SetDate(unsigned char Date[4]);
void ReadTime(unsigned char Time[4]);
void ReadDate(unsigned char Date[4]);
#endif
#ifndef Year_methods_h
#define Year_method_h
void SetYear(unsigned char year[2]);
void ReadYear(unsigned char year[2]);
_Bool getLeapyear(void);
#endif