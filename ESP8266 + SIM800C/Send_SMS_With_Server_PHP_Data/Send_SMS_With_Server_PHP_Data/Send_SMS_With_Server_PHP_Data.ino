/* ======================================== */
/* SIM800 */
/* ======================================== */
int _GSM_TXPIN_ = 13;
int _GSM_RXPIN_ = 15;

#include "SIM900.h"
#include <SoftwareSerial.h>

#include "sms.h"
SMSGSM sms;

/* ======================================== */
/* WIFI */
/* ======================================== */
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <EEPROM.h>

#define HOST "api.xoidua.com"
#define PATH "/test.php"

WiFiClient client;

boolean started = false; //trạng thái modul sim 

void setup() {
  Serial.begin(9600);
  Serial.println("Arduino UNO GSM SIM800A Testing");
  Serial.println("Made by Nguyen Van Dong"); 
  //Start configuration of shield with baudrate.
  //For http uses is raccomanded to use 4800 or slower.
  if (gsm.begin(9600)) {
    Serial.println("\nstatus=READY");
    started = true;
  } else {
    Serial.println("\nstatus=IDLE");  
  }
  
  setupWiFi();
}

void loop() {
  // Request nhận yêu cầu với chu kỳ ~ 10s
  String response = requestApi();
  processResponse(response);
  delay(10000);
}

void processResponse(String response) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(response);

  const char* content = root["content"];
  const char* phone   = root["phone"];
  int id              = root["id"];
  Serial.println(content);
  Serial.println(phone);
  Serial.println(id);

  boolean smsStatus = sms.SendSMS((char*)phone, (char*)content);
  
  // Nếu gửi thành công
  if (smsStatus == 1) {
    Serial.println("Gui thanh cong");
  }else{
    Serial.println("Gui that bai");
  }
}

void setupWiFi() {
  EEPROM.begin(512);
  Serial.begin(115200);
  WiFiManager wifiManager; // Access: 192.168.4.1
  //wifiManager.resetSettings();    //Uncomment this to wipe WiFi settings from EEPROM on boot.  Comment out and recompile/upload after 1 boot cycle.
  if (!wifiManager.autoConnect("SMS GATEWAY")) {
    Serial.println("Ket noi that bai. Vui long thu lai.");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }
  //if you get here you have connected to the WiFi
  Serial.println("Ket noi den Wifi thanh cong ...");
}

String requestApi() {
  Serial.println("Dang khoi dong ket noi den Server ...");

  if (!client.connect(HOST, 80)) {
    Serial.println("Ket noi khong thanh cong ...");
    return "0";
  }
  
  client.print(String("GET ") + PATH " HTTP/1.1\r\n" + "Host: " HOST "\r\n" + "Connection: close\r\n\r\n");
  delay(10);

  String    response  = "";
  unsigned long now   = millis();

  while (millis() - now < 5000L) {
    bool currentLineIsBlank = true;
    bool clearHeader = false;
    bool avail;
    int ch_count = 0;
    
    while(client.available()) {
      avail = clearHeader;
      char c = client.read();

      if(!clearHeader){
        if (currentLineIsBlank && c == '\n') {
          clearHeader = true;
        }
      } else {
        if (ch_count < 1000)  {
          response += c;
          ch_count++;
        }
      }

      if (c == '\n') {
        currentLineIsBlank = true;
      }else if (c != '\r') {
        currentLineIsBlank = false;
      }
    }
  }
  Serial.println(response);
  return response;
}
