#include <WiFi.h>
// #include <PubSubClient.h>
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

HardwareSerial lcdPort(1);

String message; 

unsigned long time_now;
int interval_30 = 30;
int interval_10 = 10;
unsigned long last_pub_login = 0;
String time_cur;
String date_cur;


void connectWiFi(const String& ssid, const String& password);
void processCommand(String command);
void processConfigCommand(String command);



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

    timeClient.update();

    time_now = timeClient.getEpochTime();

    if (time_now - last_pub_login >= interval_30) {
      last_pub_login = time_now;
      time_cur = getTime();
      date_cur = getDate();
      char jsonBuffer[100];
      sprintf(jsonBuffer, "{\"date\":\"%s\",\"time\":\"%s\",\"value\":\"%s\"}", date_cur, time_cur, client_id);
      publishData(&mqttClient, topic_pub[TOPIC_PING_ID], jsonBuffer);
  }

    if (Serial.available()) {
      String command = Serial.readStringUntil('\n');
      processCommand(command);
    }

    if (lcdPort.available()) {
    byte data[9];
    lcdPort.readBytes(data, 9);

      if (data[0] == 0x5A && data[1] == 0xA5 && data[3] == 0x83) {
        uint16_t address = (data[4] << 8) | data[5];
        uint16_t value = (data[7] << 8) | data[8];

        // Xử lý địa chỉ và giá trị
        Serial.println(value, HEX);
        if(value == 0){
          message = getTime() + "led off";
          publishData(&mqttClient, "espLTN/onoff", message );
        } else if(value == 1){
          message = getTime() + "led on";
          publishData(&mqttClient, "espLTN/onoff", message);
        }
      }
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



