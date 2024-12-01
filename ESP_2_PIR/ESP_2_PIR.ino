#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#define PIR_PIN 19
#define LED_PIN 26
#define Spin 5
#include <ESP32_Servo.h> 
Servo cua;

LiquidCrystal_I2C lcd(0x27, 16, 2);
///SIÊU ÂM: 0x48, 0xE7, 0x29, 0x99, 0x23, 0x4C
// Địa chỉ MAC của ESP32 DHT11 và ESP32 khác
uint8_t broadcastAddress_1[] = {0x08, 0xD1, 0xF9, 0xE1, 0x7A, 0x38};
uint8_t broadcastAddress_2[] = {0x48, 0xE7, 0x29, 0x99, 0x23, 0x4C};

DynamicJsonDocument doc(1024);
DynamicJsonDocument doc1(1024);
String recv_jsondata;
String send_jsondata;
String send_jsondata1;

int nhietdo;
int doam;
int khoangcach;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

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
            nhietdo = doc["a"];
            doam = doc["b"];
            Serial.println(nhietdo);
            Serial.println(doam);
        } else if (memcmp(mac, broadcastAddress_2, 6) == 0) {
            khoangcach = doc["k"];
            Serial.println(khoangcach);
        }
    } else {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
    }
}

void setup() {
    lcd.init();
    lcd.backlight();
    cua.attach(Spin,500,2400);
    pinMode(LED_PIN, OUTPUT);
    pinMode(PIR_PIN, INPUT);

    Serial.begin(115200);
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
    byte val = digitalRead(PIR_PIN);

    doc["p"] = val;
    doc1["t"] = nhietdo;
    doc1["h"] = doam;

    send_jsondata = "";
    serializeJson(doc, send_jsondata);
    esp_now_send(broadcastAddress_1, (uint8_t *)send_jsondata.c_str(), send_jsondata.length());
    send_jsondata1 = "";
    serializeJson(doc1, send_jsondata1);
    esp_now_send(broadcastAddress_2, (uint8_t *)send_jsondata1.c_str(), send_jsondata1.length());

    lcd.setCursor(0, 0);
    lcd.print("t:    *C h:    %");
    lcd.setCursor(3, 0);
    lcd.print(nhietdo);
    lcd.setCursor(12, 0);
    lcd.print(doam);
    lcd.setCursor(0, 1);
    lcd.print("DIST:        Cm ");
    lcd.setCursor(6, 1);
    lcd.print(khoangcach);

    if (val == 1) {
        cua.write(90);
    } else {
        cua.write(0);
    }
    delay(100);
}
