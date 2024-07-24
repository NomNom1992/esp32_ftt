#include "gpio.h"
#include "global.h"


volatile int pulseCount = 0;
volatile bool pulseState = true;
int totalPulses = 0;
esp_timer_handle_t timer;

void IRAM_ATTR onTimer(void* arg) {
    if (pulseCount < totalPulses * 3) {  // Mỗi xung hoàn chỉnh cần 3 lần chuyển trạng thái
        digitalWrite(PULSE_PIN, pulseState);
        pulseCount++;

        // Đặt lại timer cho khoảng thời gian tiếp theo
        esp_timer_start_once(timer, 50000);  // 50ms cho mỗi trạng thái

        // Cập nhật trạng thái cho lần tiếp theo
        if (pulseCount % 3 == 0) {
            pulseState = HIGH;
        } else if (pulseCount % 3 == 1) {
            pulseState = LOW;
        } else {
            pulseState = HIGH;
        }
    } else {
        esp_timer_delete(timer);
    }
}

void generatePulses(int n) {
    totalPulses = n;
    pulseCount = 0;
    pulseState = HIGH;

    esp_timer_create_args_t timerConfig = {
        .callback = onTimer,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "pulse_timer"
    };

    esp_timer_create(&timerConfig, &timer);
    esp_timer_start_once(timer, 50000);  // Bắt đầu với 50ms HIGH
}

void generatePulses_2(int n) {
    for (int i = 0; i < n; i++) {
        digitalWrite(PULSE_PIN, HIGH);
        delay(50);  // Dương 50ms
        digitalWrite(PULSE_PIN, LOW);
        delay(50);  // Âm 50ms
        digitalWrite(PULSE_PIN, HIGH);
        delay(50); 
    }
}