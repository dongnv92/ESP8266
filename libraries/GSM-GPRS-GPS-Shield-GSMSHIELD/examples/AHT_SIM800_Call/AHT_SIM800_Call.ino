int _GSM_TXPIN_ = 11;
int _GSM_RXPIN_ = 10;

#include "SIM900.h"
#include <SoftwareSerial.h>
//We don't need the http functions. So we can disable the next line.
//#include "inetGSM.h"
#include "sms.h"
#include "call.h"

//GSM Shield for Arduino
//https://ahtlab.com/san-pham/kit-arduino-uno-gsm-sim800a/
//this code is based on the example of Arduino Labs.

//Simple sketch to check if an incoming call is from an authorized
//number and in this case, send to this number an SMS with the value
//of a digital input.

//We have to create the classes for SMSs and calls.
CallGSM call;
SMSGSM sms;

char number[20];
byte stat=0, callstatus;
int value=0;
int pin=1;
char value_str[5];

void setup()
{
     pinMode(pin,INPUT);
     //Serial connection.
     Serial.begin(9600);
     Serial.println("Arduino UNO GSM SIM800A Testing");
     Serial.println("Made by AHTLAB -  www.ahtlab.com"); 
     //Start configuration of shield with baudrate.
     //For http uses is raccomanded to use 4800 or slower.
     if (gsm.begin(9600))
          Serial.println("\nstatus=READY");
     else Serial.println("\nstatus=IDLE");
     call.Call("0933226630");
};

void loop()
{
     if (Serial.available())
     {
          char c = Serial.read();
          Serial.println(c);
          switch(c)
          {
               case 'c':
                    call.Call("0933226630");
                    break;

               case 'h':
                    call.HangUp();  
                    break;

               case 'a':
                    call.PickUp();  
                    break;
          } 
     }
     delay(1000);
     callstatus = call.GetCallStatus();
     Serial.print("Call status: ");
     Serial.println(callstatus);
};
