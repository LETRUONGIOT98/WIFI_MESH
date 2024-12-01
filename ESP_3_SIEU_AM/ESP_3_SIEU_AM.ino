
#define led 26
#define TRIG_PIN 27
#define ECHO_PIN 14
#define relay 18
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);
#include <esp_now.h>
#include <WiFi.h>
//08:D1:F9:E1:7A:38 esp dht11
uint8_t broadcastAddress_1[] = {0x08, 0xD1, 0xF9, 0xEE, 0x45, 0xCC}; //add peer address
uint8_t broadcastAddress_2[] = {0x08, 0xD1, 0xF9, 0xE1, 0x72, 0x5C}; //trung tâm
#include <ArduinoJson.h>
String recv_jsondata;
String send_jsondata;
String send_jsondata1;
//StaticJsonDocument<256> doc;  // for data < 1KB
DynamicJsonDocument doc(1024);  // for data > 1KB
DynamicJsonDocument doc1(1024);  // for data > 1KB
int nhietdo;
int doam;
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//recieved data will process here
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    char buff[len + 1];
    memcpy(buff, incomingData, len);
    buff[len] = '\0';
    recv_jsondata = String(buff);
    Serial.print("Received: ");
    Serial.println(recv_jsondata);
    DeserializationError error = deserializeJson(doc, recv_jsondata);

    if (!error) {
        if (memcmp(mac, broadcastAddress_1, 6) == 0) {
            nhietdo = doc["t"];
            doam = doc["h"];
            Serial.println(nhietdo);
            Serial.println(doam);
        } else if (memcmp(mac, broadcastAddress_2, 6) == 0) {
            int dk = doc["esp2"];
            digitalWrite(led, dk);
            Serial.println(dk);
        }
    } else {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
    }
}

void setup() {
  
  //    ONBOARD LED WILL GLOW IN CASE OF RESET 
  //    {Remove if you want}
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  pinMode(led,OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);  // Cài đặt chân TRIG_PIN là OUTPUT
  pinMode(ECHO_PIN, INPUT);   // Cài đặt chân ECHO_PIN là INPUT
  pinMode(relay, OUTPUT);
  //
   
  Serial.begin(115200);
  // Hiển thị địa chỉ MAC
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  WiFi.mode(WIFI_STA);
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
    esp_now_register_send_cb(OnDataSent);

    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, broadcastAddress_1, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    memcpy(peerInfo.peer_addr, broadcastAddress_2, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
}
void loop() {
  long duration;
  int distance;
  // Đảm bảo TRIG_PIN ở mức thấp để bắt đầu
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Gửi xung siêu âm bằng cách đặt TRIG_PIN ở mức cao trong 10 micro giây
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Đo độ dài xung trên chân ECHO_PIN
  duration = pulseIn(ECHO_PIN, HIGH);
  
  // Tính khoảng cách dựa trên thời gian xung
  distance = duration * 0.034 / 2;
  send_jsondata = "";               //Clearing  JSON STRING
  if(distance > 0 && distance <= 30) digitalWrite(relay, HIGH);
  else digitalWrite(relay, LOW);
  doc["k"] = distance;                     //Creating JSON
  // Gửi dữ liệu đến broadcastAddress_1
    doc.clear();
    doc["k"] = distance;
    send_jsondata = "";
    serializeJson(doc, send_jsondata);
    esp_now_send(broadcastAddress_1, (uint8_t *)send_jsondata.c_str(), send_jsondata.length());

  // Gửi dữ liệu đến broadcastAddress_2
    doc1.clear();
    doc1["k"] = distance;
    send_jsondata1 = "";
    serializeJson(doc1, send_jsondata1);
    esp_now_send(broadcastAddress_2, (uint8_t *)send_jsondata1.c_str(), send_jsondata1.length());

  lcd.setCursor(0,0);
  lcd.print("t:    *C h:    %");
  lcd.setCursor(3,0);
  lcd.print(nhietdo);
  lcd.setCursor(12,0);
  lcd.print(doam);
  lcd.setCursor(0,1);
  lcd.print("DIST:        Cm");
    lcd.setCursor(6, 1);
    lcd.print(distance);
  Serial.print("KHOANG CACH: ");
  Serial.println(distance);
  delay(20);  
}
