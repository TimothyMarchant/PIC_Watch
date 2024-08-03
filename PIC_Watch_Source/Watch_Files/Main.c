#include "CONFIG.h"
#include "I2C_Drivers_PIC16F15225.h"
#include "SSD1306_Drivers.h"
#include "MCP9700_Drivers.h"
#include "MCP7940_Drivers.h"
/* This project uses the MCP7940M RTC, SSD1306, and the MCP9700 analog temperature sensor
 * This microcontroller was picked for several reasons
 * 1st, I had these in my house already and seemed to be big enough to do the job.
 * 2nd it's small enough for a <1.5 inch PCB (need it to be smaller than this more than likely; the PCB that is).
 * 
 */
//
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000
#endif
#define Button1 PORTCbits.RC2
#define Button2 PORTCbits.RC3
#define Button3 PORTCbits.RC4
#define Button4 PORTCbits.RC5
#define PORTCButtons 0b00111100
void SendNumber(unsigned int);
void SendByte(unsigned char);
void HandleButton(unsigned char);
//void DisplaySetting(void);

unsigned int timer0cnt = 0;
unsigned int timer1cnt = 0;
unsigned char data = 0;
_Bool DeepSleep = 0;
_Bool IsPressed = 0;
void GotoSleep(void);
void DisplayTime(void);
void DisplayState(void);
void __interrupt() ISR(void) {
    //wakeup device.
    if (IOCCF) {
        //for debouncing
        __delay_ms(50);
        //get inputs from RC2-RC5 only.
        data = (PORTC & PORTCButtons)>>2;
        IOCCF = 0;
        IOCIF = 0;
    }
    if (IOCAFbits.IOCAF3) {

    }
    if (TMR0IF) {

    }
    //go to sleep upon waiting 10 seconds.
    if (TMR1IF) {
        timer1cnt++;
        if (timer1cnt == 1000) {
            timer1cnt = 0;
            GotoSleep();
            TMR1ON=0;
        }
        TMR1IF=0;
    }
}
void GotoSleep(void){
    DeepSleep=1;
    //do some other things.
}
void main(void) {
    TRISA = 0x01;
    ANSELAbits.ANSA0 = 1;
    LATA = 0x00;
    LATC = 0x00;
    //RC2-RC5 are all inputs.
    TRISC = 0xFF;
    ANSELC = 0x00;
    WPUC = 0x00;
    IOCCP = 0x3C;
    
    Temp_Init(&LATA, 5);
    I2C_Init();
    __delay_ms(50);
    init_OLED();
    MCP7940_Init();
    GIE = 1;
    PEIE = 1;
    IOCIE = 1;
    
    //DisplaySetting();
    //data=0x02;
    while (1) {
        //don't turn off the screen immediately wait some time then turn it off.
        HandleButton(data);
        if (DeepSleep) {
            DisplayTurnOff();
        }
        
        SLEEP();
        //turn the screen back on.
        if (DeepSleep) {
            DisplayTurnOn();
            DeepSleep = 0;
        }
        
    }
}
