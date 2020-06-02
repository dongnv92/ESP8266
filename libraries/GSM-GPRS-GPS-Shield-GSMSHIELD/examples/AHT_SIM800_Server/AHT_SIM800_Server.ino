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

//Simple sketch to start a connection as server.

InetGSM inet;
//CallGSM call;
//SMSGSM sms;

char msg[50];
int numdata;
char inSerial[50];
int i=0;
boolean started=false;
long lasttime=millis();

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
          if (inet.attachGPRS("internet.wind", "", ""))
               Serial.println("status=ATTACHED");
          else Serial.println("status=ERROR");
          delay(1000);

          //Read IP address.
          gsm.SimpleWriteln("AT+CIFSR");
          delay(5000);
          int i=0;
          while(i<20) {
               gsm.SimpleRead();
               i++;
          }

          //TCP Server. Start the socket connection
          //as server on the assigned port.
          Serial.println(msg);
          delay(5000);
          if (inet.connectTCPServer(80))
               Serial.println("status=TCPSERVERWAIT");
          else Serial.println("ERROR in Server");
          lasttime=millis();
     }
};


void loop()
{
     if(started) {
          //Check if there is an active connection.
          if (inet.connectedClient()) {
               //Read and print the last message received.
               gsm.read(msg, 50);
               Serial.println(msg);
          }
     } else {
          serialhwread();
          serialswread();
     }
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
          }
          //Read last message saved.
          if(!strcmp(inSerial,"MSG")) {
               Serial.println(msg);
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
