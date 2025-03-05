#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Định nghĩa các thông số cho màn hình OLED
#define SCREEN_WIDTH 128 // Chiều rộng màn hình OLED
#define SCREEN_HEIGHT 64 // Chiều cao màn hình OLED
#define OLED_RESET -1    // Chân reset, không cần dùng với ESP8266
int val=0;
// Khai báo đối tượng màn hình OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Cấu trúc dữ liệu nhận
typedef struct struct_message {
  int mappedValue;
} struct_message;
int i =0;
// Tạo một biến kiểu struct_message
struct_message incomingData;

// Hàm xử lý dữ liệu nhận được
void OnDataRecv(uint8_t *mac, uint8_t *data, uint8_t len) {
  if (len == sizeof(incomingData)) { // Kiểm tra kích thước dữ liệu nhận được
    memcpy(&incomingData, data, sizeof(incomingData));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Mapped Value: ");
    Serial.println(incomingData.mappedValue);
   i = 0;
    // Điều khiển servo theo giá trị nhận được
    val = incomingData.mappedValue;

    
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
  // Khởi tạo màn hình OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.print("DANG KHOI DONG");
    display.display();
  display.clearDisplay();
  display.display();
  // Khởi tạo ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Đăng ký callback khi nhận được dữ liệu
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

  
}

void loop() {
  // Không làm gì trong loop, chỉ chờ nhận dữ liệu
  // Hiển thị giá trị lên màn hình OLED
  i = i +1;
  if(i >= 100){
    display.clearDisplay();
    display.setTextSize(1);             // Chọn kích thước chữ
    display.setTextColor(SSD1306_WHITE); // Chọn màu chữ
    display.setCursor(0, 20);            // Căn giữa theo chiều ngang và chiều dọc
    display.print("MAT KET NOI");
    display.display();
  }
  else{
  display.clearDisplay();
    display.setTextSize(5);             // Chọn kích thước chữ
    display.setTextColor(SSD1306_WHITE); // Chọn màu chữ
    display.setCursor(0, 20);            // Căn giữa theo chiều ngang và chiều dọc
    display.print(val);
    display.display();
  }
  delay(1);
}
