#ifndef I2C_Drivers_h
#define I2C_Drivers_h
_Bool I2C_Wait(void);
void I2C_Start(void);
void I2C_Restart(void);
void I2C_Stop(void);
unsigned char I2C_Write(unsigned char data);
#endif
#ifndef I2C_Read_h
#define I2C_Read_h
unsigned char I2C_Read(void);
void ACK(void);
void NACK(void);
#endif