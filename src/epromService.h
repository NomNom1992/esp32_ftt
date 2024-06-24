#ifndef EPROM_SERVICE_H
#define EPROM_SERVICE_H

#include <EEPROM.h>

void saveWiFiCredentials(String* ssid, String* password);
void readWiFiCredentials(String* ssid, String* password);
void extractWiFiCredentials(const String &command, String* ssid, String* password);

#endif