#include "epromService.h"

union FloatToBytes {
  float f;
  uint8_t bytes[4];
};

void saveWiFiCredentials(String* ssid, String* password) {
  EEPROM.writeString(0, *ssid);
  EEPROM.writeString(32, *password);
  EEPROM.commit();
}

void readWiFiCredentials(String* ssid, String* password) {
  if (EEPROM.read(0) == 255 && EEPROM.read(1) == 255) {
    *ssid = "";
    *password = "";
  } else {
    *ssid = EEPROM.readString(0);
    *password = EEPROM.readString(32);
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

void cash_setup_command(String cashString) {

  // if (cashString.startsWith("cash:")) {
    int cash[4];
    for (int i = 0; i < 4; i++) {
      cash[i] = cashString.substring(5 + i * 3, 8 + i * 3).toInt();
      // cash[i] = value / 1.0f;  // Chuyển đổi thành số thập phân
    }
    
    // Lưu mảng cash vào EEPROM
    for (int i = 0; i < 4; i++) {
      EEPROM.put(100 + i * sizeof(int), cash[i]);
    }
    EEPROM.commit();
}

void read_and_send_cash_data(HardwareSerial &lcdPort) {

  int cash[4];

  for (int i = 0; i < 4; i++) {

    EEPROM.get(100 + i * sizeof(int), cash[i]);

  }
  
  uint8_t data[8];
  FloatToBytes converter;
  
  // 4 byte đầu là 5AA50582
  data[0] = 0x5A;
  data[1] = 0xA5;
  data[2] = 0x05;
  data[3] = 0x82;
  
  for (int i = 0; i < 4; i++) {
    // 2 byte địa chỉ
    data[4] = 0x22 + i;
    data[5] = 0x00;

    Serial.println("***cash int***");

    Serial.println(cash[i]);
    // Chuyển đổi float sang IEEE 754
    converter.f = cash[i] * 1.0f;
    Serial.println("***cash float***");

    Serial.println(cash[i]);
    // 2 byte giá trị tiền (lấy 2 byte thấp của IEEE 754)
    data[6] = converter.bytes[3];
    data[7] = converter.bytes[2];
    Serial.println("***data 6***");
    Serial.println(data[6]);
    Serial.println("***data 7***");
    Serial.println(data[7]);
    lcdPort.write(data, 8);
  }
}

void time_setup_command(String cashString) {
  // if (cashString.startsWith("cash:")) {
    int time[4];
    for (int i = 0; i < 4; i++) {
      time[i] = cashString.substring(5 + i * 3, 8 + i * 3).toInt();
    }
    
    // Lưu mảng cash vào EEPROM
    for (int i = 0; i < 4; i++) {
      EEPROM.put(200 + i * sizeof(int), time[i]);
    }
    EEPROM.commit();
}


void read_and_send_time_data(HardwareSerial &lcdPort) {

  int time[4];
  for (int i = 0; i < 4; i++) {
    EEPROM.get(200 + i * sizeof(int), time[i]);
  }
  
  uint8_t data[8];
  
  // 4 byte đầu là 5AA50582
  data[0] = 0x5A;
  data[1] = 0xA5;
  data[2] = 0x05;
  data[3] = 0x82;
  
  for (int i = 0; i < 4; i++) {
    // 2 byte địa chỉ
    data[4] = 0x51;
    data[5] = 0x00 + i;
    
    // 2 byte giá trị tiền
    data[6] = time[i] >> 8;
    data[7] = time[i] & 0xFF;
    
    lcdPort.write(data, 8);
  }
}