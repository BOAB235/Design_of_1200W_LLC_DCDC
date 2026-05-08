#include "driver/timer.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

volatile int64_t samples[10];
volatile int count = 0;

void IRAM_ATTR timer_isr(void *arg) {
    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);

    if (count < 10) {
        samples[count++] = esp_timer_get_time();
    } else {
        timer_disable_intr(TIMER_GROUP_0, TIMER_0);
    }
}

void app_main() {
    timer_config_t config = {
        .divider = 80,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = true
    };
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 100); // 100 µs
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_isr, NULL, ESP_INTR_FLAG_IRAM, NULL);
    timer_start(TIMER_GROUP_0, TIMER_0);

    int wait_ms = 0;
    while (count < 10 && wait_ms < 1000) {
        vTaskDelay(pdMS_TO_TICKS(10));
        wait_ms += 10;
    }

    for (int i = 0; i < 10; i++) {
        printf("Sample %d: %lld µs\n", i, samples[i]);
    }
}
