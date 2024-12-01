#include "ESP8266WiFi.h" // Thêm thư viện ESP8266WiFi để kết nối mạng không dây

void setup()
{
  Serial.begin(115200); // Khởi động cổng Serial với tốc độ baud rate là 115200
  WiFi.mode(WIFI_STA); // Đặt chế độ làm việc của module WiFi là Station (client)
  Serial.println(" ");
  
  Serial.println(WiFi.macAddress()); // In ra Serial Monitor địa chỉ MAC của ESP8266
}

void loop()
{

}
