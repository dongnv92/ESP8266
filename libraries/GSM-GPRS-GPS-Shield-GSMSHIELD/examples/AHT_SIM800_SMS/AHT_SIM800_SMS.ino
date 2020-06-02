int _GSM_TXPIN_ = 11;
int _GSM_RXPIN_ = 10;

#include "SIM900.h"
#include <SoftwareSerial.h>
//If not used, is better to exclude the HTTP library,
//for RAM saving.
//If your sketch reboots itself proprably you have finished,
//your memory available.
//#include "inetGSM.h"

//If you want to use the Arduino functions to manage SMS, uncomment the lines below.
#include "sms.h"
SMSGSM sms;


//GSM Shield for Arduino
//https://ahtlab.com/san-pham/kit-arduino-uno-gsm-sim800a/
//this code is based on the example of Arduino Labs.

//Simple sketch to send and receive SMS.

int numdata;
boolean started=false;
char smsbuffer[160];
char n[20];

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
          //Enable this two lines if you want to send an SMS.
          if (sms.SendSMS("+84941732379", "AHTLAB UNO SIM800 - TEST SENT SMS"))
          Serial.println("\nSMS sent OK");
     }

};

void loop()
{
     if(started) {
          //Read if there are messages on SIM card and print them.
        /**  deprecated method
        if(gsm.readSMS(smsbuffer, 160, n, 20)) {
               Serial.println(n);
               Serial.println(smsbuffer);
          }
          **/
          //get 1st sms
          sms.GetSMS(1,n,20,smsbuffer,160);
          Serial.println(n);
          Serial.println(smsbuffer);
          
          delay(1000);
     }
};
