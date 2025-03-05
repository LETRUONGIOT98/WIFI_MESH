#include <ESP8266WiFi.h>
#include <espnow.h>
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>

// Cấu trúc dữ liệu nhận
typedef struct struct_message {
  uint8_t mappedValue;
} struct_message;

struct_message incomingData;
unsigned long lastReceivedTime = 0;
const unsigned long TIMEOUT = 500; // Thời gian chờ 2s

// Chân điều khiển 
#define DFPLAYER_RX D2  // Chân RX DFPlayer
#define DFPLAYER_TX D1  // Chân TX DFPlayer

SoftwareSerial dfSerial(DFPLAYER_RX, DFPLAYER_TX);
DFPlayerMini_Fast myDFPlayer;
bool isPlaying = false;

void OnDataRecv(uint8_t *mac, uint8_t *data, uint8_t len) {
  if (len == sizeof(incomingData)) {
    memcpy(&incomingData, data, sizeof(incomingData));
    lastReceivedTime = millis();
    
    Serial.print("Giá trị nhận: ");
    Serial.println(incomingData.mappedValue);

    // Điều khiển DFPlayer
    if(incomingData.mappedValue == 1) {
      if(!isPlaying) {
        myDFPlayer.loop(1);  // Lặp lại bài 1
        isPlaying = true;
      }
    } else {
      myDFPlayer.stop();
      isPlaying = false;
    }
    
   
  }
}

void setup() {
  Serial.begin(115200);
  dfSerial.begin(9600);
  
  while(!myDFPlayer.begin(dfSerial)){
    Serial.println("DANG KET NOI LAI");
    myDFPlayer.begin(dfSerial);
    delay(500);
  }
  Serial.println("THANH CONG");
  myDFPlayer.volume(30); // Cài đặt âm lượng (0-30)
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  // Đặt địa chỉ MAC tùy chỉnh
  uint8_t customMAC[6] = {0x98, 0xF4, 0xAB, 0xD6, 0x33, 0x19};
  wifi_set_macaddr(STATION_IF, customMAC);
  
  if (esp_now_init() != 0) {
    Serial.println("Lỗi khởi tạo ESP-NOW!");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Xử lý mất kết nối
  if (millis() - lastReceivedTime > TIMEOUT) {
    if(isPlaying) {
      myDFPlayer.stop();
      isPlaying = false;
    } 
  }
}
