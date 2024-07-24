#include "global.h"
#include <EEPROM.h>

// void SystemGetValue(){
//     // SystemConfig config;
//     uint8_t ssidAddress = 0;
//     uint8_t passwordAddress = 32;
//     uint16_t serverAddress = 300;
//     uint8_t cashAddress = 100;
//     uint8_t timeAddress = 200;

//     // Đọc SSID
//     String ssidStr = EEPROM.readString(ssidAddress);
//     strncpy(systemManager.ssid, ssidStr.c_str(), sizeof(systemManager.ssid) - 1);
//     ssidAddress += ssidStr.length() + 1;  // +1 cho ký tự null

//     // Đọc password
//     String passwordStr = EEPROM.readString(passwordAddress);
//     strncpy(systemManager.password, passwordStr.c_str(), sizeof(systemManager.password) - 1);
//     passwordAddress += passwordStr.length() + 1;

//     // Đọc địa chỉ server
//     String serverStr = EEPROM.readString(serverAddress);
//     strncpy(systemManager.server, serverStr.c_str(), sizeof(systemManager.server) - 1);
//     serverAddress += serverStr.length() + 1;

//     // Đọc mảng cash
//     for (int i = 0; i < sizeof(systemManager.cash); i++) {
//         systemManager.cash[i] = EEPROM.read(cashAddress++);
//     }

//     // Đọc mảng time
//     for (int i = 0; i < sizeof(systemManager.time); i++) {
//         systemManager.time[i] = EEPROM.read(timeAddress++);
//     }
// }

