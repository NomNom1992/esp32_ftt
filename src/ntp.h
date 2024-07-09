#ifndef NTP_H
#define NTP_H

#include <NTPClient.h>
#include <WiFiUdp.h>

// Khai báo biến toàn cục
extern WiFiUDP ntpUDP;
extern NTPClient timeClient;

void setup_NTP();
String getTime();
#endif // NTP_H
