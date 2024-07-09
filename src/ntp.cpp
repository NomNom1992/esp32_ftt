#include "ntp.h"

// Định nghĩa biến toàn cục
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.asia.pool.ntp.org", 0, 60000);  // NTP server, offset in seconds, update interval

void setup_NTP() {
    timeClient.begin();
}

String getTime() {
    timeClient.update();
    return timeClient.getFormattedTime();
}