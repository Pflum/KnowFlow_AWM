/*********************************************************************
* GravityRtc.h
*
* Copyright (C)    2017   [DFRobot](http://www.dfrobot.com),
* GitHub Link :https://github.com/DFRobot/watermonitor
* This Library is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Description:Get real-time clock data
*
* Product Links：
*
* Sensor driver pin：I2C
*
* author  :  Jason(jason.ling@dfrobot.com)
* version :  V1.0
* date    :  2017-04-18
**********************************************************************/

#pragma once
#include <Arduino.h>

#define RTC_Address   0x32  //RTC_Address

class GravityRtc
{
public:
	GravityRtc();
	~GravityRtc();

public:
	// Year Month Day Weekday Minute Seconds
	unsigned int year;
	unsigned char month;
	unsigned char day;
	unsigned char week;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;

	// initialize the RTC time to set the corresponding year, month, day, day, minute, minute
	void initRtc();

  //Initialize RTC time to set the corresponding year, month, day, Weekday Minute Second
  void adjustRtc(const __FlashStringHelper* date, const __FlashStringHelper* time);
  
  void adjustRtc(uint16_t year,uint8_t month,uint8_t day,uint8_t week,
              uint8_t hour,uint8_t minute,uint8_t second);

	// initialization
	void  setup ();

	// update the sensor data
	void  update ();


private:
	unsigned char date[7];

	// read the clock data
	void  readRtc ();

	// parse RTC data
	void processRtc();

	// decimal to BCD
	char decTobcd(char num);
	void WriteTimeOn(void);
	void WriteTimeOff(void);
	unsigned long timeUpdate;
  uint8_t conv2d(const char* p);
  //adjust RTC
  uint8_t dayOfTheWeek();
  // number of days since 2000/01/01, valid for 2001..2099
  uint16_t date2days(uint16_t y, uint8_t m, uint8_t d);

};
