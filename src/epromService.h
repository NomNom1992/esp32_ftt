#ifndef EPROM_SERVICE_H
#define EPROM_SERVICE_H

#include <EEPROM.h>
#include <global.h>

void saveWiFiCredentials(String* ssid, String* password);
void readWiFiCredentials(String* ssid, String* password);
void extractWiFiCredentials(const String &command, String* ssid, String* password);
// void cash_setup_command(const String &cashString);
void cash_setup_command(String cashString, SystemConfig& config);
void send_cash_and_time_data(HardwareSerial &lcdPort, SystemConfig& config);

void time_setup_command(String cashString);


void read_config_from_EEPROM(SystemConfig& config);
void save_config_to_EEPROM_and_restart(const SystemConfig& config);

#endif