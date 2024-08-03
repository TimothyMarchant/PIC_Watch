#include <xc.h>
_Bool I2C_Wait(void);
void ACK(void);
unsigned char I2C_Read(void);
void NACK(void);
unsigned char I2C_Read(){
    I2C_Wait();
    RCEN=1;
    I2C_Wait();
    unsigned char data=SSP1BUF;
    return data;
}
void ACK(void){
    I2C_Wait();
    ACKDT = 0;
    ACKEN = 1;
}
void NACK(void){
    I2C_Wait();
    ACKDT = 1;
    ACKEN = 1;
}