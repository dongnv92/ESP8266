int _GSM_TXPIN_ = 11;
int _GSM_RXPIN_ = 10;

#include "SIM900.h"
#include <SoftwareSerial.h>
#include "inetGSM.h"
//#include "sms.h"
//#include "call.h"

//GSM Shield for Arduino
//https://ahtlab.com/san-pham/kit-arduino-uno-gsm-sim800a/
//this code is based on the example of Arduino Labs.

//Simple sketch to start a connection as client.

InetGSM inet;
//CallGSM call;
//SMSGSM sms;

String body;
int numdata;
char inSerial[50];
int i=0;
boolean started=false;

void setup()
{
     //Serial connection.
     Serial.begin(9600);
     Serial.println("Arduino UNO GSM SIM800A Testing");
     Serial.println("Made by AHTLAB -  www.ahtlab.com"); 
     //Start configuration of shield with baudrate.
     //For http uses is raccomanded to use 4800 or slower.
     if (gsm.begin(9600)) {
          Serial.println("\nstatus=READY");
          started=true;
     } else Serial.println("\nstatus=IDLE");

     if(started) {
          //GPRS attach, put in order APN, username and password.
          //If no needed auth let them blank.
          if (inet.attachGPRS("internet.wind", "", "")) //APN setup
               Serial.println("status=ATTACHED");
          else Serial.println("status=ERROR");
          delay(1000);

          //Read IP address.
          gsm.SimpleWriteln("AT+CIFSR");
          delay(5000);
          //Read until serial buffer is empty.
          gsm.WhileSimpleRead();

          body = inet.httpGET("api.ahtlab.com", 80, "/sim800"); // change URL get serve 
          //Print the results.
          Serial.println("\n---respone serve-----");
          Serial.println("\n---------------------");
          Serial.println(body);
          Serial.println("\n---------------------");
     }
};

void loop()
{
     //Read for new byte on serial hardware,
     //and write them on NewSoftSerial.
     //serialhwread();
     //Read for new byte on NewSoftSerial.
     //serialswread();
};
