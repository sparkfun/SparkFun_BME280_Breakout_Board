/******************************************************************************
<filename>
<Title>
<name @ SparkFun Electronics>
<original creation date>
<github repository address>
<multiline verbose description of file functionality>

This file reads and writes arbitrary values to the I2C bus, targeting the BME280



Resources:
<additional library requirements>
Development environment specifics:
<arduino/development environment version>
<hardware version>
<etc>
This code is released under the [MIT License](http://opensource.org/licenses/MIT).
Please review the LICENSE.md file included with this example. If you have any questions 
or concerns with licensing, please contact techsupport@sparkfun.com.
Distributed as-is; no warranty is given.
******************************************************************************/

#include <stdint.h>
#include "SparkFunBME280.h"

#include "Wire.h"
#include "SPI.h"

//Global sensor object
BME280 mySensor;
uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;

uint16_t dig_P1;
int16_t dig_P2;
int16_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int16_t dig_P6;
int16_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;

uint8_t dig_H1;
int16_t dig_H2;
uint8_t dig_H3;
int16_t dig_H4;
int16_t dig_H5;
uint8_t dig_H6;

float t_fine;

void setup()
{
	//Tell the driver what sensor modes to use
	mySensor.settings.commInterface = SPI_MODE;
	mySensor.settings.chipSelectPin = 10;
	mySensor.settings.runMode = 3; //Forced mode
	mySensor.settings.tempOverSample = 1;  // *1
    mySensor.settings.pressOverSample = 1;  // *1
	mySensor.settings.humidOverSample = 1;  // * 1
	
	Serial.begin(57600);
	Serial.print("Program Started\n");
	Serial.print("Starting BME280... result of .begin(): 0x");
	
	//Actually start the driver
	Serial.println(mySensor.begin(), HEX);
	
	Serial.print("ID(0xD0): 0x");
	Serial.println(mySensor.readRegister(0xD0), HEX);
	Serial.print("Reset register(0xE0): 0x");
	Serial.println(mySensor.readRegister(0xE0), HEX);
	Serial.print("ctrl_meas(0xF4): 0x");
	Serial.println(mySensor.readRegister(0xF4), HEX);
	Serial.print("ctrl_hum(0xF2): 0x");
	Serial.println(mySensor.readRegister(0xF2), HEX);
		
	Serial.println();

	Serial.print("\nReading all compensation data, range 0x88:A1, 0xE1:E7\n");
	uint8_t temp8Bit[256];
	for(int i = 0x88; i <= 0xE7; i++)
	{
		//Skip a bit
		if( i == 0xA2 )
		{
			i = 0xE1;
		}

		Serial.print("loc 0x");
		Serial.print(i, HEX);
		Serial.print(": 0x");
		temp8Bit[i] = mySensor.readRegister(i);
		Serial.print(temp8Bit[i], HEX);
		Serial.print("\n");
	}

	Serial.print("\nDisplaying concatenated calibration words\n");
	dig_T1 = ((uint16_t)((temp8Bit[0x89] << 8) + temp8Bit[0x88]));
	dig_T2 = ((int16_t)((temp8Bit[0x8B] << 8) + temp8Bit[0x8A]));
	dig_T3 = ((int16_t)((temp8Bit[0x8D] << 8) + temp8Bit[0x8C]));
	Serial.print("dig_T1, uint16: ");
	Serial.println(dig_T1);
	Serial.print("dig_T2, int16: ");
	Serial.println(dig_T2);
	Serial.print("dig_T3, int16: ");
	Serial.println(dig_T3);
	
	dig_P1 = ((uint16_t)((temp8Bit[0x8F] << 8) + temp8Bit[0x8E]));
	dig_P2 = ((int16_t)((temp8Bit[0x91] << 8) + temp8Bit[0x90]));
	dig_P3 = ((int16_t)((temp8Bit[0x93] << 8) + temp8Bit[0x92]));
	dig_P4 = ((int16_t)((temp8Bit[0x95] << 8) + temp8Bit[0x94]));
	dig_P5 = ((int16_t)((temp8Bit[0x97] << 8) + temp8Bit[0x96]));
	dig_P6 = ((int16_t)((temp8Bit[0x99] << 8) + temp8Bit[0x98]));
	dig_P7 = ((int16_t)((temp8Bit[0x9B] << 8) + temp8Bit[0x9A]));
	dig_P8 = ((int16_t)((temp8Bit[0x9D] << 8) + temp8Bit[0x9C]));
	dig_P9 = ((int16_t)((temp8Bit[0x9F] << 8) + temp8Bit[0x9E]));
	Serial.print("dig_P1, uint16: ");
	Serial.println(dig_P1);
	Serial.print("dig_P2, int16: ");
	Serial.println(dig_P2);
	Serial.print("dig_P3, int16: ");
	Serial.println(dig_P3);
	Serial.print("dig_P4, int16: ");
	Serial.println(dig_P4);
	Serial.print("dig_P5, int16: ");
	Serial.println(dig_P5);
	Serial.print("dig_P6, int16: ");
	Serial.println(dig_P6);
	Serial.print("dig_P7, int16: ");
	Serial.println(dig_P7);
	Serial.print("dig_P8, int16: ");
	Serial.println(dig_P8);
	Serial.print("dig_P9, int16: ");
	Serial.println(dig_P9);
	
	dig_H1 = ((uint8_t)(temp8Bit[0xA1]));
	dig_H2 = ((int16_t)((temp8Bit[0xE2] << 8) + temp8Bit[0xE1]));
	dig_H3 = ((uint8_t)(temp8Bit[0xE3]));
	dig_H4 = ((int16_t)((temp8Bit[0xE4] << 4) + (temp8Bit[0xE5] & 0x0F)));
	dig_H5 = ((int16_t)((temp8Bit[0xE6] << 4) + ((temp8Bit[0xE5] >> 4) & 0x0F)));
	dig_H6 = ((uint8_t)temp8Bit[0xE7]);
	Serial.print("dig_H1, uint8: ");
	Serial.println(dig_H1);
	Serial.print("dig_H2, int16: ");
	Serial.println(dig_H2);
	Serial.print("dig_H3, uint8: ");
	Serial.println(dig_H3);
	Serial.print("dig_H4, int16: ");
	Serial.println(dig_H4);
	Serial.print("dig_H5, int16: ");
	Serial.println(dig_H5);
	Serial.print("dig_H6, uint8: ");
	Serial.println(dig_H6);
	
}

void loop()
{
	Serial.print("ID: 0x");
	Serial.println(mySensor.readRegister(0xD0), HEX);
	Serial.print("Status: 0x");
	Serial.println(mySensor.readRegister(0xF3), HEX);
	//Serial.print("tempMSB: 0x");
	//Serial.println(mySensor.readRegister(0xFA), HEX);
	//Serial.print("tempLSB: 0x");
	//Serial.println(mySensor.readRegister(0xFB), HEX);
	//Serial.print("tempXLSB: 0x");
	//Serial.println(mySensor.readRegister(0xFC), HEX);


	// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
	// t_fine carries fine temperature as global value
	
	float tempTemp;
	tempTemp = ((float)mySensor.readRegister(0xFA) * 4096);
	tempTemp = ((float)mySensor.readRegister(0xFB) * 16) + tempTemp;
	tempTemp = (float)((mySensor.readRegister(0xFC) >> 4) & 0x0F) + tempTemp;
	//Serial.println(tempTemp);

	float var1;
	float var2;
	float T;
	var1 = ((((tempTemp/8) - (dig_T1<<1))) * (dig_T2)) / 2048;
	//Serial.println(var1);
	var2 = (((((tempTemp/16) - (dig_T1)) * ((tempTemp/16) - (dig_T1))) / 4096) * (dig_T3)) /16384;
	//Serial.println(var2);
	t_fine = (var1 + var2);
	T = (t_fine * 5 + 128) / 256;

	//Serial.print("Temperature: ");
	float tempFloat = T / 100;
	//Serial.println(T);
	Serial.print("Temperature = ");
	Serial.print(tempFloat, 2);
	Serial.println(" degrees C");
	//Serial.println();
	
	//Serial.print("pressureMSB: 0x");
	//Serial.println(mySensor.readRegister(0xF7), HEX);
	//Serial.print("pressureLSB: 0x");
	//Serial.println(mySensor.readRegister(0xF8), HEX);
	//Serial.print("pressureXLSB: 0x");
	//Serial.println(mySensor.readRegister(0xF9), HEX);
	
	float tempPressure = 0;
	tempPressure = ((float)mySensor.readRegister(0xF7) * 4096);
	tempPressure = ((float)mySensor.readRegister(0xF8) * 16) + tempPressure;
	tempPressure = (float)((mySensor.readRegister(0xF9) >> 4) & 0x0F) + tempPressure;
	//Serial.println(tempPressure);
	// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
	// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
	float p;
	var1 = t_fine - 128000;
	var2 = var1 * var1 * dig_P6;
	var2 = var2 + ((var1*dig_P5) * 131072);
	var2 = var2 + (dig_P4 * 34359738368);
	var1 = ((var1 * var1 * dig_P3) / 256) + ((var1 * dig_P2) * 4096);
	var1 = (((1.40737488e14)+var1))*(dig_P1) / 8589934592;
	if (var1 == 0)
	{
	// avoid exception caused by division by zero
	}
	p = 1048576-tempPressure;
	p = (((p*2147483648) - var2)*3125)/var1;
	var1 = ((dig_P9) * (p/8192) * (p/8192)) / 33554432;
	var2 = ((dig_P8) * p) /524288;
	p = ((p + var1 + var2) /256) + ((dig_P7)*16);
	
	
	Serial.print("Pressure = ");
	Serial.print(p / 256, 2);
	Serial.println(" Pa");
	//Serial.println();

	float tempHumi = 0;
	tempHumi = ((float)mySensor.readRegister(0xFD) * 256);
	tempHumi = ((float)mySensor.readRegister(0xFE)) + tempHumi;
	//Serial.println(tempHumi);	

	// Returns humidity in %RH as unsigned 32 bit integer in Q22. 10 format (22 integer and 10 fractional bits).
	// Output value of “47445” represents 47445/1024 = 46. 333 %RH

	var1 = (t_fine - (76800));
	
	var1 = (((((tempHumi * 16384) - ((dig_H4) * 1048576) - ((dig_H5) * var1)) + (16384)) / 32768) * (((((((var1 * (dig_H6)) / 1024) * (((var1 * (dig_H3)) /2048) + (32768))) /1024) + (2097152)) * (dig_H2) + 8192) / 16384));
	
	var1 = (var1 - (((((var1 /32768) * (var1/32768)) /128) * (dig_H1)) /16));
	
	var1 = (var1 < 0 ? 0 : var1);
	
	var1 = (var1 > 419430400 ? 419430400 : var1);
	
	float h = var1/4096;
	h = h / 1024;
	
	Serial.print("%RH = ");
	Serial.print(h, 2);
	Serial.println(" %");
	Serial.println();
	
	delay(1000);

}
