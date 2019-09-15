GSM/GPRS & GPS Library for modules using SIM800/SIM900
======

For informations and support:
- https://ahtlab.com
- https://ahtlab.com/san-pham/kit-arduino-uno-gsm-sim800a/
Kit Arduino UNO + GSM SIM800A
Là phiên bản tích hợp giữa board Arduino UNO và module GSM SIM800A trên cùng một board mạch.Một sự kết hợp hoàn hảo giữa việc kết nối Arduino và module SIM. Board mạch với kích thước chuẩn của Arduino UNO, dễ sử dụng thích hợp cho các dự án IoT, điều khiển qua SMS, thoại, internet,… mà không cần quan tâm quá nhiều tới phần cứng.

- Change define board UNO or MEGA as : GSM.h

#define UNO 
//#define MEGA
- With define is UNO: use SoftwareSerial with pin D10, D11
- With define is MEGA: use HardwareSerial with Serial 1 pin 18, 19
