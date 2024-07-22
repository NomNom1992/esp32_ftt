#include "global.h"

struct SystemConfig {
    char ssid[32];        // Đủ dài cho hầu hết SSID
    char password[64];    // Đủ dài cho hầu hết mật khẩu WiFi
    char server[64];      // Địa chỉ server
    int cash[4];        // Mảng cash với 4 giá trị float
    int time[4];     // Mảng time với 4 giá trị uint32_t (giả sử thời gian được lưu dưới dạng Unix timestamp)

    // Constructor mặc định
    SystemConfig() {
        memset(ssid, 0, sizeof(ssid));
        memset(password, 0, sizeof(password));
        memset(server, 0, sizeof(server));

        for (int i = 0; i < 4; i++) {
            cash[i] = 10;
            time[i] = 10;
        }
    }
};

void SystemGetValue(){
    
}

