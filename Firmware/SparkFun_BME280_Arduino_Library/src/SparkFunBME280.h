/******************************************************************************
SparkFunBME280.h
BME280 Arduino and Teensy Driver
Marshall Taylor @ SparkFun Electronics
May 20, 2015
https://github.com/sparkfun/BME280_Breakout

Resources:
Uses Wire.h for i2c operation
Uses SPI.h for SPI operation
Either can be omitted if not used

Development environment specifics:
Arduino IDE 1.6.4
Teensy loader 1.23

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!
Distributed as-is; no warranty is given.
******************************************************************************/

// Test derived class for base class SparkFunIMU
#ifndef __BME280_H__
#define __BME280_H__

#include "stdint.h"

#define I2C_MODE 0
#define SPI_MODE 1

//Class SensorSettings.  This object is used to hold settings data.  The application
//uses this classes' data directly.  The settings are adopted and sent to the sensor
//at special times, such as .begin.  Some are used for doing math.
//
//This is a kind of bloated way to do this.  The trade-off is that the user doesn't
//need to deal with #defines or enums with bizarre names.
//
//A power user would strip out SensorSettings entirely, and send specific read and
//write command directly to the IC. (ST #defines below)
//
struct SensorSettings {
  public:
	
  //Main Interface and mode settings
    uint8_t commInterface;
    uint8_t I2CAddress;
    uint8_t chipSelectPin;
	
	uint8_t runMode;
	uint8_t tempOverSample;
	uint8_t pressOverSample;
	uint8_t humidOverSample;

};

//This is the man operational class of the driver.

class BME280
{
  public:
    //IMU settings
    SensorSettings settings;

	//Constructor generates default SensorSettings.
	//(over-ride after construction if desired)
    BME280( void );
    ~BME280() = default;
	
	//Call to apply SensorSettings
    uint8_t begin(void);

	//Returns the raw bits from the sensor cast as 16-bit signed integers
    int16_t readRawAccelX( void );
    int16_t readRawAccelY( void );
    int16_t readRawAccelZ( void );
    int16_t readRawGyroX( void );
    int16_t readRawGyroY( void );
    int16_t readRawGyroZ( void );

    //Returns the values as floats.  Inside, this calls readRaw___();
	float readFloatAlt( void );
    float readFloatPressure( void );

	//Temperature related methods
	int16_t readRawTemp( void );
    float readTempC( void );
    float readTempF( void );


    //The following utilities read and write to the IMU

	//ReadRegisterRegion takes a uint8 array address as input and reads
	//a chunk of memory into that array.
    void readRegisterRegion(uint8_t*, uint8_t, uint8_t );
	//readRegister reads one register
    uint8_t readRegister(uint8_t);
    //Reads two regs, LSByte then MSByte order, and concatenates them
	//Used for two-byte reads
	int16_t readRegisterInt16( uint8_t offset );
	//Writes a byte;
    void writeRegister(uint8_t, uint8_t);
	
	float calcGyro( int16_t );
	float calcAccel( int16_t );
	

    
    private:

};

#endif  // End of __BME280_H__ definition check
