#include <WiFi.h>
#include "updateFirmware.h"
#include "mqttService.h"
#include "epromService.h"
#include "ntp.h"
#include "global.h"

String currentVersion = "1";
bool isWiFiConfigured = false;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
String ssid, password;

SystemConfig SystemManager;

HardwareSerial lcdPort(1);

String message;


void connectWiFi(const String& ssid, const String& password);
void processCommand(String command);
void processConfigCommand(String command);
// void cash_setup_command(String command);


void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  lcdPort.begin(115200, SERIAL_8N1, 18, 17);

  // Đọc thông tin WiFi từ EEPROM
  readWiFiCredentials(&ssid, &password);


  if (ssid.isEmpty() || password.isEmpty()) {
    Serial.println("No WiFi credentials found. Waiting for WiFi configuration...");
  } else {
    connectWiFi(ssid, password);
    isWiFiConfigured = true;

    // Lấy địa chỉ MAC và gán cho client_id
    client_id = WiFi.macAddress();
    client_id.replace(":", ""); // Loại bỏ dấu ':'
    initMQTTClient_andSubTopic(&mqttClient);

    setup_NTP();

    read_and_send_cash_data(lcdPort);
    read_and_send_time_data(lcdPort);
  }
}

void loop() {
  if (!isWiFiConfigured) {
    if (Serial.available()) {
      String command = Serial.readStringUntil('\n');
      if (command.startsWith("CONFIG_WIFI:")) {
        processConfigCommand(command);
      }
    }
  } else {
    if (!mqttConnect(&mqttClient)) {
      Serial.println("mqtt connecting...");
      reconnectMQTT(&mqttClient);
    }
    mqttLoop(&mqttClient);
    //ping login sau mỗi 30s
    mqtt_ping_each_30s(&mqttClient);

    if (Serial.available()) {
      String command = Serial.readStringUntil('\n');
      processCommand(command);
    }

    if (lcdPort.available()) {
      //gửi lệnh order lên server
      mqtt_pub_order(&mqttClient);
      
    }
  }
}

void processCommand(String command) {
  if (command.startsWith("CONFIG_WIFI:")) {
    processConfigCommand(command);
  } else if (command.startsWith("FOTA:")) {
    String link = command.substring(5);
    int separatorIndex = link.indexOf(".com");
    if (separatorIndex != -1) {
      String host = link.substring(0, separatorIndex + 4);
      String path = link.substring(separatorIndex + 4);
      checkFirmwareUpdate(&host, &path, &currentVersion);
    }
  } else if (command.startsWith("cash:")){
    cash_setup_command(command);
  } else if (command.startsWith("time:")){
    time_setup_command(command);
  }
}

void processConfigCommand(String command) {
  String ssid, password;
  extractWiFiCredentials(command, &ssid, &password);
  saveWiFiCredentials(&ssid, &password);
  Serial.println("WiFi credentials saved. Restarting...");
  ESP.restart();
}

void connectWiFi(const String& ssid, const String& password) {
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

// void cash_setup_command(String command){}



