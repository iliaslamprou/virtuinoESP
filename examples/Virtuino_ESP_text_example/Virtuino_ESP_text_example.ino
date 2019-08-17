
/* Virtuino ESP8266/ESP32 web server and WiFi access point example
 * Example: How to send or receive Text Commands from Virtuino app 
 * Created by Ilias Lamprou
 * Updated Aug 17 2019
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


// 1.  On Arduino IDE software select the ESP32 or ESP8266 board
// 2.  Open the Virtuino library file: VirtuinoESP.h,  enable the line #define NODEMCU and disable the other boards
// 3.  Connect the board to usb and upload this code

#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
 #else
 #include <ESP8266WiFi.h>
#endif

#include "Virtuino_ESP_WifiServer.h"


const char* ssid = "WIFI NETWORK";            //Enter your wifi network ssid and password
const char* password = "PASSWORD";
WiFiServer server(8000);                      // Server port  8000 is the Virtuino default port


Virtuino_ESP_WifiServer virtuino(&server);


//--- Init some text variables. Thess are the variables for Virtuino T pins
String T0 ="";    
String T1 ="";
//String T2 ="";
//String T3 ="";

//============================================================== setup
//==============================================================
//==============================================================
void setup() {
   //----- Virtuino settings
  virtuino.DEBUG=true;                         // set this value TRUE to enable the serial monitor status
  virtuino.password="1234";                    // Set a password to your web server for more protection 
                                               // avoid special characters like ! $ = @ # % & * on your password. Use only numbers or text characters
 // virtuino.setTextReceivedCallback(onTextReceived);
  virtuino.setTextRequestedCallback(onTextRequested);
  
  Serial.begin(9600);                          // Enable this line only if DEBUG=true
  delay(10);

  connectESP_as_client();         // Connect the module to your wifi network
 //connectESP_as_AccessPoint();   // or setup the board as AccessPoint
  
  // ---- Start the server
  server.begin();
  Serial.println("Server started");

 //---- Enter your setup code below
  
  //pinMode(6,OUTPUT);             // On Virtuino panel add a switch to pin D4 to enable or disable the board led
  pinMode(5,INPUT);            // connect a switch.  On Virtuino panel add a Led to pin D5
 
}

//======================================================= connectESP_as_client
void connectESP_as_client(){
  //----  1. Settings as Station - Connect to a WiFi network
  Serial.println("Connecting to "+String(ssid));
   // If you don't want to config IP manually disable the next four lines
  IPAddress ip(192, 168, 1, 150);            // where 150 is the desired IP Address   
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
}

//======================================================= connectESP_as_AccessPoint
void connectESP_as_AccessPoint(){
  //----   2. Settings as Access point - Create a private Wifi Network.
   Serial.print("Setting soft-AP ... ");                   // Default IP: 192.168.4.1
   WiFi.mode(WIFI_AP);                                     // Config module as Acces point only.  Set WiFi.mode(WIFI_AP_STA); to config module as Acces point and station
   boolean result = WiFi.softAP("ESP_WiFI_Network", "12345678");      // SSID: ESP_WiFI_Network Password:12345678
   if(result == true)  Serial.println("Server Ready");
   else Serial.println("Failed!");
  
}

//============================================================== loop
//==============================================================
//==============================================================
void loop() {
  virtuino.run();

   // enter your loop code here.
   //------ avoid to use delay() function in your code

   T0="Hello Virtuino "+String(random(100));    // on Virtuino panel add a "Text Value Display" to get this text
   
   virtuino.vDelay(5000);    // use this instead of the default delay(5000)
   }


  
//=======================================================
/* This void is called by Virtuino library any time it receives a text from Virtuino app
   You have to store the text to a variable and then to do something with the text*/
void onTextReceived(uint8_t textPin, String receivedText){
   Serial.println("====Received T"+String(textPin)+"="+receivedText);
   if (textPin==0) T0=receivedText;
   else if (textPin==1) T1=receivedText;
   //else if (textPin==2) T2=receivedText;

   //else if (textPin==3) {
   //     T3=receivedText;
   //     if (T3=="TURN ON") digitalWrite(.....
   // }
}

//=======================================================
/* Every time Virtuino app requests about a text of a T pin, the library calls this function
   You have to return the string for which the app requests the text*/
String onTextRequested(uint8_t textPin){
   Serial.println("==== Requested T"+String(textPin));
   if (textPin==0) return T0;
   else if (textPin==1) return T1;
   //else if (textPin==2) return T2;
   
   return virtuino.NO_REPLY;
}
