/*
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi
 
 */

#include <SPI.h>
#include <UIPEthernet.h>
#include <EEPROM.h>
#include <DS18B20.h>
#include <DFRobot_PH.h>
#include <DFRobot_EC10.h>
#include <DFRobot_ORP_PRO.h>



#define PH_PIN A2
#define EC_PIN A1
#define TEMP_PIN 5
#define ORP_PIN A3
byte mac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
IPAddress ip(192, 168, 10, 203);
long int orpOffset = -482;

#define ORPVALUEADDR 0x00+5
DS18B20 ds(TEMP_PIN);
EthernetServer server(80);
float orpVoltage,phVoltage,ecVoltage,phValue,ecValue,orpValue;
char orpString[10],tempString[6],phString[10],ecString[10];
double tempValue;
int count = 0, orpCalibrationValue, orpenterCalibrationFlag = 0;
int orpCalibrationFinish  = 0;
DFRobot_PH ph;
DFRobot_EC10 ec;
DFRobot_ORP_PRO orp(0);



void setup() {
  Ethernet.init(10);  // Most Arduino shields

  Serial.begin(115200);
  Serial.println("Ethernet WebServer Example");
  Ethernet.begin(mac, ip);
  ph.begin();
  ec.begin();
  orp.setCalibration(orpOffset);

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }
  
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  int tracker = 1;
  char json[80];
  // Temp
  tempValue = ds.getTempC();
  dtostrf(tempValue, 5, 2, tempString);
  // pH
  phVoltage = analogRead(PH_PIN)/1024.0*5000;
  phValue = ph.readPH(phVoltage,tempValue);
  dtostrf(phValue, 4, 2, phString);
  // EC
  ecVoltage = analogRead(EC_PIN)/1024.0*5000;
  ecValue =  ec.readEC(ecVoltage,tempValue);
  dtostrf(ecValue, 3, 1, ecString);
  // ORP
  orpVoltage = analogRead(ORP_PIN)/1024.0*5000;
  orpValue =  orp.getORP(orpVoltage);
  dtostrf(orpValue, 3, 1, orpString);
  // DO

  sprintf(json, "{\"trackerId\":%d,\"temp\": %s,\"ec\": %s,\"ph\": %s,\"orp\": %s }", tracker, tempString, ecString, phString, orpString);
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          
          client.println(json);
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
  if(count == 0) {
    Serial.println(json);
    count = 5;
  }
  count--;
  char cmd[10];
  if(readSerial(cmd)){
    strupr(cmd);
    Serial.println(cmd);
    if(strstr(cmd,"PH")){
      ph.calibration(phVoltage,tempValue,cmd);       //PH calibration process by Serail CMD
    }
    if(strstr(cmd,"EC")){
      ec.calibration(ecVoltage,tempValue,cmd);       //EC calibration process by Serail CMD
    }
    if(strstr(cmd,"ORP")){
      if(strstr(cmd,"ENTERORP")){
        Serial.println();
        Serial.println(F(">>>Enter ORP Calibration Mode<<<"));
        Serial.println(F(">>>Please disconnect probe and press the botton<<<"));
        Serial.println();
        orpenterCalibrationFlag = 1;
      }
      else if(strstr(cmd,"EXITORP") && orpenterCalibrationFlag){
        if(orpCalibrationFinish){
          orp.setCalibration(orpCalibrationValue);
          Serial.print(F(">>>Calibration Successful"));
        } else {
          Serial.print(F(">>>Calibration Failed"));
        }
        Serial.println(F(", Exit ORP Calibration Mode<<<"));
        Serial.print(orpCalibrationValue);
        Serial.println(F(" in die Variable orpOffset schreiben und neu flashen sonst ist diese eistellung nach einem Stromausfall weg!")); 
        Serial.println();
        orpenterCalibrationFlag = 0;
        orpCalibrationFinish  = 0;
      }
      else if(strstr(cmd,"CALORP")){
        orpCalibrationValue = orp.calibrate(analogRead(ORP_PIN)/1024.0*5000);
        orpCalibrationFinish  = 1;
        Serial.println();
        Serial.println(F(">>>Send EXITORP to Save and Exit<<<")); 
        Serial.println();
      }
    }
  }
}


int i = 0;
bool readSerial(char result[]){
    while(Serial.available() > 0){
        char inChar = Serial.read();
        if(inChar == '\n'){
             result[i] = '\0';
             Serial.flush();
             i=0;
             return true;
        }
        if(inChar != '\r'){
             result[i] = inChar;
             i++;
        }
        delay(1);
    }
    return false;
}
