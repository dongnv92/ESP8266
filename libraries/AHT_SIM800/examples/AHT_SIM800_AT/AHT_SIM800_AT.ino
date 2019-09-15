int _GSM_TXPIN_ = 11;
int _GSM_RXPIN_ = 10;

#include "SIM900.h"
#include <SoftwareSerial.h>
//#include "inetGSM.h"
//#include "sms.h"
//#include "call.h"

//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//https://ahtlab.com/san-pham/kit-arduino-uno-gsm-sim800a/
//this code is based on the example of Arduino Labs.

//Simple sketch to communicate with SIM900 through AT commands.

//InetGSM inet;
//CallGSM call;
//SMSGSM sms;

int numdata;
char inSerial[40];
int i=0;


void setup()
{
     //Serial connection.
     Serial.begin(9600);
     Serial.println("Arduino UNO GSM SIM800A Testing");
     Serial.println("Made by AHTLAB -  www.ahtlab.com"); 
     //Start configuration of shield with baudrate.
     //For http uses is raccomanded to use 4800 or slower.
     if (gsm.begin(9600))
          Serial.println("\nstatus=READY");
     else Serial.println("\nstatus=IDLE");
     
     Serial.println("--------------------------------"); 
     Serial.println("  Test ATCommand with Terminal  "); 
     Serial.println("AT -> OK");
     Serial.println("AT+CSQ -> Check GSM signal");
     Serial.println("ATDxxx; -> Call number xxx");
     
};

void loop()
{
     //Read for new byte on serial hardware,
     //and write them on NewSoftSerial.
     serialhwread();
     //Read for new byte on NewSoftSerial.
     serialswread();
};

void serialhwread()
{
     i=0;
     if (Serial.available() > 0) {
          while (Serial.available() > 0) {
               inSerial[i]=(Serial.read());
               delay(10);
               i++;
          }

          inSerial[i]='\0';
          if(!strcmp(inSerial,"/END")) {
               Serial.println("_");
               inSerial[0]=0x1a;
               inSerial[1]='\0';
               gsm.SimpleWriteln(inSerial);
          }
          //Send a saved AT command using serial port.
          if(!strcmp(inSerial,"TEST")) {
               Serial.println("SIGNAL QUALITY");
               gsm.SimpleWriteln("AT+CSQ");
          } else {
               Serial.println(inSerial);
               gsm.SimpleWriteln(inSerial);
          }
          inSerial[0]='\0';
     }
}

void serialswread()
{
     gsm.SimpleRead();
}
