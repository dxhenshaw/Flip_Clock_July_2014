/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino Uno, Platform=avr, Package=arduino
*/

#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
#define __AVR_ATmega328p__
#define __AVR_ATmega328P__
#define _VMDEBUG 1
#define ARDUINO 103
#define ARDUINO_MAIN
#define __AVR__
#define __avr__
#define F_CPU 16000000L
#define __cplusplus
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __asm__ 
#define __volatile__

#define __builtin_va_list
#define __builtin_va_start
#define __builtin_va_end
#define __DOXYGEN__
#define __attribute__(x)
#define NOINLINE __attribute__((noinline))
#define prog_void
#define PGM_VOID_P int
            
typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}

//
//
void flipBY(byte column, byte row);
void flipYB(byte column, byte row);
void setNegativeColumnOn(byte column);
void setNegativeColumnOff(byte column);
void setPositiveColumnOn(byte column);
void setPositiveColumnOff(byte column);
void setNegativeRowOff(byte row);
void setNegativeRowOn(byte row);
void setPositiveRowOff(byte row);
void setPositiveRowOn(byte row);
void clearRegisters();
void writeRegisters();
void setRegisterPin(int index, int value);
void renderBoard(char* boardText);
byte calculateStartColumn(byte unit);
void renderCharacterSequential(byte unit, char currentCharacter);
void barCollapse(byte unit);
void fadeTransition(byte unit, char characterIn);
byte findLowestActiveRow(byte column);
void setBitPattern(byte column, byte row, boolean status);
void randomizePixels();
void menuSystem(byte selectedMenuItem);
void printLCDOptionStatus(boolean option);
void printLCDInformationalStatus(boolean option);
void printLCDOptionHour(byte hour);
void checkButtons();
void chimeLogic(byte hour, byte minute);
void chime(byte hammer);
void bangHammer();
void specialDisplay();
byte getNextSpecialDisplay();
byte getNextInformationalMessage();
void displayTemperature();
void displayDate();
byte addHour(byte hour);
byte subtractHour(byte hour);
boolean isItSleepMode(byte sleepHour, byte wakeHour, byte nowHour);
byte convertTwelveHour(byte hour);
boolean dialHasTurned();
void updateEncoder();
void showTime();
void showTimeOnLCD();
void displayFullTime();
void flipTicker(byte style, byte secondsGap);
void clearBoard(char fillCharacter);
void paintSinWave();
int calculateSinPosition(byte loopCounter, float sineFrequency);
void paintAnimation();
void paintExpandingLine();
void paintCheckerBoard();
void paintHorizontalLines();
void playPong();
int calculateNewAngle(int angle, int xAxis, int yAxis);
void calculateNewOffsets(int angle);
void showTemperature();
void showFullDate();
void printLCDDigits(int digits);
void addMinute();
void subtractMinute();
void addYear();
void subtractYear();
void addMonth();
void subtractMonth();
void addDay();
void subtractDay();
void printLCDActiveMenu(byte menu);
void getMessage(byte index);
void printStatusMessage(byte i);
void clearLine(byte i);

#include "C:\Program Files\arduino-1.0.3\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Program Files\arduino-1.0.3\hardware\arduino\variants\standard\pins_arduino.h" 
#include "S:\Arduino\Flip_Clock_July_2014\Flip_Clock_July_2014.ino"
#include "S:\Arduino\Flip_Clock_July_2014\LiquidCrystal_I2C.cpp"
#include "S:\Arduino\Flip_Clock_July_2014\LiquidCrystal_I2C.h"
#include "S:\Arduino\Flip_Clock_July_2014\OneWire.cpp"
#include "S:\Arduino\Flip_Clock_July_2014\OneWire.h"
#include "S:\Arduino\Flip_Clock_July_2014\Time.cpp"
#include "S:\Arduino\Flip_Clock_July_2014\Time.h"
#endif
