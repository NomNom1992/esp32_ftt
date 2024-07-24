#include "epromService.h"
#include "global.h"

#define EEPROM_SIZE 512

union FloatToBytes {
  float f;
  uint8_t bytes[4];
};

void saveWiFiCredentials(String* ssid, String* password) {
  EEPROM.writeString(450, *ssid);
  strcpy(systemManager.ssid, ssid->c_str());
  EEPROM.writeString(482, *password);
  strcpy(systemManager.password, password->c_str());
  
  EEPROM.commit();
}

void readWiFiCredentials(String* ssid, String* password) {
  if (EEPROM.read(0) == 255 && EEPROM.read(1) == 255) {
    *ssid = "";
    *password = "";
  } else {
    *ssid = EEPROM.readString(450);
    *password = EEPROM.readString(482);
  }
}

void extractWiFiCredentials(const String& command, String* ssid, String* password) {
  int separatorIndex = command.indexOf(':');
  int lastIndex = command.lastIndexOf(':');
  if (separatorIndex != -1 && lastIndex != -1 && separatorIndex != lastIndex) {
    *ssid = command.substring(separatorIndex + 1, lastIndex);
    *password = command.substring(lastIndex + 1);
  }
}

void cash_setup_command(String cashString, SystemConfig& config) {

    int cash[4];
    for (int i = 0; i < 4; i++) {
      cash[i] = cashString.substring(5 + i * 3, 8 + i * 3).toInt();
      // cash[i] = value / 1.0f;  // Chuyển đổi thành số thập phân
    }
    
    // Lưu mảng cash vào systemManager
    for (int i = 0; i < 4; i++) {
      config.cash[i] = cash[i];
    }
    send_cash_and_time_data(lcdPort, config);
}

void send_cash_and_time_data(HardwareSerial &lcdPort, SystemConfig& config) {

  uint8_t data[8];
  FloatToBytes converter;
  
  // 4 byte đầu là 5AA50582
  data[0] = 0x5A;
  data[1] = 0xA5;
  data[2] = 0x05;
  data[3] = 0x82;
  //vòng for cho data cash
  for (int i = 0; i < 4; i++) {
    // 2 byte địa chỉ
    data[4] = 0x22 + i;
    data[5] = 0x00;

    // Chuyển đổi float sang IEEE 754
    converter.f = config.cash[i] * 1.0f;
    
    // 2 byte giá trị tiền (lấy 2 byte thấp của IEEE 754)
    data[6] = converter.bytes[3];
    data[7] = converter.bytes[2];

    lcdPort.write(data, 8);
  }

  //vòng for cho data time
  for (int i = 0; i < 4; i++) {
    // 2 byte địa chỉ
    data[4] = 0x51;
    data[5] = 0x00 + i;
    
    // 2 byte giá trị tiền
    data[6] = systemManager.time[i] >> 8;
    data[7] = systemManager.time[i] & 0xFF;
    
    lcdPort.write(data, 8);
  }
}

void time_setup_command(String cashString) {
  // if (cashString.startsWith("cash:")) {
    int time[4];
    for (int i = 0; i < 4; i++) {
      time[i] = cashString.substring(5 + i * 3, 8 + i * 3).toInt();
    }
    
    // Lưu mảng time vào systemManager
    for (int i = 0; i < 4; i++) {
      systemManager.time[i] = time[i];
    }
    send_cash_and_time_data(lcdPort, systemManager);
}

void read_config_from_EEPROM(SystemConfig& config){
    EEPROM.get(0, config);
}

void save_config_to_EEPROM_and_restart(const SystemConfig& config) {
    EEPROM.put(0, config);
    EEPROM.commit();
    ESP.restart();
}