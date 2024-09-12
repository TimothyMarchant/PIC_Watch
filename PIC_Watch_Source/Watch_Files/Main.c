#include "CONFIG.h"
#include "I2C_Drivers_PIC16F15225.h"
#include "SSD1306_Drivers.h"
#include "MCP9700_Drivers.h"
#include "MCP7940_Drivers.h"
/* This project uses the MCP7940N RTC, SSD1306, and the MCP9700 analog temperature sensor
 * This microcontroller was picked for several reasons
 * 1st, I had these in my house already and seemed to be big enough to do the job.
 * 2nd it's small enough for a <1.5 inch PCB (need it to be smaller than this more than likely; the PCB that is).
 * 
 */
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000
#endif
#define Button1 PORTCbits.RC2
#define Button2 PORTCbits.RC3
#define Button3 PORTCbits.RC4
#define Button4 PORTCbits.RC5
#define INTDATA 0xFF
#define PORTCButtons 0b00111100
#define OLED_POWER LATA4
void SendNumber(unsigned int);
void SendByte(unsigned char);
void HandleButton(unsigned char);
void EnableInterrupts(void);
void IncrementTimer1cnt(void);
unsigned char GetTimer1cnt(void);
unsigned int timer0cnt = 0;
unsigned char data = 0;
_Bool DeepSleep = 0;
_Bool IsPressed = 0;
_Bool NeedToUpdateTime=0;
void GotoSleep(void);
void DisplayTime(void);
void DisplayState(void);
void UpdateTimeArray(void);
void UpdateDateArray(void);
void UpdateYearArray(void);
void __interrupt() ISR(void) {
    //I2C recovery
    if (TMR1IF) {
        TMR1ON=0;
        IncrementTimer1cnt();
        TMR1IF=0;
        //timeout has clearly gone on too long restart the device after sending 9 clock pulses.
        if (GetTimer1cnt()==0xFF){
            ForceReset();
        }
        TMR1=0;
        TMR1ON=1;
    }
    //wakeup device.
    if (IOCCF>0) {
        //for debouncing
        __delay_ms(50);
        //get inputs from RC2-RC5 only.
        data = (PORTC & PORTCButtons)>>2;
        IOCCF = 0;
        IOCIF = 0;
        NeedToUpdateTime=0;
    }
    //alarm interrupt
    if (IOCAF>0){
        NeedToUpdateTime=1;
        //we need not update anything here, as the alarm pin will not change states until we modify the internal registers of the RTC.
        //The compiler gave a warning when we call such methods saying possible stack overflow.
        IOCIF=0;
        IOCAF=0;
    }
}
//this is for other files so that it can be changed elsewhere.
void GotoSleep(void){
    //
    DeepSleep=1;
    //do some other things.
}
void main(void) {
    TRISA = 0x00;
    LATA = 0x00;
    TRISA0=1;
    ANSELA=0x01;
    TRISA2=1;
    LATC = 0x00;
    //RC2-RC5 are all inputs.
    TRISC = 0xFF;
    ANSELC = 0x00;
    WPUC = 0x00;
    WPUA=0x00;
    OLED_POWER=1;
    Temp_Init(&LATA, 5);
    EnableInterrupts();
    I2C_Init();
    __delay_ms(50);
    init_OLED();
    MCP7940_Init();
    while (1) {
        //don't turn off the screen immediately wait some time then turn it off.
        HandleButton(data);
        if (DeepSleep) {
            DisplayTurnOff();
            //turn off alarm0 no interrupts are made in deep sleep.
            TurnOffInterrupt();
            //wait a little before just turning off the screen.
            __delay_ms(1);
            ClearAlarm0Interrupt();
            OLED_POWER=0;
            TMR1IE=0;
            data=0x00;
        }
        SLEEP();
        //We need this here instead of the ISR to avoid stack overflow.  The stack can only go 15 deep according to the datasheet (the compiler made a warning about it).
        
        //turn the screen back on.
        if (DeepSleep&&data!=0x00) {
            OLED_POWER=1;
            //wait some time before sending any commands.  the datasheet of the SSD1306 has the timing diagrams lasting a few microseconds, but I rather just wait longer
            //to avoid any problems.
            __delay_ms(25);
            init_OLED();
            //prevents the screen from having garbage shown.
            DisplayTurnOff();
            DeepSleep = 0;
        }
        if (NeedToUpdateTime){
            //update alarm time.
            updatealarm0();
            data=INTDATA;
            DeepSleep=0;
            NeedToUpdateTime=0;
        }
    }
}
