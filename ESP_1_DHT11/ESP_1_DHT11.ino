#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);
#include "DHT.h"
#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define led 26
#define relay 18
#include <esp_now.h>
#include <WiFi.h>
///mac PIR: 
///mac trung tâm: 08:D1:F9:E1:72:5C

uint8_t broadcastAddress_1[] = {0x08, 0xD1, 0xF9, 0xEE, 0x45, 0xCC}; //PIR
uint8_t broadcastAddress_2[] = {0x08, 0xD1, 0xF9, 0xE1, 0x72, 0x5C}; //trung tâm
String tt = "KHONG NGUOI";
#include <ArduinoJson.h>
String recv_jsondata;
String send_jsondata;
String send_jsondata1;
int pir;
int khoangcach;
//StaticJsonDocument<256> doc;  // for data < 1KB
DynamicJsonDocument doc(1024);  // for data > 1KB
DynamicJsonDocument doc1(1024);  // for data > 1KB
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
            pir = doc["p"];
            Serial.println(pir);
        } 
        if (memcmp(mac, broadcastAddress_2, 6) == 0) {
            int dk = doc["esp1"];
            digitalWrite(led, dk);
            Serial.println(dk);
        }
    } else {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
    }
}

void setup() {
  dht.begin();
  //    ONBOARD LED WILL GLOW IN CASE OF RESET 
  //    {Remove if you want}
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(relay,OUTPUT);
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  //
    WiFi.mode(WIFI_STA);
  // Hiển thị địa chỉ MAC
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
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
int set = 37; //set nhiệt độ cao
void loop() {
   byte h = dht.readHumidity();
  // Read temperature as Celsius (the default)
   byte t = dht.readTemperature();
  send_jsondata = "";               //Clearing  JSON STRING
  doc["a"] = t;                     //Creating JSON
  doc["b"] = h;
  doc1["a"] = t;                     //Creating JSON
  doc1["b"] = h;
  if(pir == 1){
    tt = "CO NGUOI";
  }
  else tt = "KHONG NGUOI";
  if(t > set) digitalWrite(relay, HIGH);
  else digitalWrite(relay, LOW);
  doc.clear();
  doc["a"] = t;
  doc["b"] = h;
  send_jsondata = "";
  serializeJson(doc, send_jsondata);
  esp_now_send(broadcastAddress_1, (uint8_t *) send_jsondata.c_str(), sizeof(send_jsondata)*send_jsondata.length());
  
 doc1.clear();
  doc1["a"] = t;
  doc1["b"] = h;
  send_jsondata1 = "";
  serializeJson(doc1, send_jsondata1);
  esp_now_send(broadcastAddress_2, (uint8_t *) send_jsondata1.c_str(), sizeof(send_jsondata)*send_jsondata1.length());
  Serial.println(send_jsondata);
  lcd.setCursor(0,0);
  lcd.print("t:    *C h:    %");
  lcd.setCursor(3,0);
  lcd.print(t);
  lcd.setCursor(12,0);
  lcd.print(h);
  lcd.setCursor(0,1);
  lcd.print("PIR:            ");
  lcd.setCursor(5,1);
  lcd.print(tt);
  delay(20);

}
