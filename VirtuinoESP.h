/* Virtuino general ESP web server Library ver 1.72
 * Created by Ilias Lamprou
 * Updated Aug 11 2018
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino
 */


#ifndef VirtuinoESP_h
#define VirtuinoESP_h

//=================   SELECT YOUR BOARD =======
//=================   SELECT YOUR BOARD =======
//=================   SELECT YOUR BOARD ======= Enable your board line and disable the other lines 

#define WEMOS_D1_MINI_PRO                //WeMos D1 mini or other with similar pin map  
// #define NODEMCU                         //NodeMCU  or other with similar pin map  
// #define ESP8266_UNSPECIFIED               //General ESP8266 module. You have to config the pinsMap manually
                                           // ESP32 is auto detect


//-------- pinsMap for ESP32 dev 
 #if defined(ARDUINO_ARCH_ESP32)
     #define boardPinsCount 40   
     #define digitalPinsMap_ {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39}
     #define analogInputPinsCount 40  
     #define analogInputPinsMap_ {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39}
 #else
 //-------- pinsMap for Wemos D1 mini board
  #if defined WEMOS_D1_MINI_PRO  
  #define boardPinsCount 9                                  // Board digital pins count
  #define digitalPinsMap_ {D0,D1,D2,D3,D4,D5,D6,D7,D8}      // digital pinsMap
  #define analogInputPinsCount 1                            // Analog pins count
  #define analogInputPinsMap_ { A0}                         // analog pinsmap
 
 //-------- pinsMap for NodeMCU board
  #elif defined NODEMCU
  #define boardPinsCount 11                                   // Board digital pins count
  #define digitalPinsMap_ {D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10} // digital pinsMap
  #define analogInputPinsCount 1                              // Analog pins count
  #define analogInputPinsMap_ {A0}                            // analog pinsmap

//-------- pinsMap for other esp boards
 #elif defined ESP8266_UNSPECIFIED
  #define boardPinsCount 11                                   // ESP general board pins count
  #define digitalPinsMap_ {0,1,2,3,4,5,12,13,14,15,16}
  #define analogInputPinsCount 1  
  #define analogInputPinsMap_ { A0}
  
 #endif

 #endif




  
  



#include "Arduino.h"




#define esp_firmwareCode  "!C00=1.72$"           // This firmware version
#define esp_virtualDigitalMemorySize 32          // Size of virtual pins DV type integer
#define esp_virtualAnalogMemorySize 32           // Size of virtual pins V type float

#define esp_ERROR_PIN       1
#define esp_ERROR_VALUE     2
#define esp_ERROR_TYPE      3
#define esp_ERROR_SIZE      4
#define esp_ERROR_PASSWORD  5
#define esp_ERROR_COMMAND   6
#define esp_ERROR_UNKNOWN   7

#define esp_COMMAND_START_CHAR '!'                 // All Virtuino commands starts with !
#define esp_COMMAND_END_CHAR   '$'                 // All Virtuino commands ends with $
#define esp_COMMAND_ERROR  "E00="           

#define esp_TEXT_COMMAND_MAX_SIZE 80               // Set the max supported size of text commands
//====================================================================
 class VirtuinoESP {
  public:                                            
  VirtuinoESP();
  void  checkVirtuinoCommand(String* command);     
  void vDigitalMemoryWrite(int digitalMemoryIndex, int value);
  int  vDigitalMemoryRead(int digitalMemoryIndex);
  void vMemoryWrite(int memoryIndex, float value);
  float vMemoryRead(int memoryIndex);
  int getPinValue(int pin);                             // Useful to read the value of a PWM pin 
  long lastCommunicationTime;                           // The time in millis of last communication with Virtuino app
  boolean DEBUG=false;
  String password=""; 

  //-- Text Command functions
  void clearTextBuffer();
  int textAvailable();
  String getText(byte ID);
  void sendText(byte ID, String text);
  String responseBuffer="";
  String textToSendCommandBuffer="";
  
  private:
  char  getCommandType(char c);
  int  getCommandPin(String* aCommand);
  float getCommandValue(String* aCommand);
  String getErrorCommand(byte code);
  void executeReceivedCommand(char activeCommandType, int activeCommandPin ,String* commandString, boolean returnInfo);
  int  getBoardDigitalPin(int pinIndex);
  int  getBoardAnalogPin(int analogPinIndex);
  byte digitalPinsMap[boardPinsCount] =   digitalPinsMap_ ;
  byte analogInputPinsMap[analogInputPinsCount]  = analogInputPinsMap_  ;
  byte arduinoPinsValue[boardPinsCount] =  digitalPinsMap_;
  int virtualDigitalMemory[esp_virtualDigitalMemorySize]      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};                            
  float virtualFloatMemory[esp_virtualAnalogMemorySize]       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};                               
  
  
  //-- Text Command functions
  String textReceivedCommandBuffer="";
  String urlencode(String* str);
  unsigned char h2int(char c);
  String urldecode(String* str);
  void clearTextByID(byte ID, String* textBuffer);
  void addTextToReceivedBuffer(byte ID, String* text);
   
 };


#endif



//========= VirtuinoESP  public methods to use on your code

//  void vDigitalMemoryWrite(int digitalMemoryIndex, int value)       write a value to a Virtuino digital memory   (digitalMemoryIndex=0..31, value range = 0 or 1)
//  int  vDigitalMemoryRead(int digitalMemoryIndex)                   read  the value of a Virtuino digital memory (digitalMemoryIndex=0..31, returned value range = 0 or 1)
//  void vMemoryWrite(int memoryIndex, float value);                  write a value to Virtuino memory             (memoryIndex=0..31, value range as float value)
//  float vMemoryRead(int memoryIndex);                               read a value of  Virtuino memory             (memoryIndex=0..31, returned a float value
//  int getPinValue(int pin);                                         read the value of a Pin. Usefull to read the value of a PWM pin
//  long lastCommunicationTime;                                       Stores the last communication with Virtuino time
//  void clearTextBuffer();                                           Clear the received text buffer
//  int textAvailable();                                              Check if there is text in the received buffer
//  String getText(byte ID);                                          Read the text from Virtuino app
//  void sendText(byte ID, String text);                              Send text to Virtuino app  

