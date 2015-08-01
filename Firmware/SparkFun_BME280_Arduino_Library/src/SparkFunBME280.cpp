/******************************************************************************
SparkFunBME280.cpp
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
//See SparkFunBME280.h for additional topology notes.

#include "SparkFunBME280.h"
#include "stdint.h"

#include "Wire.h"
#include "SPI.h"

//****************************************************************************//
//
//  Settings and configuration
//
//****************************************************************************//

//Constructor -- Specifies default configuration
BME280::BME280( void )
{
  //Construct with these default settings if nothing is specified

  //Select interface mode
  settings.commInterface = I2C_MODE; //Can be I2C_MODE, SPI_MODE
  //Select address for I2C.  Does nothing for SPI
  settings.I2CAddress = 0x77; //Ignored for SPI_MODE
  //Select CS pin for SPI.  Does nothing for I2C
  settings.chipSelectPin = 10;
  settings.runMode = 0;
  settings.tempOverSample = 0;
  settings.pressOverSample = 0;
  settings.humidOverSample = 0;

}


//****************************************************************************//
//
//  Configuration section
//
//  This uses the stored SensorSettings to start the IMU
//  Use statements such as "myIMU.settings.commInterface = SPI_MODE;" or
//  "myIMU.settings.accelEnabled = 1;" to configure before calling .begin();
//
//****************************************************************************//
uint8_t BME280::begin()
{
  //Check the settings structure values to determine how to setup the device
  uint8_t dataToWrite = 0;  //Temporary variable

  switch (settings.commInterface) {

    case I2C_MODE:
      Wire.begin();
      break;

    case SPI_MODE:
      // start the SPI library:
      SPI.begin();
      // Maximum SPI frequency is 10MHz, could divide by 2 here:
      SPI.setClockDivider(SPI_CLOCK_DIV32);
      // Data is read and written MSb first.
      SPI.setBitOrder(MSBFIRST);
      // Data is captured on rising edge of clock (CPHA = 0)
      // Base value of the clock is HIGH (CPOL = 1)
	  // This was SPI_MODE3 for RedBoard, but I had to change to
	  // MODE0 for Teensy 3.1 operation
      SPI.setDataMode(SPI_MODE3);
      // initalize the  data ready and chip select pins:
      pinMode(settings.chipSelectPin, OUTPUT);
      digitalWrite(settings.chipSelectPin, HIGH);
      break;

    default:
      break;
  }

  //Setup the sensor*************************************

  //Configure the run mode
  dataToWrite = 0; //Start Fresh!
  dataToWrite = (settings.tempOverSample << 5) | (settings.pressOverSample << 2) | settings.runMode;
  //Now, write the patched together data
  writeRegister(0xF4, dataToWrite);
  //while(1);

  dataToWrite = settings.humidOverSample;
  //Write the humidity stuff
  writeRegister(0xF2, dataToWrite);
  
  //Return WHO AM I reg
  uint8_t result = readRegister(0xD0);

  return result;
  
}

//****************************************************************************//
//
//  Temperature Section
//
//****************************************************************************//
int16_t BME280::readRawTemp( void )
{
  int16_t output = readRegisterInt16( 0xFB );
  return output;
}  

float BME280::readTempC( void )
{
  float output = (float)readRawTemp() / 16; //divide by 16 to scale
  output += 25; //Add 25 degrees to remove offset

  return output;
  
}

float BME280::readTempF( void )
{
  float output = (float)readRawTemp() / 16; //divide by 16 to scale
  output += 25; //Add 25 degrees to remove offset
  output = (output * 9) / 5 + 32;

  return output;
  
}

//****************************************************************************//
//
//  Utility
//
//****************************************************************************//
void BME280::readRegisterRegion(uint8_t *outputPointer , uint8_t offset, uint8_t length) {
  //define pointer that will point to the external space
  uint8_t i = 0;
  char c = 0;

  switch (settings.commInterface) {

    case I2C_MODE:
      Wire.beginTransmission(settings.I2CAddress);
      Wire.write(offset);
      Wire.endTransmission();

      // request 6 bytes from slave device
      Wire.requestFrom(settings.I2CAddress, length);
      while ( (Wire.available()) && (i < length))  // slave may send less than requested
      {
        c = Wire.read(); // receive a byte as character
        *outputPointer = c;
        outputPointer++;
        i++;
      }
      break;

    case SPI_MODE:
      // take the chip select low to select the device:
      digitalWrite(settings.chipSelectPin, LOW);
      // send the device the register you want to read:
      SPI.transfer(offset | 0x80);  //Ored with "read request" bit
      while ( i < length ) // slave may send less than requested
      {
        c = SPI.transfer(0x00); // receive a byte as character
        *outputPointer = c;
        outputPointer++;
        i++;
      }
      // take the chip select high to de-select:
      digitalWrite(settings.chipSelectPin, HIGH);
      break;

    default:
      break;
  }

}

uint8_t BME280::readRegister(uint8_t offset) {
  //Return value
  uint8_t result;
  uint8_t numBytes = 1;
  switch (settings.commInterface) {

    case I2C_MODE:
      Wire.beginTransmission(settings.I2CAddress);
      Wire.write(offset);
      Wire.endTransmission();

      Wire.requestFrom(settings.I2CAddress, numBytes);
      while ( Wire.available() ) // slave may send less than requested
      {
        result = Wire.read(); // receive a byte as a proper uint8_t
      }
      break;

    case SPI_MODE:
      // take the chip select low to select the device:
      digitalWrite(settings.chipSelectPin, LOW);
      // send the device the register you want to read:
      SPI.transfer(offset | 0x80);  //Ored with "read request" bit
      // send a value of 0 to read the first byte returned:
      result = SPI.transfer(0x00);
      // take the chip select high to de-select:
      digitalWrite(settings.chipSelectPin, HIGH);
      break;

    default:
      break;
  }
  return result;
}

int16_t BME280::readRegisterInt16( uint8_t offset )
{
	uint8_t myBuffer[2];
	readRegisterRegion(myBuffer, offset, 2);  //Does memory transfer
	int16_t output = (int16_t)myBuffer[0] | int16_t(myBuffer[1] << 8);
	
	return output;
}

void BME280::writeRegister(uint8_t offset, uint8_t dataToWrite) {
  switch (settings.commInterface) {
    case I2C_MODE:
      //Write the byte
      Wire.beginTransmission(settings.I2CAddress);
      Wire.write(offset);
      Wire.write(dataToWrite);
      Wire.endTransmission();
      break;

    case SPI_MODE:
      // take the chip select low to select the device:
      digitalWrite(settings.chipSelectPin, LOW);
      // send the device the register you want to read:
      SPI.transfer(offset & 0x7F);
      // send a value of 0 to read the first byte returned:
      SPI.transfer(dataToWrite);
      // decrement the number of bytes left to read:
      // take the chip select high to de-select:
      digitalWrite(settings.chipSelectPin, HIGH);
      break;

    default:
      break;
  }
}
