#ifndef GPIO_H
#define GPIO_H

#include <Arduino.h>
#include <esp_timer.h>

#ifdef __cplusplus
extern "C" {
#endif

void IRAM_ATTR onTimer(void* arg);
void generatePulses(int n);
void generatePulses_2(int n);

#ifdef __cplusplus
}
#endif

#endif // GPIO_H
