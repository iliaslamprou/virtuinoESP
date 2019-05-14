/* Virtuino Virtuino_ESP_WifiServer Library ver 1.71
 * Created by Ilias Lamprou
 * Updated Aug 11 2018
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino
 */
 

//========= Virtuino general methods  
//  void vDigitalMemoryWrite(int digitalMemoryIndex, int value)       write a value to a Virtuino digital memory   (digitalMemoryIndex=0..31, value range = 0 or 1)
//  int  vDigitalMemoryRead(int digitalMemoryIndex)                   read  the value of a Virtuino digital memory (digitalMemoryIndex=0..31, returned value range = 0 or 1)
//  void vMemoryWrite(int memoryIndex, float value);                  write a value to Virtuino memory             (memoryIndex=0..31, value range as float value)
//  float vMemoryRead(int memoryIndex);                               read a value of  Virtuino memory             (memoryIndex=0..31, returned a float value
//  int getPinValue(int pin);                                         read the value of a Pin. Usefull to read the value of a PWM pin



#ifndef Virtuino_WifiServer_h
#define Virtuino_WifiServer_h

#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
 #else
 #include <ESP8266WiFi.h>
#endif



#include "Arduino.h"
#include "VirtuinoESP.h"


//====================================================================
 class Virtuino_ESP_WifiServer : public VirtuinoESP {
  public:                                            
   Virtuino_ESP_WifiServer(WiFiServer* server);
   void run();
   void vDelay(long milliseconds);
 
  private:
 
 };



#endif

