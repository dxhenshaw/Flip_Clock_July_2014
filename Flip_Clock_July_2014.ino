/*
 * Flip Clock - 7/27/14 - 
 * August - menuing system; RTC; temperature sensor; rotary encoder
 *
 *
 */

#include "Wire.h"					//
#include "OneWire.h"				// DS18S20 Temperature chip i/o from http://www.pjrc.com/teensy/td_libs_OneWire.html 
#include "RealTimeClockDS1307.h"	// https://github.com/davidhbrown/RealTimeClockDS1307
#include "LiquidCrystal_I2C.h"		// http://arduino-info.wikispaces.com/LCD-Blue-I2C
#include "avr/pgmspace.h"			// for flash memory storage
#include "EEPROM.h"					// for eeprom storage
#include "Servo.h"					// for servo motor

LiquidCrystal_I2C lcd(0x27,20,4);	// set the LCD address to 0x27 for a 16 chars and 4 line display
OneWire  ds(9);						// DS18B20 Temperature chip i/o on pin 9 - OneWire protocol

// Flash Storage: 20 chrs max       12345678901234567890 - these go into flash (program) memory
prog_char menuItem_0[] PROGMEM   = "Chime";     //menu items start here
prog_char menuItem_1[] PROGMEM   = "Quarter chime";
prog_char menuItem_2[] PROGMEM   = "Half hour chime";
prog_char menuItem_3[] PROGMEM   = "Hourly chime style";
prog_char menuItem_4[] PROGMEM   = "Set sleep time";
prog_char menuItem_5[] PROGMEM   = "Set wake time";
prog_char menuItem_6[] PROGMEM   = "Set chime sleep";
prog_char menuItem_7[] PROGMEM   = "Set chime wake";
prog_char menuItem_8[] PROGMEM   = "Show temp";
prog_char menuItem_9[] PROGMEM   = "Show date"; 
prog_char menuItem_10[] PROGMEM  = "Show animations";
prog_char menuItem_11[] PROGMEM  = "tbd 11";
prog_char menuItem_12[] PROGMEM  = "Set time";
prog_char menuItem_13[] PROGMEM  = "Set year";
prog_char menuItem_14[] PROGMEM  = "Set month";
prog_char menuItem_15[] PROGMEM  = "Set day";
prog_char menuItem_16[] PROGMEM  = "tbd 16";	// ** remove if not used
prog_char menuItem_17[] PROGMEM  = "tbd 17";
prog_char menuItem_18[] PROGMEM  = "tbd 18";
prog_char menuItem_19[] PROGMEM  = "tbd 19";
prog_char menuItem_20[] PROGMEM  = "tbd 20";

prog_char menuItem_21[] PROGMEM  = "Turn & push to set";  //status messages from this point on
prog_char menuItem_22[] PROGMEM  = "*Updating board";		
prog_char menuItem_23[] PROGMEM  = "Asleep. Wake at ";	// include trailing space
prog_char menuItem_24[] PROGMEM  = "Chime";
prog_char menuItem_25[] PROGMEM  = "";			// these gaps are used because of 
prog_char menuItem_26[] PROGMEM  = "Temp";		// some sloppy code in 
prog_char menuItem_27[] PROGMEM  = "";			// getNextInformationalMessage()
prog_char menuItem_28[] PROGMEM  = "Date";
prog_char menuItem_29[] PROGMEM  = "";
prog_char menuItem_30[] PROGMEM  = "Animations";
prog_char menuItem_31[] PROGMEM  = "";			
prog_char menuItem_32[] PROGMEM  = "Sleep at";
// 20 characters maximum:           12345678901234567890

// Then set up a table to refer to the strings:
PROGMEM const char *menuTable[] = {   
  menuItem_0,  menuItem_1,  menuItem_2,  menuItem_3,  menuItem_4,  menuItem_5,  menuItem_6,  menuItem_7,
  menuItem_8,  menuItem_9,  menuItem_10,  menuItem_11,  menuItem_12,  menuItem_13,  menuItem_14,  menuItem_15,
  menuItem_16, menuItem_17, menuItem_18, menuItem_19, menuItem_20, menuItem_21, menuItem_22, menuItem_23,
  menuItem_24, menuItem_25, menuItem_26, menuItem_27, menuItem_28, menuItem_29, menuItem_30, menuItem_31, menuItem_32 };

  /***************************************************************
 * Font taken from:                                            *
 * http://www.openobject.org/opensourceurbanism/Bike_POV_Beta_4*
 ***************************************************************/
const byte alphabet[][5] = {	// each entry holds codes for the five columns 
	{},{},{},{},{},{},{},{},
	{},{},{},{},{},{},{},{},
	{},{},{},{},{},{},{},{},
	{},{},{},{},{},{},{},{},
	{0x00,0x00,0x00,0x00,0x00}, // 0x20 32
	{0x00,0x00,0x6f,0x00,0x00}, // ! 0x21 33
	{0x00,0x07,0x00,0x07,0x00}, // " 0x22 34
	{0x14,0x7f,0x14,0x7f,0x14}, // # 0x23 35
	{0x00,0x07,0x04,0x1e,0x00}, // $ 0x24 36
	{0x23,0x13,0x08,0x64,0x62}, // % 0x25 37
	{0x36,0x49,0x56,0x20,0x50}, // & 0x26 38
	{0x00,0x00,0x07,0x00,0x00}, // ' 0x27 39
	{0x00,0x1c,0x22,0x41,0x00}, // ( 0x28 40
	{0x00,0x41,0x22,0x1c,0x00}, // ) 0x29 41
	{0x14,0x08,0x3e,0x08,0x14}, // * 0x2a 42
	{0x08,0x08,0x3e,0x08,0x08}, // + 0x2b 43
	{0x00,0x50,0x30,0x00,0x00}, // , 0x2c 44
	{0x08,0x08,0x08,0x08,0x08}, // - 0x2d 45
	{0x00,0x60,0x60,0x00,0x00}, // . 0x2e 46
	{0x20,0x10,0x08,0x04,0x02}, // / 0x2f 47
	{0x3e,0x51,0x49,0x45,0x3e}, // 0 0x30 48
	{0x00,0x42,0x7f,0x40,0x00}, // 1 0x31 49
	{0x42,0x61,0x51,0x49,0x46}, // 2 0x32 50
	{0x21,0x41,0x45,0x4b,0x31}, // 3 0x33 51
	{0x18,0x14,0x12,0x7f,0x10}, // 4 0x34 52
	{0x27,0x45,0x45,0x45,0x39}, // 5 0x35 53
	{0x3c,0x4a,0x49,0x49,0x30}, // 6 0x36 54
	{0x01,0x71,0x09,0x05,0x03}, // 7 0x37 55
	{0x36,0x49,0x49,0x49,0x36}, // 8 0x38 56
	{0x06,0x49,0x49,0x29,0x1e}, // 9 0x39 57
	{0x00,0x36,0x36,0x00,0x00}, // : 0x3a 58
	{0x00,0x56,0x36,0x00,0x00}, // ; 0x3b 59
	{0x08,0x14,0x22,0x41,0x00}, // < 0x3c 60 
	{0x14,0x14,0x14,0x14,0x14}, // = 0x3d 61 
	{0x00,0x41,0x22,0x14,0x08}, // > 0x3e 62
	{0x02,0x01,0x51,0x09,0x06}, // ? 0x3f 63
	{0x3e,0x41,0x5d,0x49,0x4e}, // @ 0x40 64
	{0x7e,0x09,0x09,0x09,0x7e}, // A 0x41 65
	{0x7f,0x49,0x49,0x49,0x36}, // B 0x42 66
	{0x3e,0x41,0x41,0x41,0x22}, // C 0x43 67
	{0x7f,0x41,0x41,0x41,0x3e}, // D 0x44 68
	{0x7f,0x49,0x49,0x49,0x41}, // E 0x45 69
	{0x7f,0x09,0x09,0x09,0x01}, // F 0x46 70
	{0x3e,0x41,0x49,0x49,0x7a}, // G 0x47 71
	{0x7f,0x08,0x08,0x08,0x7f}, // H 0x48 72
	{0x00,0x41,0x7f,0x41,0x00}, // I 0x49 73
	{0x20,0x40,0x41,0x3f,0x01}, // J 0x4a 74
	{0x7f,0x08,0x14,0x22,0x41}, // K 0x4b 75
	{0x7f,0x40,0x40,0x40,0x40}, // L 0x4c 76
	{0x7f,0x02,0x0c,0x02,0x7f}, // M 0x4d 77
	{0x7f,0x04,0x08,0x10,0x7f}, // N 0x4e 78
	{0x3e,0x41,0x41,0x41,0x3e}, // O 0x4f 79
	{0x7f,0x09,0x09,0x09,0x06}, // P 0x50 80
	{0x3e,0x41,0x51,0x21,0x5e}, // Q 0x51 81
	{0x7f,0x09,0x19,0x29,0x46}, // R 0x52 82
	{0x46,0x49,0x49,0x49,0x31}, // S 0x53 83
	{0x01,0x01,0x7f,0x01,0x01}, // T 0x54 84
	{0x3f,0x40,0x40,0x40,0x3f}, // U 0x55 85
	{0x0f,0x30,0x40,0x30,0x0f}, // V 0x56 86
	{0x3f,0x40,0x30,0x40,0x3f}, // W 0x57 87
	{0x63,0x14,0x08,0x14,0x63}, // X 0x58 88
	{0x07,0x08,0x70,0x08,0x07}, // Y 0x59 89
	{0x61,0x51,0x49,0x45,0x43}, // Z 0x5a 90
	{0x00,0x14,0x00,0x14,0x00}, // [ 0x5b 91 Reworked as four dots for ticker display (was: {0x3c,0x4a,0x49,0x29,0x1e})
	{0x02,0x04,0x08,0x10,0x20}, // \ 0x5c 92
	{0x00,0x41,0x7f,0x00,0x00}, // ] 0x5d 93
	{0x00,0x00,0x07,0x05,0x07}, // ^ 0x5e 94 Reworked as degree symbol for temperature display (was: {0x04,0x02,0x01,0x02,0x04})
	{0x40,0x40,0x40,0x40,0x40}, // _ 0x5f 95
	{0x00,0x00,0x03,0x04,0x00}, // ` 0x60 96
	{0x20,0x54,0x54,0x54,0x78}, // a 0x61 97
	{0x7f,0x48,0x44,0x44,0x38}, // b 0x62 98
	{0x38,0x44,0x44,0x44,0x20}, // c 0x63 99
	{0x38,0x44,0x44,0x48,0x7f}, // d 0x64 100
	{0x38,0x54,0x54,0x54,0x18}, // e 0x65 101
	{0x08,0x7e,0x09,0x01,0x02}, // f 0x66 102
	{0x0c,0x52,0x52,0x52,0x3e}, // g 0x67 103
	{0x7f,0x08,0x04,0x04,0x78}, // h 0x68 104
	{0x00,0x44,0x7d,0x40,0x00}, // i 0x69 105
	{0x20,0x40,0x44,0x3d,0x00}, // j 0x6a 106
	{0x00,0x7f,0x10,0x28,0x44}, // k 0x6b 107
	{0x00,0x41,0x7f,0x40,0x00}, // l 0x6c 108
	{0x7c,0x04,0x18,0x04,0x78}, // m 0x6d 109
	{0x7c,0x08,0x04,0x04,0x78}, // n 0x6e 110
	{0x38,0x44,0x44,0x44,0x38}, // o 0x6f 111
	{0x7c,0x14,0x14,0x14,0x08}, // p 0x70 112
	{0x08,0x14,0x14,0x18,0x7c}, // q 0x71 113
	{0x7c,0x08,0x04,0x04,0x08}, // r 0x72 114
	{0x48,0x54,0x54,0x54,0x20}, // s 0x73 115
	{0x04,0x3f,0x44,0x40,0x20}, // t 0x74 116
	{0x3c,0x40,0x40,0x20,0x7c}, // u 0x75 117
	{0x1c,0x20,0x40,0x20,0x1c}, // v 0x76 118
	{0x3c,0x40,0x30,0x40,0x3c}, // w 0x77 119
	{0x44,0x28,0x10,0x28,0x44}, // x 0x78 120
	{0x0c,0x50,0x50,0x50,0x3c}, // y 0x79 121
	{0x44,0x64,0x54,0x4c,0x44}, // z 0x7a 122
	{0x00,0x08,0x36,0x41,0x41}, // { 0x7b 123
	{0x00,0x00,0x7f,0x00,0x00}, // | 0x7c 124
	{0x41,0x41,0x36,0x08,0x00}, // } 0x7d 125
	{0x04,0x02,0x04,0x08,0x04}, // ~ 0x7e 126
};

// Variables for bit pattern array
	byte bitPattern[25];						// 25 columns of 8 bits (ignore last bit)
	byte pixelList[35];							// array used for random reordering

// Setup shift register variables
	#define number_of_74hc595s 12						// How many shift registers
	#define numOfRegisterPins number_of_74hc595s * 8	// do not touch
	#define rowSelectorPinStart numOfRegisterPins - 16	// what shift register number does row selector circuit begin? (Last 16 pins of the shift register circuit)

 //Variables for shift register
	int SER_Pin = 6;							// pin 14 on the 75HC595 Shift Register (Data) ** switch to byte
	int RCLK_Pin = 7;							// pin 12 on the 75HC595 Shift Register (Latch)
	int SRCLK_Pin = 8;							// pin 11 on the 75HC595 Shift Register (Clock)
	boolean registers[numOfRegisterPins];		// bit array for the total number of 595 registers 

// Variables for rotary encoder & push button
	int encoderPin1 = 2;						// these pins can not be changed 2/3 are special pins ** switch to byte
	int encoderPin2 = 3;						// used for interrupts on rotary encoder
	int encoderSwitchPin = 4;					// push button switch on rotary encoder
	volatile int lastEncoded = 0;				// for interrupt routine - do not change
	volatile int encoderValue = 1;				// for interrupt routine - do not change
	int lastEncoderValue = 1;
	const int encoderGap = 2;					// size of gap from encoder before we pay attention to it
	const byte encoderDelay = 200;				// delay in reading encoder value

// Constants for pushbuttons
	const byte timePlusButton   = 13;
	const byte timeMinusButton  = 12;
	const byte clockSleepButton = 10;
	const byte chimeSleepButton = 11;

// Variables for menuing system
	const byte maxMenuItems = 15;				// Maximum number of menu items (starts at zero)
	const byte msgUseDial = 21;					// "Turn dial"
	char buffer[20];							// max size of message, and max size of lcd screen
	byte activeMenuSelection = 0;				// Which menu item is active
	byte selectedMenuItem = 99;
	boolean chimeSwitch;						// switches and other things to remember 
	boolean quarterChime;
	boolean halfChime;
	byte sleepHour;
	byte wakeHour;
	byte chimeSleepHour;
	byte chimeWakeHour;
	boolean showTemp;
	boolean showDate;
	boolean showAnimations;
	boolean showPhrases;
	boolean hourlyChimeStyle;
	byte prevSpecialDisplay = 0;				// which special display went last time?
	byte prevInfoMessage = 0;					// which informational display message went last time?

// Constants for EEPROM memory (refers to index location in EEPROM where we will store the value) using EEPROM.write(address, value) and EEPROM.read(address, value)
	const byte chimeSwitch_EEPROM		= 0;
	const byte quarterChime_EEPROM		= 1;
	const byte halfChime_EEPROM			= 3;
	const byte sleepHour_EEPROM			= 4;
	const byte wakeHour_EEPROM			= 5;
	const byte chimeSleepHour_EEPROM	= 6;
	const byte chimeWakeHour_EEPROM		= 7;
	const byte showTemp_EEPROM			= 8;
	const byte showDate_EEPROM			= 9;
	const byte showAnimations_EEPROM	= 10;
	const byte showPhrases_EEPROM		= 11;
	const byte hourlyChimeStyle_EEPROM	= 12;

// Variables for Real Time Clock RTC
	byte prevSeconds = 0;
	byte prevMinutes = 0;
	boolean checkForChime = FALSE;
	boolean checkForSpecialDisplay = FALSE;
	boolean sleepMode = FALSE;
	boolean priorSleepMode = FALSE;
	const int specialDisplayInterval = 967;	// consider a special display every 967 seconds (16 minutes, 7 seconds)
	int secondsToNextSpecialDisplay = specialDisplayInterval;
	boolean nextFlippingStep = false;	// is it time to animate the clock ticker?
	byte secondsPassed = 0;				// used in calculations with nextFlippingStep
	byte animationStep = 0;				// which animation step of the ticker is active
	boolean steadyTickerFlag = false;	// flag set when a regular colon is shown as the ticker
	boolean topOfTheHour = false;		// flag to show whether it's the top of the hour
	byte flipTickerStyle = 0;			// ticker style from 0 thru 4
	byte flipTickerDelaySpeed = 0;
	byte animationTime = 99;			// what time of the hour will the random animation occur? Avoid it from happening on startup with 99 value

// Variables for playing pong
	int xOffset = 0;
	int yOffset = 0;

// Variables for servo motor
	int servoPin = 5;
	Servo servo;  
	int angle = 0;   // servo position in degrees 

// Variables for flipping
	const int pauseTime = 5;	// was 25, 5 - prev 50
	const int solenoidTime = 110; // was 110 - prev 150

void setup()
{
	lcd.init();									// initialize the lcd 
	lcd.backlight();							// turn on the backlight

// Initialize shift registers
	pinMode(SER_Pin, OUTPUT);					// Shift Register pin
	pinMode(RCLK_Pin, OUTPUT);					// Shift Register pin
	pinMode(SRCLK_Pin, OUTPUT);					// Shift Register pin
	clearRegisters();							// reset all register pins
	writeRegisters();

// Initialize servo motor
	servo.attach(servoPin);						// initialize servo

// Configure pushbutton pins
	pinMode(timePlusButton,   INPUT);
	pinMode(timeMinusButton,  INPUT);
	pinMode(clockSleepButton, INPUT);
	pinMode(chimeSleepButton, INPUT);
	digitalWrite(timePlusButton, HIGH);
	digitalWrite(timeMinusButton, HIGH);
	digitalWrite(clockSleepButton, HIGH);
	digitalWrite(chimeSleepButton, HIGH);

// Configure rotary encoder
	pinMode(encoderPin1, INPUT);				// rotary encoder
	pinMode(encoderPin2, INPUT);				// interrupt pins
	pinMode(encoderSwitchPin, INPUT);			// push button
	digitalWrite(encoderPin1, HIGH);			// turn pullup resistor on
	digitalWrite(encoderPin2, HIGH);			// turn pullup resistor on
	digitalWrite(encoderSwitchPin, HIGH);		// turn pullup resistor on
	attachInterrupt(0, updateEncoder, CHANGE);	// call updateEncoder() when any high/low changed seen
	attachInterrupt(1, updateEncoder, CHANGE);	// on interrupt 0 (pin 2), or interrupt 1 (pin 3) 

// Configure RTC
	//RTC.setHours(16);							// At startup: set time - TEST CODE ONLY
	//RTC.setMinutes(14);						// Note: Resetting time can help fix RTC clock errors
	//RTC.setSeconds(50);
	//RTC.setYear(2014);
	//RTC.setMonth(9);
	//RTC.setDate(13);
	//RTC.setClock();								// Only needed when initiating a new clock

// Clear out bitPattern array
	for (byte i = 0 ; i < 25; i++){
		//bitPattern[i] = B11111111;	// presume all dots are yellow **make function
		bitPattern[i] = B00000000;		// presume all dots are black **remove for production code
	}
	//clearBoard(' ');	// ** add for production code

// Refresh variables from EEPROM
	chimeSwitch =			EEPROM.read( chimeSwitch_EEPROM );
	quarterChime =			EEPROM.read( quarterChime_EEPROM );
	halfChime =				EEPROM.read( halfChime_EEPROM );              
	sleepHour =				EEPROM.read( sleepHour_EEPROM );
	wakeHour =				EEPROM.read( wakeHour_EEPROM );
	chimeSleepHour =		EEPROM.read( chimeSleepHour_EEPROM );
	chimeWakeHour =			EEPROM.read( chimeWakeHour_EEPROM );
	showTemp =				EEPROM.read( showTemp_EEPROM );
	showDate =				EEPROM.read( showDate_EEPROM );
	showAnimations =		EEPROM.read( showAnimations_EEPROM );
	showPhrases =			EEPROM.read( showPhrases_EEPROM );
	hourlyChimeStyle =		EEPROM.read( hourlyChimeStyle_EEPROM );

}
void loop() {

// main clock code follows
if (abs(lastEncoderValue - encoderValue) > encoderGap) {					// menu selection has changed
	if (encoderValue > lastEncoderValue) {									// figure out what the new menu selection is
		activeMenuSelection ++;												// add 1 to menu
		if (activeMenuSelection > maxMenuItems) activeMenuSelection = 0;
	}else{ 
		activeMenuSelection --;												// subtract 1 from menu
		if (activeMenuSelection > 200) activeMenuSelection = maxMenuItems;
  }
  printLCDActiveMenu(activeMenuSelection);     
  lastEncoderValue = encoderValue;
 }

if(!digitalRead(encoderSwitchPin)) {			// check if encoder button has been pushed - note the not operation
	delay(500);									// pause to allow time for button to be released *** make constant
	selectedMenuItem = activeMenuSelection;
	menuSystem(selectedMenuItem);				// process menu selection
}

RTC.readClock();								// find out what the time is 

topOfTheHour = false;
if ((RTC.getMinutes() == 0) && (RTC.getSeconds() == 0)) {	// if it's the top of the hour
	topOfTheHour = true;									// set a flag
	flipTickerStyle = random(0,5);							// for the next hour, select a ticker style from 0 thru 4
	flipTickerDelaySpeed = random(1,7);						// for the next hour, select a ticker speed from every 1 second to every 6

	// determine when this hour's animation will occur (if it's enabled)
	animationTime =	(random(0,4) * 15) + random(3,14);	// quarter hour segment + 3 to 13 minutes (avoids chiming times)
}

// is it top of the hour and time for sleep mode?
if ((RTC.getHours() == sleepHour) && (topOfTheHour == true) || sleepMode != false) {	// if it's exactly sleep hour at the top of the hour, or sleep mode
	sleepMode = true;	// it is now sleep mode
	if (priorSleepMode == false) {	// this is the first time we're sleeping, so set the display
		printStatusMessage(23);	// "Asleep. Wake at"
		lcd.print(convertTwelveHour(wakeHour));	// print wake time
		clearBoard(' ');	// blank out entire board
		priorSleepMode = true;
	}
}

// is it wake time?
if ((RTC.getHours() == wakeHour) && (RTC.getMinutes() == 0) && (RTC.getSeconds() == 0) || sleepMode == false) {
	if (priorSleepMode == true) {	// we were in sleep mode and just came out of it
		prevMinutes = 99;			// force clock update during showTime
		clearLine(1);
	}
	sleepMode = false;
	priorSleepMode = false;
}

showTime();	// update LCD and board, if required

if ((showAnimations != false) && (animationTime == RTC.getMinutes()) && (RTC.getSeconds() == 15)) paintAnimation();	// do random animation if enabled and if it's time
if (checkForChime != false)          chimeLogic(RTC.getHours(), RTC.getMinutes());		// chime logic
if (checkForSpecialDisplay == true)  specialDisplay();									// is it time for a special display?
checkForChime = false;																	// ensure we don't check next time round
checkForSpecialDisplay = false;															// ensure we don't check next time round
checkButtons();																			// see if any pushbuttons are being pressed
delay(100);	// slow things down, it's only a clock

//===================== end of clock code
}
void flipBY(byte column, byte row) {
	setNegativeColumnOn(column);
		setPositiveRowOn(row);
			writeRegisters();
			delay(solenoidTime);
		setPositiveRowOff(row);
	setNegativeColumnOff(column);
	writeRegisters();
	setBitPattern(column, row, 1);	// update array with a record of this change
}
void flipYB(byte column, byte row) {
	setPositiveColumnOn(column);
		setNegativeRowOn(row);
			writeRegisters();
			delay(solenoidTime);
		setNegativeRowOff(row);
	setPositiveColumnOff(column);
	writeRegisters();
	setBitPattern(column, row, 0);	// update array with a record of this change
}
void setNegativeColumnOn(byte column) {
	// To turn negative column on:
	// Enable = hi, In1 (pin 2 or 10) = high, In2 (pin 7 or 15) = low
	setRegisterPin((column * 3) - 2, 1);	// In1: High
	setRegisterPin((column * 3) - 1, 0);	// In2: Low
	setRegisterPin((column * 3) - 3, 1);	// Enable
}
void setNegativeColumnOff(byte column) {
	// To turn negative column off:
	// Enable = low, In1 (pin 2 or 10) = low, In2 (pin 7 or 15) = low
	setRegisterPin((column * 3) - 2, 0);	// In1: Low
	setRegisterPin((column * 3) - 1, 0);	// In2: Low
	setRegisterPin((column * 3) - 3, 0);	// Disable
}
void setPositiveColumnOn(byte column) {
	// To turn negative column on:
	// Enable = hi, In1 (pin 2 or 10) = low, In2 (pin 7 or 15) = high
	setRegisterPin((column * 3) - 2, 0);	// In1: Low
	setRegisterPin((column * 3) - 1, 1);	// In2: High
	setRegisterPin((column * 3) - 3, 1);	// Enable
}
void setPositiveColumnOff(byte column) {
	// To turn negative column off:
	// Enable = low, In1 (pin 2 or 10) = low, In2 (pin 7 or 15) = low
	setRegisterPin((column * 3) - 2, 0);	// In1: Low
	setRegisterPin((column * 3) - 1, 0);	// In2: Low
	setRegisterPin((column * 3) - 3, 0);	// Disable
}
void setNegativeRowOff(byte row) {
	// position in array is row-1... set it to 0
	setRegisterPin(rowSelectorPinStart + row - 1, 0);
}
void setNegativeRowOn(byte row) {
	// position in array is row-1... set it to 1
	setRegisterPin(rowSelectorPinStart + row - 1, 1);
}
void setPositiveRowOff(byte row) {
	// position in array is row+6... set it to 0
	setRegisterPin(rowSelectorPinStart + row + 6, 0);
}
void setPositiveRowOn(byte row) {
	// position in array is row+6... set it to 1
	setRegisterPin(rowSelectorPinStart + row + 6, 1);
}
void clearRegisters() {  //set all register pins to LOW
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
     registers[i] = LOW;
  }
} 
void writeRegisters() {  // Set and display registers - 595's
  digitalWrite(RCLK_Pin, LOW);
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
    digitalWrite(SRCLK_Pin, LOW);
    int val = registers[i];
    digitalWrite(SER_Pin, val);
    digitalWrite(SRCLK_Pin, HIGH);
  }
  digitalWrite(RCLK_Pin, HIGH);
}
void setRegisterPin(int index, int value) {  //set an individual pin HIGH or LOW
	registers[index] = value;
}
void renderBoard(char* boardText){
	byte renderStyle = random(0,10);										// pick a rendering style between 0 and 9
	for (byte unit = 1; unit < 6; unit++) {								// for each unit on the board 1 thru 5
		char currentCharacter = boardText[unit-1];						// gets the ASCII value of the current letter.

		if (unit == 5) {	// special treatment for last digit

			switch (renderStyle) {
				case 0:
					renderCharacterSequential(unit, currentCharacter);				// now print that character 
					break;
				case 1:
					renderCharacterSequential(unit, currentCharacter);
					break;
				case 2:
					fadeTransition(unit, currentCharacter);
					break;
				case 3:
					fadeTransition(unit, ' ');
					fadeTransition(unit, currentCharacter);
					break;
				case 4:
					barCollapse(unit);
					renderCharacterSequential(unit, currentCharacter);
					break;
				default:	// in case of values 5 thru 9 (i.e, about half the time)
					fadeTransition(unit, currentCharacter);
					break;

			}
		}else{
			renderCharacterSequential(unit, currentCharacter);
		}
	}																	// do remaining units
}
byte calculateStartColumn(byte unit) {	// utility function to return column number
	return ((unit-1) * 5) + 1;
}
void renderCharacterSequential(byte unit, char currentCharacter) {		// flip dots one-by-one, column at a time
	// unit is the physical character display on the board (1-5)
	byte startColumn = calculateStartColumn(unit);								// map unit (1-6) to the column on the board (1-25)
	for (byte column = 1; column < 6; column++){
		byte alphabetColumn = alphabet[currentCharacter][column-1];		// retrieves the column from the 2D array font
		for (byte row = 1; row < 8; row++){								// for row 1 thru 7
			byte bitValue = !!(alphabetColumn & (1 << (row-1)));		// retrieve the bit from the 2d array font
			// The double-not (!!) forces 0 to be 0 and non-zero to be 1
			byte activeDisplayBit = !!(bitPattern[(startColumn + column - 1)-1] & (1 << (row-1)));	// retrieve the bit from the bitPattern array
			byte needToFlip = bitValue ^ activeDisplayBit;				// XOR: do we need to flip this bit on the board?
			if (needToFlip == 1) {
				if (activeDisplayBit == 1) {							// need to flip from yellow to black
					flipYB((startColumn + column - 1), row);
					//setBitPattern((startColumn + column - 1), row, 0);	// update array with a record of this change
				} else {												// need to flip from black to yellow
					flipBY((startColumn + column - 1), row);
					//setBitPattern((startColumn + column - 1), row, 1);	// update array with a record of this change
				}
				delay(pauseTime);
			}
		}
	}
}
void barCollapse(byte unit) {	// collapse character top-down
	byte startColumn = calculateStartColumn(unit);									// map unit (1-6) to the column on the board (1-25)
	byte lowestActiveRow = 0;

	for (byte column = 1; column <= 5; column++) { // for each column
		lowestActiveRow = findLowestActiveRow((startColumn + column - 1)); // find flipped dot with lowest row number
		if (lowestActiveRow > 0) {  // if there are indeed any flipped dots in this column
			for (byte row = (lowestActiveRow + 1); row <= 7; row++) { // for each row from this to end
				flipBY((startColumn + column - 1),  row ); // flip it black
				//setBitPattern((startColumn + column - 1), row, 1);	// update array with a record of this change 1 = Y
			} // next row
		}
	} // next column
 
	for (byte i = 1; i++; i <= 7) { // for up to 7 cycles,  start counting down the rows
		for (byte column = 1; column <= 5; column++) { // for each column
			lowestActiveRow = findLowestActiveRow((startColumn + column - 1)); // find lowest row with active dot
			if (lowestActiveRow > 0) {
				flipYB((startColumn + column - 1),  lowestActiveRow ); // flip it black
				//setBitPattern((startColumn + column - 1), lowestActiveRow, 0);	// update array with a record of this change
			}
		} // next column
	}
}
void fadeTransition(byte unit, char characterIn) { // fade to characterIn
	byte startColumn = calculateStartColumn(unit);									// map unit (1-6) to the column on the board (1-25)
	randomizePixels(); // randomize list of 5x7 = 35 35 pixels
	for (byte i = 0; i < 35; i++) {	// loop through the array, which is now randomized
		byte column = ((pixelList[i]-1) / 7) + 1  ;  // look at a pixel in this column
		byte row = pixelList[i] - ((column - 1) * 7); // look at this row
 
		byte alphabetColumn = alphabet[characterIn][column-1];		// retrieves the column from the 2D array font
		byte bitValue = !!(alphabetColumn & (1 << (row-1)));		// retrieve the bit from the 2d array font
		byte activeDisplayBit = !!(bitPattern[(startColumn + column - 1)-1] & (1 << (row-1)));	// retrieve the bit from the bitPattern array
		byte needToFlip = bitValue ^ activeDisplayBit;				// XOR: do we need to flip this bit on the board?

		if (needToFlip == 1) {
				if (activeDisplayBit == 1) {							// need to flip from yellow to black
					flipYB((startColumn + column - 1), row);
					//setBitPattern((startColumn + column - 1), row, 0);	// update array with a record of this change
				} else {												// need to flip from black to yellow
					flipBY((startColumn + column - 1), row);
					//setBitPattern((startColumn + column - 1), row, 1);	// update array with a record of this change
				}
				delay(pauseTime);
			}
	}
}
byte findLowestActiveRow(byte column) { // find flipped dot with lowest row number - columns 1 thru 25
	byte lowestFlippedRow = 0; // setup returning variable
	for (byte row = 7; row >= 1; row--) { // starting at row 7 and counting down
		byte activeDisplayBit = !!(bitPattern[column - 1] & (1 << (row-1)));	// retrieve the bit from the bitPattern array
		if (activeDisplayBit != 0) lowestFlippedRow = row;// if it is yellow (aka not black) then set the lowest flipped row
	}
return lowestFlippedRow ;
}
void setBitPattern(byte column, byte row, boolean status) {				// update array with a record of a dot's value
	if (status == FALSE) {
		bitClear(bitPattern[column-1], (row-1));						// dot is black
	} else {
		bitSet(bitPattern[column-1], (row-1));							// dot is yellow
	}
}
void randomizePixels() { // randomize list of 5x7 = 35 35 pixels (needs ** randomize based on analog port)
	for (byte i = 0; i <= 34; i++) { // first, populate the array in sequence
		pixelList[i] = i;
	}

	for (byte i = 0; i <= 34; i++) { // now resort it
		byte r = random(i,35); // check visipack code
		byte temp = pixelList[i];
		pixelList[i] = pixelList[r];
		pixelList[r] = temp;
	}
}
void menuSystem(byte selectedMenuItem) {
switch (selectedMenuItem) {  // 

case 0:	// chime on & off
	printStatusMessage(msgUseDial);
	printLCDOptionStatus(chimeSwitch);
	do{
		if (abs(lastEncoderValue - encoderValue) > encoderGap) {
			if (encoderValue != lastEncoderValue) {				// figure out what the new menu selection is
				chimeSwitch = ~chimeSwitch;
				printLCDOptionStatus(chimeSwitch);
            }   
		}
		lastEncoderValue = encoderValue;
		delay(encoderDelay);
	} while (digitalRead(encoderSwitchPin));					// exit when the button is pressed to set time
	EEPROM.write( chimeSwitch_EEPROM, chimeSwitch );			// update EEPROM
	delay(500); //** make constant
	break;

case 1:	// quarter chime on & off
	printStatusMessage(msgUseDial);
	printLCDOptionStatus(quarterChime);
	do{
		if (abs(lastEncoderValue - encoderValue) > encoderGap) {
			if (encoderValue != lastEncoderValue) {				// figure out what the new menu selection is
				quarterChime = ~quarterChime;
				printLCDOptionStatus(quarterChime);
            }   
		}
		lastEncoderValue = encoderValue;
		delay(encoderDelay);
	} while (digitalRead(encoderSwitchPin));					// exit when the button is pressed to set time
	EEPROM.write( quarterChime_EEPROM, quarterChime );			// update EEPROM
	delay(500); //** make constant
	break;

case 2:	// half chime on & off
	printStatusMessage(msgUseDial);
	printLCDOptionStatus(halfChime);
	do{
		if (abs(lastEncoderValue - encoderValue) > encoderGap) {
			if (encoderValue != lastEncoderValue) {				// figure out what the new menu selection is
				halfChime = ~halfChime;
				printLCDOptionStatus(halfChime);
            }   
		}
		lastEncoderValue = encoderValue;
		delay(encoderDelay);
	} while (digitalRead(encoderSwitchPin));					// exit when the button is pressed to set time
	EEPROM.write( halfChime_EEPROM, halfChime );				// update EEPROM
	delay(500); //** make constant
	break;

case 3:	// hourly chime style (single or multiple chimes)
	printStatusMessage(msgUseDial);
	do{
		if (abs(lastEncoderValue - encoderValue) > encoderGap) {
			if (encoderValue != lastEncoderValue) {				// figure out what the new menu selection is
				hourlyChimeStyle = ~hourlyChimeStyle;			// false (0): single, !false (255): multi
				lcd.setCursor(8,3);								// print user-friendly on/off message
				lcd.print("> ");
				if (hourlyChimeStyle == FALSE) { 
					lcd.print("Single");
				}else{
					lcd.print("Multi ");						// trailing space to overwrite "Single"
				}
            }   
		}
		lastEncoderValue = encoderValue;
		delay(encoderDelay);
	} while (digitalRead(encoderSwitchPin));					// exit when the button is pressed to set time
	EEPROM.write( hourlyChimeStyle_EEPROM, hourlyChimeStyle );	// update EEPROM
	delay(500); //** make constant
	break;

case 4: // set clock sleep time
	printStatusMessage(msgUseDial);
	printLCDOptionHour(sleepHour);
    do{
		if (abs(lastEncoderValue - encoderValue) > encoderGap) {
			if (encoderValue > lastEncoderValue) {				// figure out what the new menu selection is
				sleepHour = addHour(sleepHour);
            }else{ 
                sleepHour = subtractHour(sleepHour);
            }
			printLCDOptionHour(sleepHour);
        }          
		lastEncoderValue = encoderValue;
        delay(encoderDelay);
        } while (digitalRead(encoderSwitchPin));				// exit when the button is pressed to set time
	EEPROM.write( sleepHour_EEPROM, sleepHour );				// update EEPROM	
	delay(500);													//  pause to allow time for button to be released
    break; 

case 5: // set clock wake time
	printStatusMessage(msgUseDial);
	printLCDOptionHour(wakeHour);
    do{
		if (abs(lastEncoderValue - encoderValue) > encoderGap) {
			if (encoderValue > lastEncoderValue) {				// figure out what the new menu selection is
				wakeHour = addHour(wakeHour);
            }else{ 
                wakeHour = subtractHour(wakeHour);
            }
			printLCDOptionHour(wakeHour);
        }          
		lastEncoderValue = encoderValue;
        delay(encoderDelay);
        } while (digitalRead(encoderSwitchPin));				// exit when the button is pressed to set time
	EEPROM.write( wakeHour_EEPROM, wakeHour );				// update EEPROM	
	delay(500);													//  pause to allow time for button to be released
    break; 

case 6: // set chime sleep time
	printStatusMessage(msgUseDial);
	printLCDOptionHour(chimeSleepHour);
    do{
		if (abs(lastEncoderValue - encoderValue) > encoderGap){
			if (encoderValue > lastEncoderValue) {				// figure out what the new menu selection is
				chimeSleepHour = addHour(chimeSleepHour);
            }else{ 
                chimeSleepHour = subtractHour(chimeSleepHour);
            }
			printLCDOptionHour(chimeSleepHour);
        }          
		lastEncoderValue = encoderValue;
        delay(encoderDelay);
        } while (digitalRead(encoderSwitchPin));				// exit when the button is pressed to set time
	EEPROM.write( chimeSleepHour_EEPROM, chimeSleepHour );				// update EEPROM	
	delay(500);													//  pause to allow time for button to be released
    break; 

case 7: // set chime wake time
	printStatusMessage(msgUseDial);
	printLCDOptionHour(chimeWakeHour);
    do{
		if (abs(lastEncoderValue - encoderValue) > encoderGap) {
			if (encoderValue > lastEncoderValue) {				// figure out what the new menu selection is
				chimeWakeHour = addHour(chimeWakeHour);
            }else{ 
                chimeWakeHour = subtractHour(chimeWakeHour);
            }
			printLCDOptionHour(chimeWakeHour);
        }          
		lastEncoderValue = encoderValue;
        delay(encoderDelay);
        } while (digitalRead(encoderSwitchPin));				// exit when the button is pressed to set time
	EEPROM.write( chimeWakeHour_EEPROM, chimeWakeHour );				// update EEPROM	
	delay(500);													//  pause to allow time for button to be released
    break; 

case 8:	// show temperature on & off
	printStatusMessage(msgUseDial);
	printLCDOptionStatus(showTemp);
	do{
		if (abs(lastEncoderValue - encoderValue) > encoderGap) {
			if (encoderValue != lastEncoderValue) {				// figure out what the new menu selection is
				showTemp = ~showTemp;
				printLCDOptionStatus(showTemp);
            }   
		}
		lastEncoderValue = encoderValue;
		delay(encoderDelay);
	} while (digitalRead(encoderSwitchPin));					// exit when the button is pressed to set time
	EEPROM.write( showTemp_EEPROM, showTemp );			// update EEPROM
	delay(500); //** make constant
	break;

case 9:	// show date on & off
	printStatusMessage(msgUseDial);
	printLCDOptionStatus(showDate);
	do{
		if (abs(lastEncoderValue - encoderValue) > encoderGap) {
			if (encoderValue != lastEncoderValue) {				// figure out what the new menu selection is
				showDate = ~showDate;
				printLCDOptionStatus(showDate);
            }   
		}
		lastEncoderValue = encoderValue;
		delay(encoderDelay);
	} while (digitalRead(encoderSwitchPin));					// exit when the button is pressed to set time
	EEPROM.write( showDate_EEPROM, showDate );			// update EEPROM
	delay(500); //** make constant
	break;

case 10:	// show special animations on & off
	printStatusMessage(msgUseDial);
	printLCDOptionStatus(showAnimations);
	do{
		if (abs(lastEncoderValue - encoderValue) > encoderGap) {
			if (encoderValue != lastEncoderValue) {				// figure out what the new menu selection is
				showAnimations = ~showAnimations;
				printLCDOptionStatus(showAnimations);
            }   
		}
		lastEncoderValue = encoderValue;
		delay(encoderDelay);
	} while (digitalRead(encoderSwitchPin));					// exit when the button is pressed to set time
	EEPROM.write( showAnimations_EEPROM, showAnimations );			// update EEPROM
	delay(500); //** make constant
	break;

case 11:	// show phrases on & off
	printStatusMessage(msgUseDial);
	printLCDOptionStatus(showPhrases);
	do{
		if (abs(lastEncoderValue - encoderValue) > encoderGap) {
			if (encoderValue != lastEncoderValue) {				// figure out what the new menu selection is
				showPhrases = ~showPhrases;
				printLCDOptionStatus(showPhrases);
            }   
		}
		lastEncoderValue = encoderValue;
		delay(encoderDelay);
	} while (digitalRead(encoderSwitchPin));					// exit when the button is pressed to set time
	EEPROM.write( showPhrases_EEPROM, showPhrases );			// update EEPROM
	delay(500); //** make constant
	break;

case 12: //set time
	printStatusMessage(msgUseDial);
    do{
		if (abs(lastEncoderValue - encoderValue) > encoderGap) {
			if (encoderValue > lastEncoderValue) {				// figure out what the new menu selection is
				addMinute();
            }else{ 
                subtractMinute();
            }
        }          
		lastEncoderValue = encoderValue;
        delay(encoderDelay);	// ** make constant
        } while (digitalRead(encoderSwitchPin));					// exit when the button is pressed to set time      
	delay(500);	//  pause to allow time for button to be released
    break; 

	case 13: //set year
        printStatusMessage(msgUseDial);
        do{
            if (abs(lastEncoderValue - encoderValue) > encoderGap) {
              if (encoderValue > lastEncoderValue) {				// figure out what the new menu selection is
                addYear();
            }else{ 
                subtractYear();
            }
            }          
          lastEncoderValue = encoderValue;
          delay(encoderDelay);	// ** make constant
        } while (digitalRead(encoderSwitchPin));					// exit when the button is pressed to set time       
		delay(500);	//  pause to allow time for button to be released
       break; 

	case 14: //set month
        printStatusMessage(msgUseDial);
        do{
            if (abs(lastEncoderValue - encoderValue) > encoderGap) {
              if (encoderValue > lastEncoderValue) {				// figure out what the new menu selection is
                addMonth();
            }else{ 
                subtractMonth();
            }
            }          
          lastEncoderValue = encoderValue;
          delay(encoderDelay);	// ** make constant
        } while (digitalRead(encoderSwitchPin));					// exit when the button is pressed to set time       
		delay(500);	//  pause to allow time for button to be released
       break; 

	case 15: //set day
        printStatusMessage(msgUseDial);
        do{
            if (abs(lastEncoderValue - encoderValue) > encoderGap) {
              if (encoderValue > lastEncoderValue) {				// figure out what the new menu selection is
                addDay();
            }else{ 
                subtractDay();
            }
            }          
          lastEncoderValue = encoderValue;
          delay(encoderDelay);	// ** make constant
        } while (digitalRead(encoderSwitchPin));					// exit when the button is pressed to set time       
		delay(500);	//  pause to allow time for button to be released
       break; 
}
clearLine(3);
clearLine(2);
clearLine(1);
}
void printLCDOptionStatus(boolean option) {	// print user-friendly on/off message
	lcd.setCursor(8,3);
	lcd.print("> O");
	if (option == FALSE) { 
		lcd.print("FF");
	}else{
		lcd.print("N ");
	}
}
void printLCDInformationalStatus(boolean option) {	// print user-friendly on/off message
	lcd.setCursor(8,1);
	lcd.print("> O");
	if (option == FALSE) { 
		lcd.print("FF");
	}else{
		lcd.print("N ");
	}
}
void printLCDOptionHour(byte hour) {	// print user-friendly hour
	lcd.setCursor(8,3);
	printLCDDigits(hour);
	lcd.print(" Hrs");
}
void checkButtons() {
	byte buttonDelay = 255;
	while (digitalRead(timePlusButton) == LOW) {	// add time
		addMinute();
		delay(buttonDelay);
		buttonDelay--;
	}
	while (digitalRead(timeMinusButton) == LOW) {	// subtract time
		subtractMinute();
		delay(buttonDelay);
		buttonDelay--;
	}

if (digitalRead(chimeSleepButton)  == LOW) {	// flip value of chimeswitch
	chimeSwitch = ~chimeSwitch;
	EEPROM.write( chimeSwitch_EEPROM, chimeSwitch );			// update EEPROM
	printLCDOptionStatus(chimeSwitch);
	lcd.setCursor(0,2);
	lcd.print("Chime");
	delay(2000);
	clearLine(2);
	clearLine(3);
}
if (digitalRead(clockSleepButton)  == LOW) {	// flip value of sleep
	if (sleepMode == true) {
		sleepMode = false;
	} else {
		sleepMode = true;
	}
	printLCDOptionStatus(sleepMode);
	lcd.setCursor(0,2);
	lcd.print("Sleep");
	delay(2000);
	clearLine(2);
	clearLine(3);
}
}
void chimeLogic(byte hour, byte minute){	// chime logic
if ((chimeSwitch != FALSE) &&											// chime is on
	(isItSleepMode(chimeSleepHour, chimeWakeHour, hour) == FALSE) &&	// and it's not chime sleep window
	(sleepMode == FALSE) &&												// we're not sleeping
	(isItSleepMode(sleepHour, wakeHour, RTC.getHours()) == FALSE)) {	// and it's not clock sleep window

	if ((minute == 15) && (quarterChime != FALSE)) {					// do quarter chime
		
		chime(1);
		
	}
	if ((minute == 30) && ((halfChime != FALSE) || (quarterChime != FALSE))) {
														// do half past chime
		chime(1);
		delay(650);
		chime(2);
		
	}
	if ((minute == 45) && (quarterChime != FALSE)) {					// do three quarters chime
		
		chime(1);
		delay(1000);
		chime(2);
		delay(1000);
		chime(1);
		
	}
	if (minute == 0) {
		
		if (hourlyChimeStyle != FALSE) {								// do full set of chimes

			chime(1);
			delay(1000);
			chime(2);
			delay(300);
			chime(2);
			delay(300);
			chime(1);
			delay(3000);

			for (byte i = 0; i < convertTwelveHour(hour); i++) {			// adjusted for 12 hours
				chime(3);
				delay(2000);
			}
		} else {														// only one chime
				chime(3);
			}
		
	}
}
}
void chime(byte hammer) {			// move hammer into place, bang once
	switch (hammer) {
	case 1:
		servo.write(88);			// lower tone at 90 degrees
		break;
	case 2:
		servo.write(75);			// mid tone at 90 degrees
		break;
	case 3:
		servo.write(62);			// high tone at 90 degrees
		break;
	}

	delay(300);						// wait for servo to get into position
	bangHammer();
}
void bangHammer() {
	setRegisterPin(75, 1);	// engage solenoid
	writeRegisters();					// 
	delay(20);							// wait was 15
	setRegisterPin(75, 0);	// turn off solenoid
	writeRegisters();					// 
}
void specialDisplay() {// is it time for a special display?
	if (isItSleepMode(sleepHour, wakeHour, RTC.getHours()) == FALSE) {	// it's not sleep mode, so...
		byte i = getNextSpecialDisplay();	// figure out which special display, if any, goes next

		switch (i) {
		case 1:
			displayTemperature();
			break;
		case 2:
			displayDate();
			break;
		}
		prevMinutes = 99;	// force dots to update 
	}
}
byte getNextSpecialDisplay() {	// figure out which special display, if any, goes next
	prevSpecialDisplay++;								// point to the next special display in sequence
	if (prevSpecialDisplay > 2) prevSpecialDisplay = 1;	// reset to the beginning if necessary
	if ((prevSpecialDisplay == 1) && (showTemp == FALSE)) prevSpecialDisplay++;
	if ((prevSpecialDisplay == 2) && (showDate == FALSE)) prevSpecialDisplay++; // ** bugs around here... ? check
	return prevSpecialDisplay;
}
byte getNextInformationalMessage() {		// figure out which informational message goes next
	prevInfoMessage++;								// point to the next special display in sequence
	if (prevInfoMessage > 10) prevInfoMessage = 1;	// reset to the beginning if necessary
	return prevInfoMessage;
}
void displayTemperature() {	// show temperature for 5 seconds, if switch is on
		printStatusMessage(22);			// display message "Updating board"
		clearLine(0);
		showTemperature();
		clearLine(1);						// remove "Updating board" message
		delay(10000);						// make constant **
		clearLine(0);						// remove "Temp" message
}
void displayDate() {	// show date for 5 seconds, if switch is on
		printStatusMessage(22);			// display message "Updating board"
		clearLine(0);
		showFullDate();
		clearLine(1);					// remove "Updating board" message
		delay(10000);						// make constant **
		clearLine(0);
}
byte addHour(byte hour) {
	if (hour >= 23) {
		hour = 0;
	} else {
	hour++;
	}
	return hour;
}
byte subtractHour(byte hour) {
	if (hour == 0) {
		hour = 23;
	} else {
	hour--;
	}
	return hour;
}
boolean isItSleepMode(byte sleepHour, byte wakeHour, byte nowHour){	// is it currently a sleep mode (for chime or for clock?)
	boolean sleep = FALSE;
	if (sleepHour > wakeHour){
		sleep = TRUE;
		if ((nowHour < sleepHour) && (nowHour >= wakeHour)) sleep = FALSE;
	}
	if (sleepHour < wakeHour){
		sleep = FALSE;
		if ((nowHour > sleepHour) && (nowHour < wakeHour)) sleep = TRUE;
	}
	return sleep;	// false = it's not sleep hour, true = it is sleep hour
}
byte convertTwelveHour(byte hour) {	// convert incoming 24 hour and return its 12 hour equivalent
	if (hour < 13) return hour;
	if (hour > 12) return (hour-12);
}
boolean dialHasTurned(){	// Has the dial been turned on the rotary encoder? set flag if dial was turned or switch set to on
	boolean dialChange = false;
	if ((abs(lastEncoderValue - encoderValue) > encoderGap))	// dial has been turned OR
		dialChange = true; 										// then set to true so we can break and change dial/go to off/go to sleep
		return dialChange;
}
void updateEncoder(){	// interrupt routine called whenever rotary dial is moved
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time
  
  if (encoderValue < 1) encoderValue = 99; //prevent negative numbers on menu
  if (encoderValue > 99) encoderValue = 1; //prevent negative numbers on menu
}
void showTime() {
if (RTC.getSeconds() != prevSeconds) {  // if one second has passed...
	showTimeOnLCD();
	if (isItSleepMode(sleepHour, wakeHour, RTC.getHours()) == FALSE && sleepMode == false) flipTicker(flipTickerStyle,flipTickerDelaySpeed);	// animate the ticker, styles 0 thru 4
	if ((RTC.getMinutes() != prevMinutes) && isItSleepMode(sleepHour, wakeHour, RTC.getHours()) == FALSE && sleepMode == false) {	// if one minute has passed and not sleep mode
		displayFullTime();	// update flip dot board
	}

	if (prevMinutes !=  RTC.getMinutes()) checkForChime = true;				// consider whether to chime when we return
	secondsToNextSpecialDisplay --;
	if ((secondsToNextSpecialDisplay < 1) && isItSleepMode(sleepHour, wakeHour, RTC.getHours()) == FALSE) { 
		checkForSpecialDisplay = true;										// consider whether to do a special display when we return
		secondsToNextSpecialDisplay = specialDisplayInterval;				// reset counter
	}
}                  
prevSeconds = RTC.getSeconds();         // save for next time round
prevMinutes = RTC.getMinutes();
}
void showTimeOnLCD() {
	lcd.setCursor(0, 0);
	lcd.print("Time");
	printLCDDigits(convertTwelveHour(RTC.getHours())); // show 12 hour clock
	printLCDDigits(RTC.getMinutes());
	printLCDDigits(RTC.getSeconds());
	lcd.print(" ");
	if (RTC.getHours() > 11) {
		lcd.print("P");
	}else{
		lcd.print("A");
	}
	lcd.print("M ");  // trailing space solves a problem where a digit sometimes gets printed to the right of the time - clear it out instead

	// print an information message...
	if ((RTC.getSeconds() % 5 == 0) && (sleepMode == false)) {		// we only want to do this every 5 seconds and not in sleep mode
		byte message = getNextInformationalMessage();

		// only do this if message is 1,3,5,7 (i.e. every 5 seconds)
		if (message % 2) {	// message number is odd
			printStatusMessage(23 + message);	// message starts at position 24
			lcd.print(" ");

			switch (message) {	// now print value
			case 1:
				printLCDInformationalStatus(chimeSwitch);
				break;
			case 3:
				printLCDInformationalStatus(showTemp);
				break;
			case 5:
				printLCDInformationalStatus(showDate);
				break;
			case 7:
				printLCDInformationalStatus(showAnimations);
				break;
			case 9:	// status of next sleep event
				//lcd.setCursor(8,1);
				lcd.print(convertTwelveHour(sleepHour));
			}
		} else {
			clearLine(1);
		}
	}
}
void displayFullTime(){
	printStatusMessage(22);			// display message "Updating board"
	byte mins = RTC.getMinutes();	// get minutes
	byte hours = convertTwelveHour(RTC.getHours());	// get hours and convert to 12 hour format

	String xtime;					// this is the string we'll build
	if (hours < 10) xtime += ' ';	// pad hours with a space if reqd
	xtime += String(hours);			// add hours
	xtime += ' ';					// add space for ticker
	if (mins < 10) xtime += '0';	// pad minutes with a zero if reqd
	xtime += String(mins);			// add minutes
	
	char boardText[6] = "";			// prep the char array
	xtime.toCharArray(boardText,6);	// copy the string to the char array
	//strncpy(boardText, xtime, 6);
	renderBoard(boardText);			// render the board
	clearLine(1);					// remove "Updating board" message
}
void flipTicker(byte style, byte secondsGap) {	// call once a second, have logic determine what to do
	// figure out how many seconds have passed since the last time around
	nextFlippingStep = false;
	secondsPassed++;
	if (secondsPassed >= secondsGap) {
		nextFlippingStep = true;	// signal that it's time to do the next animation of the ticker style
		animationStep++;			// which step of the animation we're ready to do *** need to reset to 0 when changing style
		secondsPassed = 0;
	}

	if ((style == 0) && (steadyTickerFlag == false)) {	// two large dots on permanently
		renderCharacterSequential(3,':');				// render a ":"
		steadyTickerFlag = true;						// don't want to update board every second, so set a flag to show it's already been rendered
	}

	if (nextFlippingStep == true) {						// time to do the next animation of the ticker style
		steadyTickerFlag = false;						// will force a board unit refresh when style = 0

		switch (style) {
		case 1:	// two dots at the same time flipping every X seconds

			switch (animationStep) {
			case 1:	// animation step 1: display dots
				flipBY(13,3);
				flipBY(13,5);
				break;
			case 2: // animation step 2: hide dots
				flipYB(13,3);
				flipYB(13,5);
				animationStep = 0;	// reset counter for next time round
				break;
			}
			break;
		
		case 2:	// two dots one at a time flipping every X seconds

			switch (animationStep) {
			case 1:	// animation step 1: display dot 1
				flipBY(13,3);
				break;
			case 2:	// animation step 2: hide dot 1
				flipYB(13,3);
				break;
			case 3:	// animation step 3: display dot 2
				flipBY(13,5);
				break;
			case 4:	// animation step 4: hide dot 2
				flipYB(13,5);
				animationStep = 0;	// reset counter for next time round
				break;
			}
			break;

		case 3:	// four dots one at a time flipping every X seconds

			switch (animationStep) {
			case 1:	// animation step 1: four dots
				renderCharacterSequential(3,91);	// [ 0x5b 91 Reworked as four dots
				break;
			case 2:	// animation step 2: black out one dot
				flipYB(12,3);
				break;
			case 3:	// animation step 3: four dots
				renderCharacterSequential(3,91);
				break;
			case 4:	// animation step 4: black out one dot
				flipYB(14,3);
				break;
			case 5:	// animation step 5: four dots
				renderCharacterSequential(3,91);
				break;
			case 6:	// animation step 6: black out one dot
				flipYB(14,5);
				break;
			case 7:	// animation step 7: four dots
				renderCharacterSequential(3,91);
				break;
			case 8:	// animation step 8: black out one dot
				flipYB(12,5);
				animationStep = 0;	// reset counter for next time round
				break;
		}
			break;

			case 4:	// one dot flipping every X seconds

			switch (animationStep) {
			case 1:	// animation step 1: display dot
				flipBY(13,4);
				break;
			case 2:	// animation step 2: hide dot
				flipYB(13,4);
				animationStep = 0;	// reset counter for next time round
				break;
			}
			break;
	}
	}			
}
void clearBoard(char fillCharacter) {	// clean out the entire board with fillCharacter
	for (byte i = 0 ; i < 25; i++){
		bitPattern[i] = B11111111;	// presume all dots are yellow
	}
	for (byte i = 1; i < 6; i++) {
		renderCharacterSequential(i, ' ');
		renderCharacterSequential(i, fillCharacter);
	}
}
void paintSinWave() {
	//clearBoard(' ');	// clear out the board
	for (byte wave = 0; wave <= 4; wave++) {	// do X number of waves

		float sineFrequency = random(23,50);
		sineFrequency = sineFrequency / 100;				// convert to a number between 2.3 and 4.9
		
		for (byte column = 1; column <= 25; column++) {		// for each column...
				//Serial.print(column);
				//Serial.print(" ");
				//Serial.print(calculateSinPosition(column, sineFrequency));	// paint new value
				flipBY(column, calculateSinPosition(column, sineFrequency));
				//setBitPattern(column, calculateSinPosition(column, sineFrequency), 1);	// update array with a record of this change
		}
		//Serial.println(" ");
		for (byte column = 1; column <= 25; column++) {		// for each column...
				flipYB(column, calculateSinPosition(column, sineFrequency));
				//setBitPattern(column, calculateSinPosition(column, sineFrequency), 0);	// update array with a record of this change
		}
	}
}
int calculateSinPosition(byte loopCounter, float sineFrequency) {
  float transformedSinWave = 0;
  float sinWave = 0;									// prepare a floating point number
  sinWave = sin(loopCounter * sineFrequency);			// this is the value, in radians, for this point of the sine wave
  transformedSinWave = ((sinWave + 1) * 4);
  byte index = transformedSinWave;						// ram floating point number into integer... it will be close enough for union work
  if (index == 0) index++;								// prevent zero from being returned
  return index;
}
void paintAnimation() {	// select an animation
	byte animation = random(0,4);	// pick a number 0 thru 3
	for (byte i = 1; i < 6; i++) {	// first, clear the board
		fadeTransition(i, ' ');
	}

	switch (animation) {
	case 0:
		paintExpandingLine();
		break;
	case 1:
		paintCheckerBoard();
		break;
	case 2:
		paintHorizontalLines();
		break;
	case 3:
		paintSinWave();
		break;
	}
prevMinutes = 99; // force a clock redraw
}
void paintExpandingLine() {
	byte row = random(2,7);
	for (byte column = 1; column < 24; column++) {	// extend column
		flipBY(column, row);
	}
	delay(4000);
	for (byte column = 23; column > 0; column--) {	// retract column
		flipBY(column-1, row-1);
		flipBY(column-1, row+1);
		flipYB(column, row);
		flipYB(column, row-1);
		flipYB(column, row+1);
	}
}
void paintCheckerBoard() {
	for (byte row = 1; row < 8; row=row+2) {
		for (byte column = 1; column < 25; column=column+2) {	//
		 	flipBY(column, row);
			flipBY(column+1, row+1);
	}
	}
	delay(4000);
	for (byte row = 1; row < 8; row=row+2) {
		for (byte column = 1; column < 25; column=column+2) {	//
		 	flipYB(column, row);
			flipYB(column+1, row+1);
	}
	}
}
void paintHorizontalLines() {
	byte column[4];
	byte row[4];
	
	
	for (byte i = 0; i < 4; i++) {	// pick 4 random points on the board and place in an array
		column[i] = random(1,21);
		row[i] = random(1,8);
		flipBY(column[i],row[i]);	// flip each of the 4 points BY
	}

	delay(4000);	// wait 4 seconds
	for (byte j = 0; j < 4; j++) {	// for each point,
		for (byte i = 1; i < 6; i++) {	// extend the line out by 5 additional lines (6 total)
			flipBY(column[j]+i, row[j]);
		}
	}

	delay(4000);

	for (byte j = 0; j < 4; j++) {	// for each point,
		for (byte i = 0; i < 5; i++) {	// remove all but the last point
			flipYB(column[j]+i, row[j]);
		}
	}

	// for all remaining points, scroll them to column 13
	boolean stillMoving = true;
	do {
		stillMoving = true;
		for (byte j = 0; j < 4; j++) {	// for each point,
			if ((column[j]+5) < 13) {
				column[j] = column[j] + 1;
				flipBY(column[j]+5, row[j]);
				flipYB(column[j]+4, row[j]);
			}
			if ((column[j]+5) > 13) {
				column[j] = column[j] - 1;
				flipBY(column[j]+5, row[j]);
				flipYB(column[j]+6, row[j]);
			}
		}
		// are any dots not at column 13?
		for (byte j = 0; j < 4; j++) {	// for each point,
			if ((column[j]+5) != 13) stillMoving = false;
		}
	} while (stillMoving == false);

	// then collapse row 30
	barCollapse(3);
	delay(4000);

}
void playPong() {	// not currently implemented
	int angle = 90;										// right direction to begin with
	calculateNewOffsets(angle);
	int xAxis = 13;
	int yAxis = 4;

	for (int i = 0; i < 50; i++){						// going to do 350 drawings
		flipBY(xAxis, yAxis);			// draw the ball
		//matrix.drawPixel(xAxis, yAxis, BLACK);			// black out the ball next time we do a show()

		//if (angle > 180) drawPadle( 0, yAxis);			// if ball coming to the left, move left padle
		//if (angle < 180) drawPadle(11, yAxis);			// if ball coming to the right, move right padle

		if (xAxis > 23 || xAxis < 1 || yAxis > 6 || yAxis < 2) {	// have we hit a wall?
			angle = calculateNewAngle(angle, xAxis, yAxis);
			calculateNewOffsets(angle);
		}
		delay(60);
		flipYB(xAxis, yAxis);
		xAxis = xAxis + xOffset;
		yAxis = yAxis + yOffset;
	}
}
int calculateNewAngle(int angle, int xAxis, int yAxis) {
	if	    (angle ==   0)				{angle = 135;}
	else if (angle ==  45 && xAxis > 23) {angle = 270;}
	else if (angle ==  45 && yAxis < 2) {angle = 135;}
	else if (angle ==  90)				{angle = 315;}
	else if (angle == 135 && xAxis > 6) {angle = 225;}
	else if (angle == 135 && yAxis > 6) {angle =  45;}
	else if (angle == 180)				{angle =  45;}
	else if (angle == 225 && xAxis < 2) {angle = 135;}
	else if (angle == 225 && yAxis > 6) {angle = 315;}
	else if (angle == 270)				{angle =  90;}
	else if (angle == 315 && xAxis < 2) {angle =  45;}
	else if (angle == 315 && yAxis < 2) {angle = 225;}

	return angle;
}
void calculateNewOffsets(int angle){
	switch (angle) {									// what are new x and y offsets?
				case 0:
					xOffset =  0;
					yOffset = -1;
					break;
				case 45:
					xOffset =  1;
					yOffset = random(-1,1);				// to prevent endless back-and-forth
					break;
				case 90:
					xOffset =  1;
					yOffset =  random(-1,2);			// was 0;
					break;
				case 135:
					xOffset =  1;
					yOffset =  1;
					break;
				case 180:
					xOffset =  0;
					yOffset =  1;
					break;
				case 225:
					xOffset = -1;
					yOffset = +1;
					break;
				case 270:
					xOffset = -1;
					yOffset =  0;
					break;
				case 315:
					xOffset = -1;
					yOffset = -1;
					break;
			}
}
void showTemperature(){	// see http://playground.arduino.cc/Learning/OneWire
	byte i;
	byte present = 0;
	byte data[12];
	byte addr[8];
	int HighByte, LowByte, TReading, Tc_100, Whole;
  
  if ( !ds.search(addr)) {
    //Serial.print("No more addresses.\n");
    ds.reset_search();
    delay(250);
    return;
  }
  
  //Serial.print("R=");
  //for( i = 0; i < 8; i++) {
    //Serial.print(addr[i], HEX);
    //Serial.print(" ");
  //}

  //if ( OneWire::crc8( addr, 7) != addr[7]) {
  //    Serial.print("CRC is not valid!\n");
  //    return;
  //}
  
  //if ( addr[0] != 0x28) {	// Address 0x28 = DS18B20 sensor
//	  //Serial.print(addr[0]);
 //     Serial.print(": Device is not a DS18S20 family device.\n");
//      return;
//  }

  // The DallasTemperature library can do all this work for you!

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  //Serial.print("P=");
  //Serial.print(present,HEX);
  //Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }
  //Serial.print(" CRC=");
  //Serial.print( OneWire::crc8( data, 8), HEX);
  //Serial.println();
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25
  Whole = (((Tc_100 / 100) * 9 ) / 5 ) + 32;  // separate off the whole and fractional portions and convert from C to F
  
	// show temperature on LCD
	lcd.setCursor(0, 0);
	lcd.print("Temp");
	printLCDDigits(Whole);
	lcd.print("F");

	String xtime;	// ** rename
	xtime += String(Whole);
	xtime += '^';
	xtime += 'F';
	char boardText[6] = "";			// prep the char array
	xtime.toCharArray(boardText,6);	// copy the string to the char array
	renderBoard(boardText);			// render the board
}
void showFullDate() {
	lcd.setCursor(0, 0);			// show date on LCD
	lcd.print("Date");
	printLCDDigits(RTC.getMonth());
	lcd.print("/");
	lcd.print(RTC.getDate());

	String xtime;	// ** rename
	xtime += String(RTC.getMonth());
	xtime += '/';
	xtime += String(RTC.getDate());
	char boardText[6] = "";			// prep the char array
	xtime.toCharArray(boardText,6);	// copy the string to the char array
	renderBoard(boardText);			// render the board
}
void printLCDDigits(int digits) {   // utility function for lcd display: prints preceding colon and leading 0
  lcd.print(":");
  if(digits < 10) lcd.print('0');
  lcd.print(digits);
}
void addMinute() {
  if (RTC.getMinutes() == 59){
    RTC.setMinutes(0);
    if (RTC.getHours() == 23){
      RTC.setHours(0);
   }else{
      RTC.setHours(RTC.getHours()+1);}
   }else{  
      RTC.setMinutes(RTC.getMinutes()+1);}
  RTC.setClock();
  showTimeOnLCD();
}
void subtractMinute() {
  if (RTC.getMinutes() == 0){
    RTC.setMinutes(59);
    if (RTC.getHours() == 0){
      RTC.setHours(23);
  }else{
      RTC.setHours(RTC.getHours()-1);}
  }else{  
      RTC.setMinutes(RTC.getMinutes()-1);}
  RTC.setClock();
  showTimeOnLCD();
}    
void addYear() {
	RTC.setYear(RTC.getYear()+1);
	RTC.setClock();
	displayFullTime();
}
void subtractYear() {
	RTC.setYear(RTC.getYear()-1);
	RTC.setClock();
	displayFullTime();
}    
void addMonth() {
	if (RTC.getMonth() == 12){
		RTC.setMonth(1);
   }else{
		RTC.setMonth(RTC.getMonth()+1);
	}
  RTC.setClock();
  displayFullTime();
}
void subtractMonth() {
	if (RTC.getMonth() == 1){
		RTC.setMonth(12);
   }else{
		RTC.setMonth(RTC.getMonth()-1);
	}
  RTC.setClock();
  displayFullTime();
}    
void addDay() {
	if (RTC.getDate() == 31){
		RTC.setDate(1);
   }else{
		RTC.setDate(RTC.getDate()+1);
	}
  RTC.setClock();
  displayFullTime();
}
void subtractDay() {
	if (RTC.getDate() == 1){
		RTC.setDate(31);
   }else{
		RTC.setDate(RTC.getDate()-1);
	}
  RTC.setClock();
  displayFullTime();
}  
void printLCDActiveMenu(byte menu) {	// display the currently-selected menu item
	  clearLine(2);
      getMessage(menu);
	  lcd.setCursor(0, 2);
	  lcd.print(">");	
      lcd.print(buffer);
}
void getMessage(byte index){ //retrieve eeprom message and put it into buffer
  strcpy_P(buffer, (char*)pgm_read_word(&(menuTable[index]))); // Necessary casts and dereferencing, just copy.
}
void printStatusMessage(byte i){        //print status message #i on lcd
      getMessage(i);
      clearLine(1);
      lcd.setCursor(0, 1);
	  //lcd.print("*");
      lcd.print(buffer);
      }  
void clearLine(byte i){	//clear a specific line
 for (byte j = 0; j <20; j++){
   lcd.setCursor(j,i);
   lcd.print(" ");
 }
}
