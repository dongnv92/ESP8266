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

const String host         = "http://smsapi.topcongty.vn/"; // Địa Chỉ URL Lấy Dữ Liệu Tin Nhắn
const String imei         = "862273048557193"; // Seri Module SIM
const String access_token = "MmJVb0ZVQVczUEhLVGl5T3pidkp4UT09"; // Access Token Client
const int   time_reload   = 10000;  // Thời Gian Lỗi Lần Lấy Dữ Liệu.
const int   time_send_sms = 1000;   // Thời Gian Mỗi Lần Nhắn Tin Khi Server Nhiều Tin Nhắn.
boolean     started       = false;  // Trạng thái modul sim

void setup() {
  Serial.begin(9600);
  Serial.println("Arduino SIM800C Send SMS");
  Serial.println("Made by Nguyen Van Dong"); 
  if (gsm.begin(9600)) {
    Serial.println("\nstatus=READY");
    started = true;
  } else {
    Serial.println("\nstatus=IDLE");  
  }
  WifiSetup();
  deleteSMS();
}

void loop() {
  String url_loop_data = "http://smsapi.topcongty.vn/client/get_sms/";
  url_loop_data += imei;
  url_loop_data += "/";
  String request_loop_data  = "token=";
  request_loop_data += access_token;
  String response    = requestApi(url_loop_data, request_loop_data); 
  // Lấy nội dung tin nhắn để nhắn tin
  processResponse(response);
  receiveSMS();
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
    Serial.print("..... GUI TIN NHAN THU ");
    Serial.print(countSms);
    Serial.println(" .....");
    const int id        = repo["sms_id"];
    const char* phone   = repo["sms_phone_receive"];
    const char* content = repo["sms_content"];

    Serial.print("ID SMS: ");
    Serial.println(id);
    Serial.print("SO DIEN THOAI: ");
    Serial.println(phone);
    Serial.print("NOI DUNG: ");
    Serial.println(content);
    
    // Gọi Hàm Gửi Tin Nhắn
    sendSMS(id, phone, content);

    Serial.print("..... GUI TIN NHAN THU ");
    Serial.print(countSms);
    Serial.println(" .....");
    
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
      String url_update_success = "http://smsapi.topcongty.vn/client/update_sms/";
      url_update_success+= id;
      url_update_success+= "/";
      
      String param_update_success = "token=";
      param_update_success += access_token;
      param_update_success += "&imei=";
      param_update_success += imei;
      param_update_success += "&status=DONE";
      
      String request_update = requestApi(url_update_success, param_update_success);
      Serial.println(request_update);
      Serial.println(message_success);
    }else{
      String url_update_success = "http://smsapi.topcongty.vn/client/update_sms/";
      url_update_success += id;
      url_update_success += "/";
      
      String param_update_success = "token=";
      param_update_success += access_token;
      param_update_success += "&imei=";
      param_update_success += imei;
      param_update_success += "&status=UNSEND";
      
      String request_update = requestApi(url_update_success, param_update_success);
      Serial.println(request_update);
      Serial.println(message_false);
    }  
  }else{
      Serial.println("Loi GMS khong gui duoc tin nhan.");
  }  
}

// Hàm xóa tất cả tin nhắn trong bộ nhớ
void deleteSMS(){
  byte position_sms;
  char *sms_receive_phone;
  char *sms_receive_content;
  Serial.println(".... DELETE ALL SMS START ...");
  position_sms = sms.IsSMSPresent(SMS_ALL);
  Serial.println("... READ SMS POSITION ...");
  if (position_sms) {
    sms.DeleteSMS(byte(position_sms));    
  } else {
    Serial.println(F("KHONG CO TIN NHAN"));
  }
  Serial.println(F("DELETE ALL SMS END"));
  Serial.println();  
}

// Hàm Gửi SMS nhận được lên Server
void receiveSMS(){
  byte position_sms;
  char sms_receive_phone[20];
  char sms_receive_content[160];
  Serial.println(".... READ SMS START ...");
  position_sms = sms.IsSMSPresent(SMS_UNREAD);
  Serial.println("... READ NEW SMS POSITION ...");
  
  if (position_sms) {
    delay(100);
    Serial.print(F("... VI TRI SMS: "));
    Serial.println(position_sms);
    if(sms.GetSMS(position_sms, sms_receive_phone, 20, sms_receive_content, 160)){
      Serial.print("... SO DIEN THOAI: ");
      Serial.println(String(sms_receive_phone));
      Serial.print("... NOI DUNG: ");
      Serial.println(String(sms_receive_content));
      String url_receive_sms = "http://smsapi.topcongty.vn/client/receive_sms/";        
      String paraReceiveSms     = "imei=";
      paraReceiveSms           += imei;
      paraReceiveSms           += "&token=";
      paraReceiveSms           += access_token;
      paraReceiveSms           += "&phone_send=";
      paraReceiveSms           += sms_receive_phone;
      paraReceiveSms           += "&content=";
      paraReceiveSms           += sms_receive_content;
      String requestReceiveSMS  = requestApi(url_receive_sms, paraReceiveSms);
      Serial.println(requestReceiveSMS);        
    } else {
      Serial.println(F("CHUA CO TIN NHAN NAO CHUA DUOC DOC"));
    }
    sms.DeleteSMS(byte(position_sms));
  } else {
    Serial.println(F("KHONG CO TIN NHAN MOI"));
  }
  Serial.println(F("READ SMS END"));
  Serial.println();
}

// Hàm Setup Wifi Access: 192.168.4.1
String WifiSetup(){
  EEPROM.begin(512);
  Serial.begin(115200);
  WiFiManager wifiManager;
  //wifiManager.resetSettings();    //Uncomment this to wipe WiFi settings from EEPROM on boot.  Comment out and recompile/upload after 1 boot cycle.
  if (!wifiManager.autoConnect("GATEWAY SMS 7193")) {
    Serial.println("Ket noi that bai. Vui long thu lai.");
    ESP.reset();
    delay(1000);
  }
  Serial.println("Ket noi den Wifi thanh cong ...");
}

// Hàm lấy dữ liệu từ Server về
String requestApi(String host, String parameter) {
  Serial.println("Dang khoi dong ket noi den Server ...");
  HTTPClient http;
  http.begin(host);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode    = http.POST(parameter);
  String payload  = http.getString();
  http.end();
  return payload;
}
