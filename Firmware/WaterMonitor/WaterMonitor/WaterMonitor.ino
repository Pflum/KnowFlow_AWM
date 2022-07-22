/*********************************************************************
 * WaterMonitor.ino
 *
 * Copyright (C)    2017   [DFRobot](http://www.dfrobot.com)
 * GitHub Link :https://github.com/DFRobot/watermonitor
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Description:
 * This sample code is mainly used to monitor water quality
 * including ph, temperature, dissolved oxygen, ec and orp,etc.
 *
 * Software Environment: Arduino IDE 1.8.2
 * Software download link: https://www.arduino.cc/en/Main/Software
 *
 * Install the library file：
 * Copy the files from the github repository folder libraries to the libraries
 * in the Arduino IDE 1.8.2 installation directory
 *
 * Hardware platform   : Arduino M0 Or Arduino Mega2560
 * Sensor pin:
 * EC  : A1
 * PH  : A2
 * ORP : A3
 * RTC : I2C
 * DO  : Serial port Rx(0),Tx(1)
 * GravityDO：A4
 * temperature:D5
 *
 * SD card attached to SPI bus as follows:
 * Mega:  MOSI - pin 51, MISO - pin 50, CLK - pin 52, CS - pin 53
 * and pin #53 (SS) must be an output
 * M0:   Onboard SPI pin,CS - pin 4 (CS pin can be changed)
 *
 * author  :  Jason(jason.ling@dfrobot.com)
 * version :  V1.0
 * date    :  2017-04-06
 **********************************************************************/

#include <SPI.h>
#include <EEPROM.h>
#include <SD.h>
#include <Wire.h>
#include <Arduino.h>
#include "GravitySensorHub.h"
#include <stdlib.h>
#include "GravityRtc.h"
#include "OneWire.h"
#include "SdService.h"
#include "Debug.h"
#include "EasyNextionLibrary.h"

// clock module
GravityRtc rtc;
EasyNex myNex(Serial);

// sensor monitor
GravitySensorHub sensorHub;
SdService sdService = SdService(sensorHub.sensors);
void setup() {
	//Serial.begin(9600);
  myNex.begin(9600);/*
  delay(50);
  myNex.writeNum("baud", 115200);
  delay(50);
  myNex.begin(115200);*/
	rtc.setup();
  rtc.adjustRtc(F(__DATE__), F(__TIME__));
	sensorHub.setup();
	sdService.setup();
}


//********************************************************************************************
// function name: sensorHub.getValueBySensorNumber (0)
// Function Description: Get the sensor's values, and the different parameters represent the acquisition of different sensor data     
// Parameters: 0 ph value  
// Parameters: 1 temperature value    
// Parameters: 2 Dissolved Oxygen
// Parameters: 3 Conductivity
// Parameters: 4 Redox potential
// return value: returns a double type of data
//********************************************************************************************

unsigned long updateTime = 0;

void loop() {
  myNex.NextionListen();
	rtc.update();
  myNex.NextionListen();
	sensorHub.update();
  myNex.NextionListen();
	sdService.update();
  myNex.NextionListen();
  
	// ************************* Serial debugging ******************
	if(millis() - updateTime > 2000)
	{
		/*updateTime = millis();
		Serial.print(F("ph= "));
		Serial.print(sensorHub.getValueBySensorNumber(0));
		Serial.print(F("  Temp= "));
		Serial.print(sensorHub.getValueBySensorNumber(1));
		Serial.print(F("  Do= "));
		Serial.print(sensorHub.getValueBySensorNumber(2));
		Serial.print(F("  Ec= "));
		Serial.print(sensorHub.getValueBySensorNumber(3));
		Serial.print(F("  Orp= "));
		Serial.println(sensorHub.getValueBySensorNumber(4));*/
    
    String timestamp = String(rtc.day) + "." + String(rtc.month) + "." + String(rtc.year) + " " + String(rtc.hour) + ":" + String(rtc.minute);
    //String timestamp = String(rtc.hour) + ":" + String(rtc.minute);
    myNex.writeStr("tTime.txt", String(timestamp));
    myNex.writeStr("vPH.txt", String(sensorHub.getValueBySensorNumber(0))); // PH
    myNex.writeStr("vTemp.txt", String(sensorHub.getValueBySensorNumber(1))); // °C
    myNex.writeStr("vDO.txt", String(sensorHub.getValueBySensorNumber(2))); // mg/L
    myNex.writeStr("vEC.txt", String(sensorHub.getValueBySensorNumber(3))); // ms/cm
    myNex.writeStr("vORP.txt", String(sensorHub.getValueBySensorNumber(4))); // mV
	}
}

void trigger0(){
  if(rtc.day < 32){
    rtc.adjustRtc(rtc.year, rtc.month, rtc.day + 1, 0, rtc.hour, rtc.minute, 0);
  } else {
    rtc.adjustRtc(rtc.year, rtc.month, 1, 0, rtc.hour, rtc.minute, 0);
  }
}

void trigger1(){
  if(rtc.month < 13){
    rtc.adjustRtc(rtc.year, rtc.month + 1, rtc.day, 0, rtc.hour, rtc.minute, 0);
  } else {
    rtc.adjustRtc(rtc.year, 1, rtc.day, 0, rtc.hour, rtc.minute, 0);
  }
}

void trigger2(){
  rtc.adjustRtc(rtc.year + 1, rtc.month, rtc.day, 0, rtc.hour, rtc.minute, 0);
}

void trigger3(){
  if(rtc.hour < 24){
    rtc.adjustRtc(rtc.year, rtc.month, rtc.day, 0, rtc.hour + 1, rtc.minute, 0);
  } else {
    rtc.adjustRtc(rtc.year, rtc.month, rtc.day, 0, 0, rtc.minute, 0);
  }
}

void trigger4(){
  if(rtc.minute < 60){
    rtc.adjustRtc(rtc.year, rtc.month, rtc.day, 0, rtc.hour, rtc.minute + 1, 0);
  } else {
    rtc.adjustRtc(rtc.year, rtc.month, rtc.day, 0, rtc.hour, 0, 0);
  }
}

void trigger5(){
  if(rtc.day > 0){
    rtc.adjustRtc(rtc.year, rtc.month, rtc.day - 1, 0, rtc.hour, rtc.minute, 0);
  } else {
    rtc.adjustRtc(rtc.year, rtc.month, 31, 0, rtc.hour, rtc.minute, 0);
  }
}

void trigger6(){
  if(rtc.month > 0){
    rtc.adjustRtc(rtc.year, rtc.month - 1, rtc.day, 0, rtc.hour, rtc.minute, 0);
  } else {
    rtc.adjustRtc(rtc.year, 12, rtc.day, 0, rtc.hour, rtc.minute, 0);
  }
}

void trigger7(){
  rtc.adjustRtc(rtc.year - 1, rtc.month, rtc.day, 0, rtc.hour, rtc.minute, 0);
}

void trigger8(){
  if(rtc.hour > 0){
    rtc.adjustRtc(rtc.year, rtc.month, rtc.day, 0, rtc.hour - 1, rtc.minute, 0);
  } else {
    rtc.adjustRtc(rtc.year, rtc.month, rtc.day, 0, 23, rtc.minute, 0);
  }
}

void trigger9(){
  if(rtc.minute > 0){
    rtc.adjustRtc(rtc.year, rtc.month, rtc.day, 0, rtc.hour, rtc.minute - 1, 0);
  } else {
    rtc.adjustRtc(rtc.year, rtc.month, rtc.day, 0, rtc.hour, 60, 0);
  }
}

//* ***************************** Print the relevant debugging information ************** ************ * /
// Note: Arduino M0 need to replace Serial with SerialUSB when printing debugging information

// ************************* Serial debugging ******************
//Serial.print("ph= ");
//Serial.print(sensorHub.getValueBySensorNumber(0));
//Serial.print("  Temp= ");
//Serial.print(sensorHub.getValueBySensorNumber(1));
//Serial.print("  Orp= ");
//Serial.println(sensorHub.getValueBySensorNumber(4));
//Serial.print("  EC= ");
//Serial.println(sensorHub.getValueBySensorNumber(3));


// ************************************************************ time ********************** **********
//Serial.print("   Year = ");//year
//Serial.print(rtc.year);
//Serial.print("   Month = ");//month
//Serial.print(rtc.month);
//Serial.print("   Day = ");//day
//Serial.print(rtc.day);
//Serial.print("   Week = ");//week
//Serial.print(rtc.week);
//Serial.print("   Hour = ");//hour
//Serial.print(rtc.hour);
//Serial.print("   Minute = ");//minute
//Serial.print(rtc.minute);
//Serial.print("   Second = ");//second
//Serial.println(rtc.second);
