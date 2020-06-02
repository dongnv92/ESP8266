
int _GSM_TXPIN_ = 11;
int _GSM_RXPIN_ = 10;

#include "SIM900.h"
#include <SoftwareSerial.h>
#include "inetGSM.h"
#include "call.h"
#include "gps.h"

/* GPS ------------------*/
GPSGSM gps;
char lon[15];
char lat[15];

/* GPRS ------------------*/
InetGSM inet;
String body;
char IMEI[17];
char URL_START[78];
char URL_END_CALL[55];

/* CALL ------------------*/
CallGSM call;

/* SERIAL ------------------*/
boolean started=false;

void setup()
{
   Serial.begin(9600);
   pinMode(13, OUTPUT);
   gsmInit();
}

void loop()
{
  getGPS();
  delay(3000);
  getImei();
  delay(3000);
}

void gsmInit() {
  Serial.println("Arduino UNO GSM SIM800A Testing");
  Serial.println("Made by AHTLAB -  www.ahtlab.com"); 
  if (gsm.begin(9600)) {
    gsm.forceON();
    started=true;
  } else Serial.println("\nstatus=IDLE");
  
  if(started) {
    delay(1000);
    delay(1000);
    
    started = getGPS();
    delay(1000);

    if (started == false) {
      Serial.println("Fail to get GPS");
    } else {
      getImei();
      delay(1000);
      
    }
   }
}


boolean getGPS() {
  //GPS attach
  if (gps.attachGPS())
       Serial.println("status=GPSREADY");
  else Serial.println("status=ERROR");
  
  delay(5000);  //Time for fixing

  Serial.println("\n--------GPS----------");

  
  //Get data from GPS
  gps.getParOnlyLatLon(lon,lat);
  Serial.println(lon);
  Serial.println(lat);
  Serial.println("\n---------------------");

  return strlen(lon) > 1;
}



void getImei() {
  gsm.getIMEI(IMEI);
  Serial.println("\n--------IMEI---------");
  Serial.println(IMEI);
  Serial.println("\n---------------------");
}
