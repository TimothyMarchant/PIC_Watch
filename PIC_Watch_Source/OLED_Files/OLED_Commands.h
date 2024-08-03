//SSD1306 Commands
#define OLED_Address 0b01111000
#define OLED_Inst 0x00 //send OLED command
#define OLED_Data 0x40 //Write to GGDRAM
enum OLED_Commands{
    SetConstrastComm=0x81, //Use before setting contrast.  Contrast is a 8 bit number.
    defaultconstrast=0x7F, //default constrast value I arbitarily chose.  It's 127.
    DisplayRAM=0xA4,
    DisplayIgnoreRAM=0xA5,
    NoninvertDisplay=0xA6,
    InvertDisplay=0xA7,
    DisplayON=0xAF,
    DisplayOFF=0xAE,
    TurnOffScroll=0x2E,
    TurnOnScroll=0x2F,
    //these two next commands are only used for Page addressing mode.
    SetLowerColumnAdd=0x0F, //This is the max value (for the lower bound) it's between 0x00 and 0x0F.
    SetUpperColumnAdd=0x1F, //This is the max value (for the upper bound) it's between 0x10 and 0x1F.
    //Must use this command before using these next ones
    SetMemoryForMode=0x20,
    SetHorzAddMode=0x00, //Set address mode to horizontal mode.
    SetVertAddMode=0x01, //Set address mode to vertical mode.
    SetPageAddMode=0x02, //Set address mode to page mode.
    //Must use this next command before using the next two
    SetColumnAddress=0x21,
    //These next two commands are only for horizontal and vertical address mode.  The range of values is from 0-127.
    ColumnStartAdd=0x00, //this is the minimun value.
    ColumnEndAdd=0x7F, //This is the maximun value
    //Must use this next command before the next two.
    SetPageAddress=0x22,
    //set page range.  Range is from 0 to 7.  Only for horizontal and vertical address modes.
    PageStartAdd=0x00,
    PageEndAdd=0x07,
    //Set Page Address for page mode.  ranges from 0 to 7.
    PageModeAdd=0xB7, //This is the max value.  Min value is 0xB0.
    //hardware config
    //Set display ram StartLine register from 0 to 63.
    SetMinDisplayLine=0x40, 
    SetMaxDisplayLine=0x7F,
    //default Segment remap
    SegmentMap=0xA0,
    //Set multiplex ratio
    SetMultiplexComm =0xA8, //Must use first before applying next command
    MaxMultiplexValue=0x3F,
    MinMultiplexValue=0x0F, // <14 are invalid values.
    //setcom direction
    SetCOMNormal_DIR=0xC0,
    //set display offset
    DisplayOffSetComm=0xD3,
    DisplayOffSetMax=0x3F,
    DisplayOffSetOFF=0x00,
    SetCOMconfigComm=0xDA,
    DefaultCOMconfig=0x12, //Alternate COM config.  Needed for 128x64.  Use 0x02 for a 128x32 display.
    SegmentRemap=0xA1, //remap COM0 to column address 127.
    RemapScanDir=0xC8, //Scan from COM 127 to COM 0.
    //Timing and Driving commands
    SetDisplayClockDivideComm=0xD5, //must use before the next command
    //use both of these in conjugation with eachother.
    SetDisplayDivideration=0x0F, //max value make sure it's not 0x00 otherwise it puts in reset.  the number is the value + 1. only pays attention to lower 4 bits
    SetOscilMAX=0xF0, //Max value.  Range from 0x00 to 0xF0 only pays attention to upper 4 bits.
    SetOscilDefault=0x80, //recommend value
    //precharge period
    SetChargeComm=0xD9, //must use before the next command.
    SetPreChargePeriod=0xFF, //max value First digit is the first phase.  Second digit is the second phase.
    VCOMHDeselectComm=0xDB, //use before next command
    VCOMHDeselect=0x00,
    TurnOnChargePumpComm1=0x8D, //Turns on chargepump.  First command.
    TurnOnChargePumpComm2=0x14, //Second command for chargepump.
    NOP=0xE3 //No operation.
};
//required initzaltion commands for startup.
#define OLED_Init_Inst_Length 28
const unsigned char OLED_Init_Inst[OLED_Init_Inst_Length]={SetMultiplexComm,MaxMultiplexValue,DisplayOffSetComm,DisplayOffSetOFF,
    SetMinDisplayLine,SegmentRemap,RemapScanDir,SetCOMconfigComm,DefaultCOMconfig,SetConstrastComm,defaultconstrast,
    DisplayRAM,NoninvertDisplay,SetDisplayClockDivideComm,SetOscilDefault,TurnOnChargePumpComm1,TurnOnChargePumpComm2,DisplayON,
    TurnOffScroll,SetMemoryForMode,SetHorzAddMode,SetColumnAddress,ColumnStartAdd,ColumnEndAdd,SetPageAddress,PageStartAdd,PageEndAdd,};