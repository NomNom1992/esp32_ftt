#include "ntp.h"

// Định nghĩa biến toàn cục
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.asia.pool.ntp.org", 25200, 60000);  // NTP server, offset in seconds, update interval


void setup_NTP() {
    timeClient.begin();
}

String getTime() {
    timeClient.update();

    unsigned long epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime((time_t *)&epochTime);

    char timeStr[7];
    sprintf(timeStr, "%02d%02d%02d",  ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

    return String(timeStr);
}

String getDate() {
    timeClient.update();

    unsigned long epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime((time_t *)&epochTime);

    char dateStr[7];
    sprintf(dateStr, "%02d%02d%02d", ptm->tm_mday, ptm->tm_mon+1, ptm->tm_year-100);

    return String(dateStr);
}