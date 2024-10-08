//this file handles the actual logic.  not the displaying part.
#include "CONFIG.h"
#include "MCP7940_Drivers.h"
#include "DigitToASCII_Table.h"
//watch_state is the current mode the watch is in.
/*
 * 0 implies the watch is in sleep
 * 1 implies on the clock screen
 * 2 implies the clock is in the settings screen.
 * 3 implies the clock is in the change time menu
 * 4 implies the clock is in the date menu (save dates)
 * 5 implies the clock is in the year menu
 */
#define Watch_Sleep 0
#define Clock 1
#define Settings 2
#define ChangeTime 3
#define DateMenu 4
#define YearMenu 5
//these two are currently unused.
#define OtherSettings 6
#define Timer 7
//
#define RTC_Alarm_INT 0xFF
#define Button_One 0x01
#define Button_Two 0x02
#define Button_Three 0x04
#define Button_Four 0x08
#define Exit Button_One
#define Confirm Button_Four
//initalize functions
void DisplayTime(void);
void DisplaySetting(void);
void UpdateDisplayDate(void);
void UpdateDisplayTime(void);
void UpdateDisplayYear(void);
void UpdateTemperature(void);
void updateSettingPos(_Bool, unsigned char);
void ChangeTimeDisplay(void);
void DateMenuDisplay(void);
//void OtherSettingsDisplay(void);
void SettingLogic(unsigned char);
void SettingsConfirm(void);
void TimeLogic(unsigned char);
void DateLogic(unsigned char);
//void OtherLogic(unsigned char);
void WatchLogic(unsigned char);
void updateCursor(unsigned char);
void UpdateDigit(unsigned char, signed char);
void UpdateTimeArray(void);
void GotoSleep(void);
void isLeapYear(void);
void YearMenuDisplay(void);
void YearLogic(unsigned char);
_Bool IsFeb29(void);
//variables
unsigned char settingcursor = 1;
unsigned char Watch_State = 0;
_Bool LeapYear = 0;

unsigned char Digits[4] = {0, 0, 0, 0};
unsigned char TempYear[2] = {0, 0};
//keep track of the previous state
unsigned char Previous_State = 0;
//displays the current state to the screen.
void DisplayState(void) {
    switch (Watch_State) {
        case Watch_Sleep:
            DisplayTime();
            break;
        case Clock:
            DisplayTime();
            break;
        case Settings:
            DisplaySetting();
            break;
        case ChangeTime:
            ChangeTimeDisplay();
            break;
        case DateMenu:
            DateMenuDisplay();
            break;
        case YearMenu:
            YearMenuDisplay();
            break;
        case OtherSettings:
            //OtherSettingsDisplay();
            break;
        case Timer:
            break;
        default:
            DisplayTime();
            break;
    }
}
//button logic
//Need to get this when changing dates.

void isLeapYear(void) {
    LeapYear = getLeapyear();
}
//Handle button inputs.
void HandleButton(unsigned char data) {
    if (data==RTC_Alarm_INT){
        UpdateDisplayTime();
        UpdateDisplayDate();
        UpdateDisplayYear();
        UpdateTemperature();
        return;
    }
    unsigned char previous_state = Watch_State;
    switch (Watch_State) {
        case Watch_Sleep:
            Watch_State = 1;
            break;
        case Clock:
            WatchLogic(data);
            break;
        case Settings:
            SettingLogic(data);
            break;
        case ChangeTime:
            TimeLogic(data);
            break;
        case DateMenu:
            DateLogic(data);
            break;
        case YearMenu:
            YearLogic(data);
            break;
            //these two are not called as of right now.
        case OtherSettings:
            //OtherLogic(data);
            break;
        case Timer:
            break;
            //do nothing.
        default:
            break;

    }
    //change screens if the state has changed.
    if (Watch_State != previous_state) {
        if (Watch_State == DateMenu) {
            isLeapYear();
            //dates and months have a minimum value of 1 not zero.
            Digits[1] = 1;
            Digits[3] = 1;
        }
        
        DisplayState();

    }
}
#define GotoSettings Button_One
//This is state 1.  Do something based on the input.
void WatchLogic(unsigned char data) {
    if (data == GotoSettings) {
        Watch_State = 2;
        return;
    }
    //goto sleep if any other button is pressed.
    GotoSleep();
    Watch_State = 0;

}

#define Up Button_Three
#define Down Button_Two

//
#define UpCursor 0
#define DownCursor 1
#define MinSetCursor 1
//final cursor value is 6.  But for now it's set to 4.
#define MaxSetCursor 4
//State 2, change cursor position and on confirmation switch states.
void SettingLogic(unsigned char data) {
    switch (data) {
        case Exit:
            Watch_State = 1;
            break;
        case Up:
            if (settingcursor > MinSetCursor) {
                settingcursor--;
                updateSettingPos(UpCursor, settingcursor);
            }
            break;
        case Down:
            if (settingcursor < MaxSetCursor) {
                settingcursor++;
                updateSettingPos(DownCursor, settingcursor);
            }
            break;
        case Confirm:
            SettingsConfirm();
            break;
    }

}

void SettingsConfirm(void) {
    switch (settingcursor) {
        case 1:
            Watch_State = ChangeTime;
            break;
        case 2:
            Watch_State = DateMenu;
            break;
        case 3:
            Watch_State = YearMenu;
            break;
        case 4:
            Watch_State = Clock;
            //Watch_State = OtherSettings;
            break;
            //case 5 and case 6 not implmented yet.
        case 5:
            Watch_State = Timer;
            break;
        case 6:
            Watch_State = Clock;
            break;
    }
    settingcursor = 1;
}
unsigned char TimeCursor = 0;
_Bool CurrentlySelected = 0;
#define Left Button_Two
#define Right Button_Three

#define Increment 1
#define Decrement 0
#define Time ChangeTime
#define Date DateMenu
#define Year YearMenu
//Time array is read HH:MM

unsigned char GetTimeBound(unsigned char place) {
    switch (place) {
        case 0:
            return 2;
        case 1:
            if (Digits[0] == 2) {
                return 3;
            } else {
                return 9;
            }
            break;
        case 2:
            return 5;
            break;
        case 3:
            return 9;
            break;
        default:
            return 9;
    }
}
//Leap year changes position 2.
const unsigned char MonthDayCount [12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


//getting upper bound on date.

unsigned char GetDateBound(unsigned char place) {
    unsigned char limit = 1;
    //force the date to change when updating the month to prevent incorrect dates.
    //This is better than making a bunch of conditionals to prevent weird dates like Feburary 31st.
    if (place < 2) {
        Digits[2] = 0;
        Digits[3] = 1;
        UpdateDigit(2, DigitToASCII[Digits[2]]);
        UpdateDigit(3, DigitToASCII[Digits[3]]);
    }
    //first two are month and the third and fourth are the day of the month.
    switch (place) {
        case 0:
            return 1;
        case 1:
            if (Digits[0] == 1) {
                return 2;
            } else {
                return 9;
            }
            break;
        case 2:
            limit = MonthDayCount[(Digits[0]*10 + Digits[1]) - 1] / 10;
            return limit;
        case 3:
            //obviously if this is less than 3 the upper bound is either 8 or 9.
            if (Digits[2] < 3) {
                if (Digits[0] == 0 && Digits[1] == 2 && Digits[2] == 2 && !LeapYear) {
                    return 8;
                }
                return 9;
            }
            //this will either return 0 or 1 so we must add 1 to get the correct result for the upper bound.
            limit = (MonthDayCount[(Digits[0]*10 + Digits[1]) - 1] % 10);
            return limit;
        default:
            return 1;
    }

}
//get this to determine what the upper bound is.  This is the value that the digit is strictly less than.

unsigned char getupperbound(unsigned char type, unsigned char place) {
    //the date can be configured up to 99, that is 2099.
    if (type == Year) {
        return 9;
    }
    if (type == ChangeTime) {
        return GetTimeBound(place);
    }
    if (type == DateMenu) {
        return GetDateBound(place);
    }
    //should never be called.
    return 0;
}
//exists to make the next method nicer looking.
_Bool NeedToResetDate(unsigned char DigitIndex) {
    if ((DigitIndex == 1 || DigitIndex == 3) && Digits[DigitIndex] == 0 && Digits[2] == 0) {
        return 1;
    }
    return 0;
}
//update the currently selected digit.
void ChangeDigit(_Bool Direction, unsigned char DigitIndex) {
    unsigned char upperbound = getupperbound(Watch_State, DigitIndex);
    if (Direction) {
        if (Digits[DigitIndex] >= upperbound) {
            //these two are so that we don't have a zeroth date or month.
            if (Watch_State == DateMenu && DigitIndex == 3 && Digits[2] == 0) {
                Digits[DigitIndex] = 1;
                return;
            } else if (Watch_State == DateMenu && DigitIndex == 1 && Digits[0] == 0) {
                Digits[DigitIndex] = 1;
                return;
            }
            Digits[DigitIndex] = 0;
            return;
        }
        Digits[DigitIndex]++;
    } else {
        if (Digits[DigitIndex] == 0) {
            Digits[DigitIndex] = upperbound;
            return;
        }
        Digits[DigitIndex]--;
        //meant to prevent getting zeroth dates.
        if (Watch_State == DateMenu && NeedToResetDate(DigitIndex)) {
            Digits[DigitIndex] = upperbound;
        }
    }
}
//since TimeLogic and DateLogic are very similar they share this common method.
//data is the input from the button selected.
void DigitOption(unsigned char data, unsigned char Cursor) {
    switch (data) {
        case Up:
            ChangeDigit(Increment, Cursor);
            break;
        case Down:
            ChangeDigit(Decrement, Cursor);
            break;
            //exit
        case Confirm:
            CurrentlySelected = 0;
            break;
    }
}
//confirmation method, do something based on where the cursor is located.
void TimeConfirm(void) {
    if (TimeCursor == 4) {
        Watch_State = Settings;
        SetTime(Digits);
        for (unsigned char i = 0; i < 4; i++) {
            //reset the array to be all zeros.
            Digits[i] = 0;
        }
        CurrentlySelected = 0;
        UpdateTimeArray();
        TimeCursor = 0;
        return;
    }
    CurrentlySelected = 1;
}
//State 3 meant for changing the time.
void TimeLogic(unsigned char data) {
    //Modify digit when true.
    if (CurrentlySelected) {
        DigitOption(data, TimeCursor);
        UpdateDigit(TimeCursor, DigitToASCII[Digits[TimeCursor]]);
        //there isn't a 29 hour day.  Only check when updating the tens hour digit.
        if (Digits[1] > 3&&Digits[0]==2&&TimeCursor==0) {
                    Digits[1] = 0;
                    UpdateDigit(1, DigitToASCII[Digits[0]]);
                }
        return;
    }
    switch (data) {
        case Exit:
            TimeCursor = 0;
            Watch_State = Settings;
            break;
        case Left:
            if (TimeCursor > 0) {
                TimeCursor--;
                updateCursor(TimeCursor);
            }
            break;
        case Right:
            if (TimeCursor < 4) {
                TimeCursor++;
                updateCursor(TimeCursor);
            }
            break;
        case Confirm:
            TimeConfirm();
            break;
    }
}
unsigned char DateCursor = 0;

void DateConfirm(void) {
    if (DateCursor == 4) {
        Watch_State = Settings;
        SetDate(Digits);
        for (unsigned char i = 0; i < 4; i++) {
            //reset the array to be all zeros.
            Digits[i] = 0;
        }
        CurrentlySelected = 0;
        DateCursor = 0;
        return;
    }
    CurrentlySelected = 1;
}
//State 4 meant for changing the date.  It's almost identical to TimeLogic().

void DateLogic(unsigned char data) {
    if (CurrentlySelected) {
        DigitOption(data, DateCursor);
        UpdateDigit(DateCursor, DigitToASCII[Digits[DateCursor]]);
        //these are for weird exception cases, so that we don't have odd dates like march 0th or June 39th.
        //there isn't a 0th month
        if (Digits[0]==0&&Digits[1]==0&&DateCursor==0){
            UpdateDigit(1,DigitToASCII[1]);
            Digits[1]=1;
        }
        //if exceeding 12
        else if (Digits[0]==1&&Digits[1]>2&&DateCursor==0){
            UpdateDigit(1,DigitToASCII[2]);
            Digits[1]=2;
        }
        //to prevent June 39th and other incorrect dates.
        else if (Digits[2]==3&&Digits[3]>1&&DateCursor==2){
            UpdateDigit(3,DigitToASCII[0]);
            Digits[3]=0;
        }
        //to prevent December 0th (or the 0th of a month).
        else if (Digits[2]==0&&Digits[3]==0&&DateCursor==2){
            UpdateDigit(3,DigitToASCII[1]);
            Digits[3]=1;
        }
        return;
    }
    switch (data) {
        case Exit:
            DateCursor = 0;
            Watch_State = Settings;
            break;
        case Left:
            if (DateCursor > 0) {
                DateCursor--;
                updateCursor(DateCursor);
            }
            break;
        case Right:
            if (DateCursor < 4) {
                DateCursor++;
                updateCursor(DateCursor);
            }
            break;
        case Confirm:
            DateConfirm();
            break;
    }
}
unsigned char YearCursor = 2;
const unsigned char Feb28[4]={0,2,2,8};
void YearConfirm(void) {
    if (YearCursor == 4) {
        Watch_State = Settings;
        unsigned char tempyear[2];
        tempyear[0] = Digits[2];
        tempyear[1] = Digits[3];
        //prevent a nonleap year having February 29th.  Only change the date when it's February 29th to the 28th.
        if (IsFeb29()){
            SetDate(Feb28);
        }
        //this only accepts a two element array.
        SetYear(tempyear);
        for (unsigned char i = 0; i < 4; i++) {
            //reset the array to be all zeros.
            Digits[i] = 0;
        }
        CurrentlySelected = 0;
        YearCursor = 2;
        return;
    }
    CurrentlySelected = 1;
}
//State 5, basically a smaller version of DateLogic and TimeLogic.
void YearLogic(unsigned char data) {
    if (CurrentlySelected) {
        DigitOption(data, YearCursor);
        UpdateDigit(YearCursor, DigitToASCII[Digits[YearCursor]]);
        return;
    }
    switch (data) {
        case Exit:
            YearCursor = 2;
            Watch_State = Settings;
            break;
        case Left:
            if (YearCursor > 2) {
                YearCursor--;
                updateCursor(YearCursor);
            }
            break;
        case Right:
            if (YearCursor < 4) {
                YearCursor++;
                updateCursor(YearCursor);
            }
            break;
        case Confirm:
            YearConfirm();
            break;
    }
}
