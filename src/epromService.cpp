#include "epromService.h"

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