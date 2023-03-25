/*
  WriteMultipleFields
  
  Description: Writes values to fields 1,2,3,4 and status in a single ThingSpeak update every 20 seconds.
  
  Hardware: ESP8266 based boards
  
  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!
  
  Note:
  - Requires ESP8266WiFi library and ESP8622 board add-on. See https://github.com/esp8266/Arduino for details.
  - Select the target hardware from the Tools->Board menu
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and 
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.  
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.
  
  For licensing information, see the accompanying license file.
  
  Copyright 2018, The MathWorks, Inc.
*/

#include "ThingSpeak.h"
#include "secrets.h"
#include "MAX30100_PulseOximeter.h"
#include <ESP8266WiFi.h>
#define REPORTING_PERIOD_MS 1000
PulseOximeter pox;

float BPM, SpO2,temp;
uint32_t tsLastReport = 0;
char ssid[] = "SSID";   // your network SSID (name) 
char pass[] = "12345689";   // your network password

int keyIndex = 0;            
WiFiClient  client;

unsigned long myChannelNumber =  1008677;
const char * myWriteAPIKey = "HWMOBW9LXW6EHSOV";


void onBeatDetected(){
    Serial.println("Beat Detected!");
}

void setup() {
  Serial.begin(9600);  // Initialize serial
  pinMode(16, OUTPUT);
   pinMode(A0, INPUT);
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  if (!pox.begin())
      {
         Serial.println("FAILED");
         for(;;);
      }
      else
      {
         Serial.println("SUCCESS");
         pox.setOnBeatDetectedCallback(onBeatDetected);
      }
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  pox.update();
  BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
        Serial.print("Heart rate:");
        Serial.print(BPM);
        Serial.print(" bpm / SpO2:");
        Serial.print(SpO2);
        Serial.println(" %");
        tsLastReport = millis();
    }
  temp=analogRead(A0);
  temp=(3.3*temp*1000.0)/(1024*10);

  ThingSpeak.setField(1, BPM);
  ThingSpeak.setField(2, SpO2);
  ThingSpeak.setField(3, temp);

  
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  delay(15000); // Wait 15 seconds to update the channel again
}
