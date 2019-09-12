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
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>          // JSON
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <EEPROM.h>

const String host         = "http://api.xoidua.com/test.php"; // Địa Chỉ URL Lấy Dữ Liệu Tin Nhắn
const int   time_reload   = 10000;  // Thời Gian Lỗi Lần Lấy Dữ Liệu.
const int   time_send_sms = 1000;   // Thời Gian Mỗi Lần Nhắn Tin Khi Server Nhiều Tin Nhắn.
boolean     started       = false;  // Trạng thái modul sim

void setup() {
  Serial.begin(9600);
  Serial.println("Arduino SIM800A Send SMS");
  Serial.println("Made by Nguyen Van Dong"); 
  if (gsm.begin(9600)) {
    Serial.println("\nstatus=READY");
    started = true;
  } else {
    Serial.println("\nstatus=IDLE");  
  }
  WifiSetup();
}

void loop() {
  String response = requestApi(host, "act=act&type=type");
  processResponse(response);
  delay(time_reload);  //Time Post Data Reload
}

// Hàm xử lý chuỗi JSON nhận về
void processResponse(String response){
  StaticJsonDocument<2000> doc;
  DeserializationError error = deserializeJson(doc, response);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  JsonArray repos = doc["list"];
  // Print the values
  Serial.println("Dang phan tich du lieu lay duoc tu server gui ve ...");
  for (JsonObject repo : repos) { 
    const int id        = repo["id"];
    const char* phone   = repo["phone"];
    const char* content = repo["content"];
    sendSMS(id, phone, content);        
    delay(time_send_sms);
  }
}

void sendSMS(int id, const char* phone, const char* content){
  if(started){
    boolean smsStatus = sms.SendSMS((char*)phone, (char*)content);
    if (smsStatus == 1) {
      Serial.println("Gui tin nhan thanh cong :)");
    }else{
      Serial.println("Gui tin nhan khong thanh cong :(");
    }  
  }else{
      Serial.println("Gui tin nhan khong thanh cong :(");
  }  
}

// Hàm Setup Wifi Access: 192.168.4.1
String WifiSetup(){
  EEPROM.begin(512);
  Serial.begin(115200);
  WiFiManager wifiManager;
  //wifiManager.resetSettings();    //Uncomment this to wipe WiFi settings from EEPROM on boot.  Comment out and recompile/upload after 1 boot cycle.
  if (!wifiManager.autoConnect("GATEWAY SMS")) {
    Serial.println("Ket noi that bai. Vui long thu lai.");
    ESP.reset();
    delay(1000);
  }
  Serial.println("Ket noi den Wifi thanh cong ...");
}

// Hàm lấy dữ liệu từ Server về
String requestApi(String host, String parameter) {
  Serial.println("Dang khoi dong ket noi den Server ...");
  HTTPClient http;    //Declare object of class HTTPClient
  String postData;
  //Post Data
  postData = parameter;
  http.begin(host);              //Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
  int httpCode = http.POST(postData);   //Send the request
  String payload = http.getString();    //Get the response payload
  http.end();  //Close connection
  return payload;
}
