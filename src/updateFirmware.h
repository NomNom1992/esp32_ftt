#ifndef UPDATE_FIRMWARE_H
#define UPDATE_FIRMWARE_H
#define FILE_NAME "firmware.bin"
#define PORT 443

#include <HTTPClient.h>
#include <Update.h>
#include <SPIFFS.h>
#include <WiFiClientSecure.h>

void checkFirmwareUpdate(String *host, String *path, String *currentVersion);
String getLatestVersion(String *firmwareURL);
void updateFirmware(const String& firmwareURL);


void getFileFromServer(const char* host, const char* path);
void performOTAUpdateFromSPIFFS();
#endif