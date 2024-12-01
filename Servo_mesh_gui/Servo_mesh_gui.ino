#include <ESP8266WiFi.h>
#include <espnow.h> 
// Địa chỉ MAC của các ESP8266 nhận 84:CC:A8:A4:25:0B
uint8_t peer1Address[] = {0x84, 0xCC, 0xA8, 0xA4, 0x25, 0x0B}; // Thay đổi địa chỉ MAC này với địa chỉ thực tế
//uint8_t peer2Address[] = {0xB4, 0xE6, 0x2D, 0x6A, 0x0D, 0x6E}; // Thay đổi địa chỉ MAC này với địa chỉ thực tế

// Cấu trúc dữ liệu gửi
bool tt = true;
bool tt1 = true;
uint8_t goc = 0;
typedef struct struct_message {
  uint8_t mappedValue;
} struct_message;

// Tạo một biến kiểu struct_message
struct_message myData;

// Hàm gửi dữ liệu thành công
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery Success");
    
  } else {
    Serial.println("Delivery Fail");
  }
}

void setup() {
  // Khởi tạo Serial Monitor
  Serial.begin(115200);
  pinMode(D1, OUTPUT);
  digitalWrite(D1, LOW);
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
  //esp_now_add_peer(peer2Address, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  tt = true;

}

void loop() {
    int val = analogRead(A0);
    
        goc = map(val,0,480,90,0);
        if(goc <= 0) goc =0;
        if(goc >= 90) goc = 90;
        Serial.println(goc);
     if(val <= 480){
      digitalWrite(D1, HIGH);
      Serial.println("DANG GUI");
     }
     else digitalWrite(D1, LOW);
      myData.mappedValue = goc;
      // Gửi dữ liệu đến ESP8266 nhận 1
      esp_now_send(peer1Address, (uint8_t *) &myData, sizeof(myData));
      // Gửi dữ liệu đến ESP8266 nhận 2
      //esp_now_send(peer2Address, (uint8_t *) &myData, sizeof(myData));
      delay(200);
     
}
