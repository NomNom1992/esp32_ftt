#ifndef GLOBAL_H
#define GLOBAL_H

#define PULSE_PIN 2  // Chọn chân GPIO bạn muốn sử dụng

#include <HardwareSerial.h>
extern HardwareSerial lcdPort;


const uint8_t data_on[8] = {0x5A, 0xA5, 0x05, 0x82, 0x51, 0x00, 0x00, 0x01};
const uint8_t data_off[8] = {0x5A, 0xA5, 0x05, 0x82, 0x51, 0x00, 0x00, 0x00};
const uint8_t clear_qr[8] = {0x5A, 0xA5, 0x05, 0x82, 0x52, 0x40, 0xFF, 0xFF};
const uint8_t form_length_5[8] = {0x5A, 0xA5, 0x05, 0x82, 0x00, 0x00, 0x00, 0x00};
// địa chỉ data cash là 2200 2300 2400 2500
// địa chỉ số phút 5100 5101 5102 5103
// địa chỉ cảm ứng 5500 return code 1 2 3 4
struct SystemConfig {
    char ssid[32];
    char password[64];
    char server[64];
    uint8_t cash[4];
    uint8_t time[4];

    SystemConfig() {
        memset(ssid, 0, sizeof(ssid));
        memset(password, 0, sizeof(password));
        strcpy(server, "mqtt.thanhtoanqrcode.vn");
        for (int i = 0; i < 4; i++) {
            cash[i] = 10;
            time[i] = 10;
        }
    }
};

// extern SystemConfig config;
extern SystemConfig systemManager;


// void SystemGetValue();
#endif