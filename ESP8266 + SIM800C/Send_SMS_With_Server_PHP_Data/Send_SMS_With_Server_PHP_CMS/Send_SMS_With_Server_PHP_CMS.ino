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

const String host         = "http://sms.xoidua.com/api/"; // Địa Chỉ URL Lấy Dữ Liệu Tin Nhắn
const String imei         = "862273048557193"; // Seri SIM
const int   time_reload   = 15000;  // Thời Gian Lỗi Lần Lấy Dữ Liệu.
const int   time_send_sms = 1500;   // Thời Gian Mỗi Lần Nhắn Tin Khi Server Nhiều Tin Nhắn.
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
  String request_loop_data  = "act=sms&type=not_send&imei=";
  request_loop_data += imei;
  String response    = requestApi(host, request_loop_data); 
  // Lấy nội dung tin nhắn để nhắn tin
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

  JsonArray repos = doc["data"];
  // Print the values
  Serial.println("Dang phan tich du lieu lay duoc tu server gui ve ...");
  int countSms = 0;
  for (JsonObject repo : repos) {
    countSms ++;
    // Thông Báo Đếm Tin Nhắn
    String message_start_cout_sms = "..... TIN NHAN THU ";
    message_start_cout_sms += countSms;
    message_start_cout_sms += " ......";
    
    Serial.println(message_start_cout_sms);
    const int id        = repo["sms_id"];
    const char* phone   = repo["sms_phone"];
    const char* content = repo["sms_content"];

    String sms_info_id        = "ID SMS: ";
    sms_info_id += id;
    Serial.println(sms_info_id);
    String sms_info_phone     = "SO DIEN THOAI NHAN: ";
    sms_info_phone += phone;
    Serial.println(sms_info_phone);
    String sms_info_content   = "NOI DUNG: ";
    sms_info_content += content;
    Serial.println(sms_info_content);
    
    // Gọi Hàm Gửi Tin Nhắn
    sendSMS(id, phone, content);        
    delay(time_send_sms);
  }
  if(countSms == 0){
      Serial.println("Khong Nhan Duoc Tin Nhan Nao De Gui");
  }
}

// Hàm gửi tin nhắn và update lên Server
void sendSMS(int id, const char* phone, const char* content){
  String message_intro = "DANG THUC HIEN GUI TIN NHAN ID:";
  message_intro += id;
  message_intro += " , SO DIEN THOAI: ";
  message_intro += phone;
  Serial.println(message_intro);
  if(started){
    boolean smsStatus       = sms.SendSMS((char*)phone, (char*)content);
    String message_success  = "Gui tin nhan thanh cong den so: ";
    String message_false    = "Gui tin nhan that bai den so: ";
    message_success        += phone;
    message_false          += phone;
    if (smsStatus == 1) {
      Serial.println(message_success);
      String parameter_update = "act=sms&type=update_status_sent&sms_id=";
      parameter_update += id;
      parameter_update += "&imei=";
      parameter_update += imei;
      String request_update = requestApi(host, parameter_update);
      Serial.println(request_update);
    }else{
      Serial.println(message_false);
    }  
  }else{
      Serial.println("Loi GMS khong gui duoc tin nhan.");
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
