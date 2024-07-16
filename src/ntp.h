#ifndef NTP_H
#define NTP_H

#include <NTPClient.h>
#include <WiFiUdp.h>

// Khai báo biến toàn cục
extern WiFiUDP ntpUDP;
extern NTPClient timeClient;


// extern unsigned long time_now;
// extern int interval_30;
// extern unsigned long last_pub_login;


// extern String time_cur;
// extern String date_cur;

void setup_NTP();
String getTime();
String getDate();

#endif // NTP_H
