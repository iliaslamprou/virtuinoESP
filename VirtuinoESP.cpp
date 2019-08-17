/* Virtuino general ESP web server Library ver 1.8.0
 * Created by Ilias Lamprou
 * Updated Aug 16 2019
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino
 */


//========= VirtuinoEsp8266 Class methods  

//========= Virtuino general methods  
//  void vDigitalMemoryWrite(int digitalMemoryIndex, int value)       write a value to a Virtuino digital memory   (digitalMemoryIndex=0..31, value range = 0 or 1)
//  int  vDigitalMemoryRead(int digitalMemoryIndex)                   read  the value of a Virtuino digital memory (digitalMemoryIndex=0..31, returned value range = 0 or 1)
//  void vMemoryWrite(int memoryIndex, float value);                  write a value to Virtuino memory             (memoryIndex=0..31, value range as float value)
//  float vMemoryRead(int memoryIndex);                               read a value of  Virtuino memory             (memoryIndex=0..31, returned a float value
//  int getPinValue(int pin);                                         read the value of a Pin. Usefull to read the value of a PWM pin
//  long lastCommunicationTime;                                       Stores the last communication with Virtuino time

#include "VirtuinoESP.h"




//======================================================================================== wifiRun()
//========================================================================================
//========================================================================================


 VirtuinoESP::VirtuinoESP(){}




//======================================================================================
void  VirtuinoESP::checkVirtuinoCommand(String* command){
     int pos=command->lastIndexOf(esp_COMMAND_END_CHAR);                      
  if (pos>5){                                          // check the size of command
      command->remove(pos+1);     // clear the end of command buffer
     //------------------  get command password
      String commandPassword="";
      int k=command->indexOf(esp_COMMAND_START_CHAR);
       if (k>0) {
        commandPassword = command->substring(0,k);
        command->remove(0,k);   // clear the start part of the command  buffer
       }
      responseBuffer="";
      if ((password.length()==0) || (commandPassword.equals(password))) {                       // check if password correct
           while (command->indexOf(esp_COMMAND_START_CHAR)>=0){
              int cStart=command->indexOf(esp_COMMAND_START_CHAR);
              int cEnd=command->indexOf(esp_COMMAND_END_CHAR);
              if (cEnd-cStart>5){
                String oneCommand = command->substring(cStart+1,cEnd);                               // get one command
                char commandType = getCommandType(oneCommand.charAt(0));
                  if (commandType!='E') {
                     int pin= getCommandPin(&oneCommand);
                     if (pin!=-1){
                        boolean returnInfo=false;
                        if (oneCommand.charAt(4)=='?') returnInfo=true;
                        oneCommand.remove(0,4);
                        if (oneCommand.length()<esp_TEXT_COMMAND_MAX_SIZE) executeReceivedCommand(commandType,pin,&oneCommand,returnInfo);
                        else {
                          responseBuffer+=getErrorCommand(esp_ERROR_SIZE);
                          String s="";
                          if (pin<9) s+="0";
                          s+=String(pin);
                          responseBuffer+="!T";
                          responseBuffer+=s;
                          responseBuffer+="= $";           // return an empty text if the size of text command is too big
                          
                        }
                        lastCommunicationTime=millis();
                     } else  responseBuffer +=getErrorCommand(esp_ERROR_PIN);  // response  error pin number   !E00=1$   
                  } else responseBuffer +=getErrorCommand(esp_ERROR_TYPE);  // response  error type   !E00=3$   
          
              } else{
                responseBuffer+=getErrorCommand(esp_ERROR_SIZE);  // response  error size of command   !E00=4$   
              }
              command->remove(0,cEnd+1-cStart);
           }  // while
      } else responseBuffer+=getErrorCommand(esp_ERROR_PASSWORD);     // the password is not correct
  }
  else  responseBuffer+=getErrorCommand(esp_ERROR_SIZE);         // the size of command is not correct

   
  
}


//================================================================================== urlencode
//==================================================================================
String VirtuinoESP::urlencode(String* str){
   String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str->length(); i++){
      c=str->charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
    
}

//================================================================================== h2int
//==================================================================================
unsigned char VirtuinoESP::h2int(char c){
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}
//================================================================================== urldecode
//==================================================================================
String VirtuinoESP::urldecode(String* str){
   String encodedString="";
    char c;
    char code0;
    char code1;
    for (int i =0; i < str->length(); i++){
        c=str->charAt(i);
      if (c == '+'){
        encodedString+=' ';  
      }else if (c == '%') {
        i++;
        code0=str->charAt(i);
        i++;
        code1=str->charAt(i);
        c = (h2int(code0) << 4) | h2int(code1);
        encodedString+=c;
      } else{
         encodedString+=c;  
      }
      yield();
    }
    
   return encodedString;
}


   //================================================================= getCommandType
  //This function returns the command type which was received from app
  //The second character of command string determines the command type
  // I  digital input read command    
  // Q  digital ouput write
  // D  digital memory read - write commang
  // A  analog input analog input read command
  // O  analog output write command
  // V  float memory read - write command
  // C  connect or disconect command    
  // E  error
  
  // Other characters are recognized as an error and function returns the character E
  //This is a system fuction. Don't change this code
  
  char  VirtuinoESP::getCommandType(char c){
    if (!(c== 'I' || c == 'Q' || c == 'D' ||c == 'A' ||c == 'O' || c == 'V' || c == 'C' || c == 'T')){
      c='E'; //error  command
    }
    return c;
  }

  //================================================================= getCommandPin
  //This function returns the pin number of the command string which was received from app
  //Fuction checks if pin number is correct. If not it returns -1 as error code
  //This is a system fuction. Don't change this code
 
  int  VirtuinoESP::getCommandPin(String* aCommand){
    char p1= aCommand->charAt(1);
    char p2= aCommand->charAt(2);
    String s="-1";
    if (isDigit(p1) && isDigit(p2)) {
       s="";
       s+=p1;
       s+=p2;
    }
     return s.toInt();
  }

//================================================================= getCommandValue
float VirtuinoESP::getCommandValue(String* aCommand){
     boolean er=false;
     for (int i=0;i<aCommand->length();i++){
        char c=aCommand->charAt(i);
        if (! ((c=='.') || (c=='-') || (isDigit(aCommand->charAt(i))))) er=true;
       }
      if (er==false) return aCommand->toFloat();
    
    return 0;
  }
//================================================================== getErrorCommand
String VirtuinoESP::getErrorCommand(byte code){
  String s="";
  s+=esp_COMMAND_START_CHAR;
  s+=esp_COMMAND_ERROR;
  s+=code;
  s+=esp_COMMAND_END_CHAR;
  return s;
}

//================================================================= executeCommandFromApp
  void VirtuinoESP::executeReceivedCommand(char activeCommandType, int activeCommandPin ,String* commandString, boolean returnInfo){
    //------------------------------------
   // The value activeCommandType contains command type such as Digital output, Analog output etc.
   // The value activeCommandPin contains the pin number of the command
   // The value activeCommandValue contains the value (0,1 or 2 for digital, 0-255 for analog outputs, 0-1023 for analog memory) 
   // In this void we have to check if the command is correct and then execute the command 
   // After executing the command we have to send a response to Virtuinio app
      
    String response="";//getErrorCommand(esp_ERROR_UNKNOWN); 
    String pinString="";
    int boardPin=-1;
    
    if (activeCommandPin<10) pinString = "0"+String(activeCommandPin);
    else pinString=String(activeCommandPin);
    float activeCommandValue=0;
    switch (activeCommandType) {
       case 'T':
            if ((activeCommandPin>=0) & (activeCommandPin < 100)){
                if (returnInfo) {
                  if (textRequestedHandler==NULL) return;
                  String testReply = textRequestedHandler(activeCommandPin);
                  if (testReply==NO_REPLY) return;
                  response="";
                  response =esp_COMMAND_START_CHAR;
                  response +=activeCommandType;
                  response +=pinString;
                  response +="=";
                  response += urlencode(&testReply);
                  response +=esp_COMMAND_END_CHAR;  // response 
                }
                else {
                  if (textReceivedHandler!=NULL) {
                    String decodeStr=urldecode(commandString);
                    textReceivedHandler(activeCommandPin,decodeStr);
                  }
                }
            }
          break;
      case 'I':                         
              boardPin=getBoardDigitalPin(activeCommandPin);
              if (boardPin!=-1)
              response =esp_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(digitalRead(boardPin))+esp_COMMAND_END_CHAR;  // response 
            else  response =esp_COMMAND_START_CHAR+String(activeCommandType)+pinString+"=2"+esp_COMMAND_END_CHAR;  // response 
          break;
      case 'Q': 
              boardPin=getBoardDigitalPin(activeCommandPin);
              if (boardPin!=-1){
                 if (returnInfo) response =esp_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(digitalRead(boardPin))+esp_COMMAND_END_CHAR;  // response 
                 else{ 
                   activeCommandValue = getCommandValue(commandString);
                   if ((activeCommandValue == 0) || (activeCommandValue == 1)) {
                          digitalWrite(boardPin,activeCommandValue);
                          arduinoPinsValue[activeCommandPin]=activeCommandValue;
                          response =esp_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(digitalRead(boardPin))+esp_COMMAND_END_CHAR;  // response 
                   } else    response =getErrorCommand(esp_ERROR_VALUE);
                   }
            }  else  response =esp_COMMAND_START_CHAR+String(activeCommandType)+pinString+"=0"+esp_COMMAND_END_CHAR;  // response 
          break; 

       case 'D':
            if ((activeCommandPin>=0) & (activeCommandPin<esp_virtualDigitalMemorySize)){ 
                if (returnInfo) response =esp_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(virtualDigitalMemory[activeCommandPin])+esp_COMMAND_END_CHAR;  // response 
                else{
                      activeCommandValue = getCommandValue(commandString);
                      virtualDigitalMemory[activeCommandPin]= activeCommandValue; 
                      response =esp_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(virtualDigitalMemory[activeCommandPin])+esp_COMMAND_END_CHAR;  // response 
                }
            } else   response =getErrorCommand(esp_ERROR_PIN);  // response  error pin number   !E00=1$   
            break; 
      
       case 'V': 
           if ((activeCommandPin>=0) & (activeCommandPin<esp_virtualAnalogMemorySize)){
               if (returnInfo) response =esp_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(virtualFloatMemory[activeCommandPin])+esp_COMMAND_END_CHAR;  // response
               else { 
                    activeCommandValue = getCommandValue(commandString);
                    virtualFloatMemory[activeCommandPin]= activeCommandValue; 
                    response =esp_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(virtualFloatMemory[activeCommandPin])+esp_COMMAND_END_CHAR;  // response
                    } 
           } else   response =getErrorCommand(esp_ERROR_PIN);
          break;
       case 'O': 
             boardPin=getBoardDigitalPin(activeCommandPin);
              if (boardPin!=-1){
                  #ifdef ARDUINO_ARCH_ESP32
                    returnInfo=false;
                  #endif
                   if (returnInfo) {
                      int pwm_value = pulseIn(boardPin, HIGH);
                      pwm_value = pwm_value /7.85;
                      response =esp_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(pwm_value)+esp_COMMAND_END_CHAR;  // response 
                  }
                   else {
                        activeCommandValue = getCommandValue(commandString); 
                        arduinoPinsValue[activeCommandPin]=(int) activeCommandValue;
                        #ifndef ARDUINO_ARCH_ESP32 
                            analogWrite(boardPin,(int)activeCommandValue); 
                        #endif
                        response =esp_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String((int)activeCommandValue)+esp_COMMAND_END_CHAR;  // response 
                   }
                   
           } else    response =esp_COMMAND_START_CHAR+String(activeCommandType)+pinString+"=0"+esp_COMMAND_END_CHAR;  // response 
          break;

         case 'A':                        
             boardPin=getBoardAnalogPin(activeCommandPin);
              if (boardPin!=-1)
              response ="!"+String(activeCommandType)+pinString+"="+String(analogRead(boardPin))+"$";  // response 
            else   response =getErrorCommand(esp_ERROR_PIN);  // response  error pin number   !E00=1$       
          break;  
         
           case 'C':                         
                      activeCommandValue = getCommandValue(commandString); 
                      if (activeCommandValue==1) response =esp_firmwareCode;     // return firmware version
                      break;  
           }
       responseBuffer += response;
  }

//=================================================================== getBoardDigitalPin
int  VirtuinoESP::getBoardDigitalPin(int pinIndex){
  if (pinIndex<boardPinsCount) return digitalPinsMap[pinIndex];
  else return -1;                                         // error: pin number is not correct
}
//=================================================================== getBoardAnalogPin
int  VirtuinoESP::getBoardAnalogPin(int analogPinIndex){
  if (analogPinIndex<analogInputPinsCount) return analogInputPinsMap[analogPinIndex];
  else return -1;                                         // error: pin number is not correct
}

//=================================================================== getPinValue
int VirtuinoESP::getPinValue(int pin){
  if (pin>=0 && pin<boardPinsCount) return arduinoPinsValue[pin];
  else return 0;
  }
  //=================================================================== vmDigitalWrite
 void VirtuinoESP::vDigitalMemoryWrite(int digitalMemoryIndex, int value){
  if ((digitalMemoryIndex>=0) && (digitalMemoryIndex<esp_virtualDigitalMemorySize)){
    virtualDigitalMemory[digitalMemoryIndex]=value;
  }
}
//=================================================================== vmDigitalRead
 int VirtuinoESP::vDigitalMemoryRead(int digitalMemoryIndex){
  if ((digitalMemoryIndex>=0) && (digitalMemoryIndex<esp_virtualDigitalMemorySize)){
    return virtualDigitalMemory[digitalMemoryIndex];
  }
  else return 0;    // error
}

//=================================================================== vmAnalogWrite
void VirtuinoESP::vMemoryWrite(int memoryIndex, float value){
  if ((memoryIndex>=0) && (memoryIndex<esp_virtualAnalogMemorySize)){
      virtualFloatMemory[memoryIndex]=value;
  }
}

//=================================================================== vmAnalogRead
 float VirtuinoESP::vMemoryRead(int memoryIndex){
  if ((memoryIndex>=0) && (memoryIndex<esp_virtualAnalogMemorySize)){
    return virtualFloatMemory[memoryIndex];
  }
  else return 0;    // error
}
