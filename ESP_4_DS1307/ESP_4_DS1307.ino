#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <esp_now.h>
#include <RTClib.h>
RTC_DS1307 rtc;
#include <WiFi.h>
#include <ArduinoJson.h>

#include <HardwareSerial.h>
const int RX_PIN = 16; // RX pin of ESP32
const int TX_PIN = 17; // TX pin of ESP32
const int BAUD_RATE = 9600; // Baud rate for UART communication

HardwareSerial uartSerial(2); // Create a UART object using UART2

LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <FirebaseESP32.h>
// Thông tin dự án Firebase
#define DATABASE_URL "mesh-f5094-default-rtdb.firebaseio.com"
#define API_KEY "YnvqpNfrx63RsT9yYEKbJwqqE3MAQqEf4ux0Z5U6"
// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

const char* ssid = "Thu Thao";
const char* password = "12052002";
// Khởi tạo đối tượng Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

//48:E7:29:99:23:4C


// Địa chỉ MAC của ESP32 DHT11 và ESP32 khác
uint8_t broadcastAddress_1[] = {0x08, 0xD1, 0xF9, 0xE1, 0x7A, 0x38}; ///DHT11
uint8_t broadcastAddress_2[] = {0x48, 0xE7, 0x29, 0x99, 0x23, 0x4C};////Siêu âm

DynamicJsonDocument doc(1024);
DynamicJsonDocument doc1(1024);
String recv_jsondata;
String send_jsondata;
String send_jsondata1;
int nhietdo;
int doam;
int khoangcach;
#define WIFI_CHANNEL 6
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
        }
        else if (memcmp(mac, broadcastAddress_2, 6) == 0) {
            khoangcach = doc["k"];
            Serial.println("DIST: ");
            Serial.println(khoangcach);
        }
    } else {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
    }
}
long times;
void setup() {
    lcd.init();
    lcd.backlight();
    Wire.begin();
    Serial.begin(115200);
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
    WiFi.mode(WIFI_STA);
   uartSerial.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN); // Initialize UART communication
   if (!rtc.begin()) {
    lcd.print("RTC not detected!");
    while (1);
  }
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

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
   /* connectToWiFi();
    // Cấu hình Firebase
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    config.signer.test_mode = true;
      Firebase.reconnectWiFi(true);
    Firebase.begin(&config, &auth);
    Firebase.setDoubleDigits(5); */
}
int val1, val2;
void loop() {
//  WiFi.disconnect();
 DateTime now = rtc.now();
    Serial.print("VAL1: ");
    Serial.print(val1);
     Serial.print("   VAL2: ");
    Serial.print(val2);
     doc.clear();
    doc["esp1"] = val1;
    send_jsondata = "";
    serializeJson(doc, send_jsondata);
    esp_now_send(broadcastAddress_1, (uint8_t *)send_jsondata.c_str(), send_jsondata.length());
    doc1.clear();
    doc1["esp2"] = val2;
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
    lcd.print("Time:           ");
    lcd.setCursor(6, 1);
    lcd.print(String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));
    delay(20);
      StaticJsonDocument<200> jsonDoc;
  jsonDoc["t"] = nhietdo; // Nhiệt độ
  jsonDoc["h"] = doam; // Độ ẩm
  jsonDoc["d"] = khoangcach; // Khoảng cách

  // Serialize JSON document thành một chuỗi
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  // Gửi chuỗi JSON qua UART
  uartSerial.println(jsonString);

  if (uartSerial.available()) {
    // Đọc dữ liệu JSON từ UART
    StaticJsonDocument<200> receivedJson;
    DeserializationError error = deserializeJson(receivedJson, uartSerial);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    // Kiểm tra xem JSON có chứa các trường như mong đợi không
    if (receivedJson.containsKey("esp1") && receivedJson.containsKey("esp2")) {
      // Lấy dữ liệu từ JSON
      val1 = receivedJson["esp1"];
      val2 = receivedJson["esp2"];
  }
  }
  /* connectToWiFi();
    if (Firebase.ready()){
    Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/DATA/Nhiet_do"), nhietdo) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/DATA/Do_am"), doam) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/DATA/Khoang_cach"), khoangcach) ? "ok" : fbdo.errorReason().c_str());
    if (Firebase.getInt(fbdo, "/DATA/ESP2")) {
            val1 = fbdo.intData();
            Serial.printf("ESP2 Value: %d\n", val1);
        }
     if (Firebase.getInt(fbdo, "/DATA/ESP2")) {
            val2 = fbdo.intData();
            Serial.printf("ESP2 Value: %d\n", val2);
        }
  }*/
}
void initESPNow() {
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
/*void connectToWiFi() {
    Serial.println("Connecting to WiFi...");
    
    // Kết nối với mạng WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password,6);

    // Đợi kết nối hoàn tất
    while (WiFi.status() != WL_CONNECTED) {
        delay(10);
    }
}*/
