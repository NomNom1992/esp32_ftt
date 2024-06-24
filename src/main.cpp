#include <WiFi.h>
#include <PubSubClient.h>
#include "updateFirmware.h"
#include "mqttService.h"
#include "epromService.h"
#include <HardwareSerial.h>
#include "global.h"

const char* TOPIC_TEMP = "espLTN/temperature";





String currentVersion = "1";
bool isWiFiConfigured = false;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
String ssid, password;

HardwareSerial lcdPort(1);

void connectWiFi(const String& ssid, const String& password);
void processCommand(String command);
void processConfigCommand(String command);



void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  lcdPort.begin(115200, SERIAL_8N1, 18, 17);
  // lcdPort.println("Hello");

  // Đọc thông tin WiFi từ EEPROM
  readWiFiCredentials(&ssid, &password);

  if (ssid.isEmpty() || password.isEmpty()) {
    Serial.println("No WiFi credentials found. Waiting for WiFi configuration...");
  } else {
    connectWiFi(ssid, password);
    isWiFiConfigured = true;
    initMQTTClient(&mqttClient);
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
      Serial.println("mqtt...");
      // lcdPort.println("mqtt...");
      reconnectMQTT(&mqttClient);
    }
    mqttLoop(&mqttClient);

    if (Serial.available()) {
      String command = Serial.readStringUntil('\n');
      processCommand(command);
    }

    if (lcdPort.available() >= 8) {
    byte data[9];
    lcdPort.readBytes(data, 9);

      if (data[0] == 0x5A && data[1] == 0xA5 && data[3] == 0x83) {
        uint16_t address = (data[4] << 8) | data[5];
        uint16_t value = (data[7] << 8) | data[8];

        // Xử lý địa chỉ và giá trị
        Serial.print("Address: ");
        Serial.println(address, HEX);
        Serial.print("Value: ");
        Serial.println(value, HEX);
        if(value == 0){
        publishData(&mqttClient, "espLTN/onoff", "led off");
        } else if(value == 1){
        publishData(&mqttClient, "espLTN/onoff", "led on");
        }
      }
  }

    float temperature = random(20, 30);
    publishData(&mqttClient, TOPIC_TEMP, String(temperature));
    // Serial.println("publish OK...");
    delay(1000);
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



