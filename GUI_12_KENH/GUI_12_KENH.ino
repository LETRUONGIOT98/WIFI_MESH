#include <ESP8266WiFi.h>
#include <espnow.h>
#define SOUND_SENSOR_PIN A0  // Chân đầu vào cảm biến âm thanh

// Địa chỉ MAC của các ESP8266 nhận
//1: 08:3A:8D:D1:B3:06
//2: 08:3A:8D:D1:B9:E0
// 08:3A:8D:CC:DC:E4
uint8_t peer1Address[] = {0x08, 0x3A, 0x8D, 0xCC, 0xDC, 0xE4}; // Thay đổi địa chỉ MAC này với địa chỉ thực tế
//uint8_t peer2Address[] = {0x08, 0x3A, 0x8D, 0xD1, 0xB9, 0xE0}; // Thay đổi địa chỉ MAC này với địa chỉ thực tế

// Cấu trúc dữ liệu gửi
typedef struct struct_message {
  int mappedValue;
} struct_message;

// Tạo một biến kiểu struct_message
struct_message myData;

// Hàm gửi dữ liệu thành công
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(sendStatus == 0 ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // Khởi tạo Serial Monitor
  Serial.begin(115200);

  // Khởi động WiFi
  WiFi.mode(WIFI_STA);

  // Khởi tạo ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Đăng ký callback khi dữ liệu gửi đi thành công
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);

  // Thêm ESP8266 nhận 1
  esp_now_add_peer(peer1Address, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // Thêm ESP8266 nhận 2
 // esp_now_add_peer(peer2Address, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  // Đọc giá trị từ đầu vào analog
  int analogValue = analogRead(A0);

  // Ánh xạ giá trị từ 0-1023 thành 180-0
  myData.mappedValue = analogValue;

  // Gửi dữ liệu đến ESP8266 nhận 1
  esp_now_send(peer1Address, (uint8_t *) &myData, sizeof(myData));
  // Gửi dữ liệu đến ESP8266 nhận 2
  //esp_now_send(peer2Address, (uint8_t *) &myData, sizeof(myData));
  delay(300); // Gửi dữ liệu mỗi 2 giây
}
