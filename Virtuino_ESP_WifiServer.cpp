/* Virtuino Virtuino_ESP_WifiServer Library ver 1.71
 * Created by Ilias Lamprou
 * Updated Aug 11 2018
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino
 */
 
#include "Virtuino_ESP_WifiServer.h"

#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
 #else
 #include <ESP8266WiFi.h>
#endif


WiFiServer* wifiServer;

 Virtuino_ESP_WifiServer::Virtuino_ESP_WifiServer(WiFiServer* server):VirtuinoESP::VirtuinoESP(){
  wifiServer=server;
}


//============================================================== run
//==============================================================
//==============================================================
void   Virtuino_ESP_WifiServer::Virtuino_ESP_WifiServer::run(){
    WiFiClient client = wifiServer->available();   // Check if a client has connected
  if (client) {
    while(!client.available()){    // Wait until the client sends some data
      delay(1);
    }
    String req = client.readStringUntil('\r');   // Read the first line of the request
    if (DEBUG) Serial.println(req);
    

    int pos = req.indexOf("GET /");
    if (pos!=-1){                                   // We have a GET message
      req.remove(0,pos+5);         // Clear the characters GET / 
      if (DEBUG) Serial.println("Command="+req);  
      checkVirtuinoCommand(&req);    // Virtuino library checks the command 
      // Now the string responseBuffer contains the text that the library have to send to virtuino as reply to request
      String responseHttp= "HTTP/1.1 200 OK\r\n Content-Type: text/html\r\nConnection: close\r\n\r\n";
      responseHttp+=responseBuffer;
      responseHttp+="\r\n\r\n";
      if (DEBUG) Serial.println("responseHttp="+responseHttp);
      client.print(responseHttp);
      client.flush();
      responseBuffer="";
      delay(1);
      if (DEBUG)  Serial.println("Client disonnected");
      lastCommunicationTime=millis();
  }
  else {
    if (DEBUG) Serial.println("invalid request");
    client.stop();
  }
 
}
}





//====================================================================================== vDelay
//======================================================================================
void Virtuino_ESP_WifiServer::vDelay(long milliseconds){
  long timeStart=millis();
  while (millis()-timeStart<milliseconds) run();
}
