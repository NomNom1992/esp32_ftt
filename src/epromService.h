#ifndef EPROM_SERVICE_H
#define EPROM_SERVICE_H

#include <EEPROM.h>

void saveWiFiCredentials(String* ssid, String* password);
void readWiFiCredentials(String* ssid, String* password);
void extractWiFiCredentials(const String &command, String* ssid, String* password);
// void cash_setup_command(const String &cashString);
void cash_setup_command(String cashString);
void read_and_send_cash_data(HardwareSerial &lcdPort);

void time_setup_command(String cashString);
void read_and_send_time_data(HardwareSerial &lcdPort);

#endif