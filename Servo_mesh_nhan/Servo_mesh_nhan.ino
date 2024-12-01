#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Servo.h>

// Cấu trúc dữ liệu nhận
typedef struct struct_message {
  uint8_t mappedValue;
} struct_message;

// Tạo một biến kiểu struct_message
struct_message incomingData;

// Khai báo đối tượng Servo
Servo myServo;

// Chân điều khiển servo
const int servoPin = D6;

// Hàm xử lý dữ liệu nhận được
void OnDataRecv(uint8_t *mac, uint8_t *data, uint8_t len) {
  if (len == sizeof(incomingData)) { // Kiểm tra kích thước dữ liệu nhận được
    memcpy(&incomingData, data, sizeof(incomingData));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Mapped Value: ");
    Serial.println(incomingData.mappedValue);

    // Điều khiển servo theo giá trị nhận được
    uint8_t servoPosition = incomingData.mappedValue;
    myServo.write(servoPosition);
  } else {
    Serial.println("Received data size mismatch");
  }
}

void setup() {
  // Khởi tạo Serial Monitor
  Serial.begin(115200);

  // Khởi động WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Khởi tạo ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Đăng ký callback khi nhận được dữ liệu
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

  // Khởi tạo servo
  myServo.attach(servoPin,500,2400);
}

void loop() {
  // Không làm gì trong loop, chỉ chờ nhận dữ liệu
}
