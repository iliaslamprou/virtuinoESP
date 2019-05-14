
/* Virtuino NodeMCU ESP8266 web server and WiFi access point example No3   
 * Example: How to send or receive Text Commands from Virtuino app 
 * Created by Ilias Lamprou
 * Updated Feb 1 2016
 * Before  running this code config the settings below as the instructions on the right
 * 
 * 
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino&hl=el
 */


/*========= Virtuino General methods  
*
*  void vDigitalMemoryWrite(int digitalMemoryIndex, int value)       write a value to a Virtuino digital memory   (digitalMemoryIndex=0..31, value range = 0 or 1)
*  int  vDigitalMemoryRead(int digitalMemoryIndex)                   read  the value of a Virtuino digital memory (digitalMemoryIndex=0..31, returned value range = 0 or 1)
*  void vMemoryWrite(int memoryIndex, float value);                  write a value to Virtuino memory             (memoryIndex=0..31, value range as float value)
*  float vMemoryRead(int memoryIndex);                               read a value of  Virtuino memory             (memoryIndex=0..31, returned a float value
*  run();                                                            neccesary command to communicate with Virtuino android app  (on start of void loop)
*  int getPinValue(int pin);                                         read the value of a Pin. Usefull to read the value of a PWM pin
*  long lastCommunicationTime;                                       Stores the last communication with Virtuino time
*/ 


// 1.  On Arduino IDE software select the board NodeMCU. If the board isn't in boards list, open the boards manager, on the search line enter esp8266 and download all ESP8266 boards  
// 2.  Open the Virtuino library file: VirtuinoESP.h,  enable the line #define NODEMCU and disable the other boards
// 3.  Connect the NODEMCU ESP8266 module to usb and upload this code

#include <ESP8266WiFi.h>
#include "Virtuino_ESP_WifiServer.h"


const char* ssid = "WIFI NETWORK";            //Enter your wifi network ssid and password
const char* password = "WIFI_PASSWORD";
WiFiServer server(8000);                      // Server port


Virtuino_ESP_WifiServer virtuino(&server);

//NodeMCU ESP8266 pins= D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10                  

// Example variables
 byte storedPinState;

//============================================================== setup
//==============================================================
//==============================================================
void setup() {
   //----- Virtuino settings
  virtuino.DEBUG=true;                         // set this value TRUE to enable the serial monitor status
  virtuino.password="1234";                    // Set a password to your web server for more protection 
                                               // avoid special characters like ! $ = @ # % & * on your password. Use only numbers or text characters

  Serial.begin(9600);                          // Enable this line only if DEBUG=true
  delay(10);

  //----- NodeMCU module settings
  //----- prepare GPIO2
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);
  
  //----  1. Settings as Station - Connect to a WiFi network
  Serial.println("Connecting to "+String(ssid));
  
   // If you don't want to config IP manually disable the next four lines
  IPAddress ip(192, 168, 1, 140);            // where 150 is the desired IP Address   
  IPAddress gateway(192, 168, 1, 1);         // set gateway to match your network
  IPAddress subnet(255, 255, 255, 0);        // set subnet mask to match your network
  WiFi.config(ip, gateway, subnet);          // If you don't want to config IP manually disable this line
  
  WiFi.mode(WIFI_STA);                       // Config module as station only.
  WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
    }
   Serial.println("");
   Serial.println("WiFi connected");
   Serial.println(WiFi.localIP());
 
  //----   2. Settings as Access point - Create a private Wifi Network. Enable the next five lines to use module as Acces point 
  // Serial.print("Setting soft-AP ... ");                   // Default IP: 192.168.4.1
  // WiFi.mode(WIFI_AP);                                     // Config module as Acces point only.  Set WiFi.mode(WIFI_AP_STA); to config module as Acces point and station
  // boolean result = WiFi.softAP("NodeMCU", "12345678");      // SSID: NodeMCU   Password:12345678
  // if(result == true)  Serial.println("Server Ready");
  // else Serial.println("Failed!");
  
  // ---- Start the server
  server.begin();
  Serial.println("Server started");

 //---- Enter your setup code below
  
  pinMode(D4,OUTPUT);             // On Virtuino panel add a switch to pin D4 to enable or disable the board led
  pinMode(D5,INPUT);            // connect a switch.  On Virtuino panel add a Led to pin D5
 
}


//============================================================== loop
//==============================================================
//==============================================================
void loop() {
  virtuino.run();

   // enter your loop code here.
   //------ avoid to use delay() function in your code

   //--------------------------------------------------------
   //----- example 1 how to read a Text Command from virtuino
   if (virtuino.textAvailable()){          // check for a new received TEXT Command from Virtuino app
     String text= virtuino.getText(0);     // Read the text command from the  text Channel ID = 0
     //String text2= virtuino.getText(2);  // Read the text command from the  Channel ID ID = 2
     Serial.println("Text of ID 0="+text); // Print the command on serial display
     virtuino.clearTextBuffer();           // Clear all Text Commands from buffer

     if (text.equals("0")) {
        digitalWrite(D4,0);                                // do something 
     }
     else 
       if (text.equals("1")){
          digitalWrite(D4,1);                            // do something 
          virtuino.sendText(1,"Hello Virtuino the pin is 1");   // On Virtuino app add a terminal display to Text Channel 1 to get this message
       }
   }

     
   
   //--------------------------------------------------------
   //----- example 2 how to send a Text Command to virtuino
   
   byte pinState=digitalRead(D5);       // read Pin 6 state
   if (pinState!=storedPinState) {     // check if pin value has changed
     delay(100);                       // wait 100 millis
      if (digitalRead(D5)==1) virtuino.sendText(1,"Hello Virtuino the pin D5 is 1");
      else virtuino.sendText(1,"Now the pin D5 is 0");
      storedPinState=pinState;
   }

   
  }





