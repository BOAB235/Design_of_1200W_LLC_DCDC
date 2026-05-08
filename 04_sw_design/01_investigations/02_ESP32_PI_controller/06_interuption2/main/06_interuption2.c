#include "driver/timer.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include <stdio.h>

volatile int64_t samples[10];
volatile int count = 0;

// Shared flags
volatile bool step_run = false;
volatile bool print_ = false;
volatile bool stop_ = false;

static inline void step_loop(void) {
    if (step_run) {
        // Bound check to avoid any race-based overflow
        if (count < 10) {
            samples[count++] = esp_timer_get_time();  // safe in task context
        }
        step_run = false;
    }
}

static inline void print_loop(void) {
    if (print_ && !stop_) {
        // Stop timer/interrupt once (task context is safe)
        timer_disable_intr(TIMER_GROUP_0, TIMER_0);
        timer_pause(TIMER_GROUP_0, TIMER_0);

        for (int i = 0; i < 10; i++) {
            printf("Sample %d: %lld µs\n", i, samples[i]);
        }

        print_ = false;  // not strictly needed now
        stop_  = true;   // mark we're done (run once)
    }
}

void IRAM_ATTR timer_isr(void *arg) {
    // Clear interrupt
    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);

    if (count < 10) {
        step_run = true;
        // Re-arm ONLY while still collecting
        timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
    } else {
        // Done: DO NOT re-enable the alarm => ISR will not fire again
        print_ = true;
        // (No timer_pause() here; not ISR-safe in legacy driver)
    }
}

void init_time_inter(void) {
    timer_config_t config = {
        .divider = 80,                 // 1 MHz => 1 tick = 1 us
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
}

void app_main(void) {
    init_time_inter();
    
    // Disable Task Watchdog in code (affects all subscribed tasks)
    #ifdef CONFIG_ESP_TASK_WDT
    esp_task_wdt_deinit();
    #endif
    vTaskDelay(pdMS_TO_TICKS(1));//1ms

    // Run until we’re told to print; avoid watchdog with a tiny yield
    while (!stop_) {
        step_loop();
        print_loop();
        //vTaskDelay(1);  // doesn't affect ISR timing (ISR preempts tasks)
    }

    // Done once: sleep forever (or vTaskDelete(NULL);)
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
