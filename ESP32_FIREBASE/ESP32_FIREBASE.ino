#include <WiFi.h>
#include <ArduinoJson.h>

#include <HardwareSerial.h>
const int RX_PIN = 16; // RX pin of ESP32
const int TX_PIN = 17; // TX pin of ESP32
const int BAUD_RATE = 9600; // Baud rate for UART communication

HardwareSerial uartSerial(2); // Create a UART object using UART2
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

int nhietdo;
int doam;
int khoangcach;
long times;
void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
   uartSerial.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN); // Initialize UART communication
   connectToWiFi();
    // Cấu hình Firebase
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    config.signer.test_mode = true;
      Firebase.reconnectWiFi(true);
    Firebase.begin(&config, &auth);
    Firebase.setDoubleDigits(5); 
}
int val1, val2;

void loop() {
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
    if (receivedJson.containsKey("t") && receivedJson.containsKey("h") && receivedJson.containsKey("d")) {
      // Lấy dữ liệu từ JSON
      nhietdo = receivedJson["t"];
      doam = receivedJson["h"];
      khoangcach = receivedJson["d"];

      // Tạo JSON response
      StaticJsonDocument<200> responseJson;
      responseJson["esp1"] = val1;
      responseJson["esp2"] = val2;

      // Serialize JSON response thành một chuỗi
      String responseString;
      serializeJson(responseJson, responseString);

      // Gửi JSON response qua UART
      uartSerial.println(responseString);
    }
  }
   if(millis() - times >= 1000){
    if (Firebase.ready()){
    Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/DATA/Nhiet_do"), nhietdo) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/DATA/Do_am"), doam) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/DATA/Khoang_cach"), khoangcach) ? "ok" : fbdo.errorReason().c_str());
    if (Firebase.getInt(fbdo, "/DATA/ESP1")) {
            val1 = fbdo.intData();
            Serial.printf("ESP2 Value: %d\n", val1);
        }
     if (Firebase.getInt(fbdo, "/DATA/ESP2")) {
            val2 = fbdo.intData();
            Serial.printf("ESP2 Value: %d\n", val2);
        }
  }
  times = millis();
   }
}

void connectToWiFi() {
    Serial.println("Connecting to WiFi...");
    
    // Kết nối với mạng WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password,6);

    // Đợi kết nối hoàn tất
    while (WiFi.status() != WL_CONNECTED) {
        delay(10);
    }
}
