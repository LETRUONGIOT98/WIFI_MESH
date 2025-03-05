#include <ESP8266WiFi.h>
#include <espnow.h>

//98:F4:AB:D6:33:19
uint8_t peer1Address[] = {0x98, 0xF4, 0xAB, 0xD6, 0x33, 0x19}; // Địa chỉ MAC ESP nhận
// Cấu trúc dữ liệu để gửi qua ESP-NOW
typedef struct struct_message {
  uint8_t mappedValue;
} struct_message;

struct_message myData;
// Hàm callback khi gửi dữ liệu thành công
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Trạng thái gửi dữ liệu: ");
  Serial.println(sendStatus == 0 ? "Thành công" : "Thất bại");
}
void setup() {
  // Khởi tạo Serial và MPU6050
  Serial.begin(115200);
  // Cài đặt ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Không thể khởi tạo ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(peer1Address, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  // Gửi trạng thái qua ESP-NOW
  myData.mappedValue = 1;
  esp_now_send(peer1Address, (uint8_t *)&myData, sizeof(myData));
  delay(500); // Độ trễ 200ms
}
