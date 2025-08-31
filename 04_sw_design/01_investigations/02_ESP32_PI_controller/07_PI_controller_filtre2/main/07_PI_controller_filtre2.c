#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "esp_err.h"
#include "esp_timer.h"
#include "driver/ledc.h"
#include "driver/adc.h"
#include "esp_rom_sys.h"
#include "driver/gpio.h"
#include "esp_task_wdt.h"
/*
#include "esp_wifi.h"
#include "esp_bt.h"
*/

// ----------------- Pins -----------------
#define ADC_PIN_CH   ADC2_CHANNEL_6    // GPIO14 (ADC2)
// GPIO14 (ADC2) can be perturbed by wifi, prefere ADC1
#define PWM_GPIO     25                // PWM output (LEDC)
#define GPIO_TRIGGER      33

// ----------------- Targets & scaling -----------------
// v_target: desired RC output voltage (in volts)
static float v_target = 2.0f;         // volts

// Convert raw ADC (12-bit) to volts for ADC2 @ 11 dB (~0..3.3 V)

static const float ADC_SCALE = 3.3f / 4095.0f;       // raw -> volts





// ----------------- PI controller (as-is) -----------------
static const float To = 0.0222f;   // RC time constant (s)
static const float n  = 3.0f;      // Kp
static float integral  = 0.0f;
static float integral_ = 0.0f;
static const int Ts_us = 100;

static float PI_cont(float v_tgt, float v_meas_in) {
    // PI WITH Anti-windup logic

    const float Kp = n;
    const float Ki = Kp / To;

    float PI_out_upper = 1.0f;
    float PI_out_lower = 0.0f;
    float PI_out;
    //   this is Backward Euler (more stable than the forward)
    float e = v_tgt - v_meas_in;
    integral_ = integral + e * Ts_us*1e-6f;
    PI_out = Kp * e + Ki * integral_;
    // Backward Euler integrator uses current error:
    // I[k] = I[k−1] + Ts * e[k]
    // Forward Euler would use the previous error:
    // I[k] = I[k−1] + Ts * e[k−1]
    // Tustin would use the average of the current and previous errors:
    // I[k] = I[k−1] + Ts * (e[k] + e[k−1]) / 2

    // Clamp (preserves your original logic incl. integral_)
    if (PI_out > PI_out_upper) {
        return PI_out_upper;
    } else if (PI_out < PI_out_lower) {
        return PI_out_lower;
    } 

    integral = integral_;
    
    return PI_out;
}




volatile bool level = false;
void step_run() {
    // Measure RC output (volts)
    int raw = 0;
    // adc_width is implicit for adc2_get_raw -> 12 bits in IDF v4.x
    ESP_ERROR_CHECK(adc2_get_raw(ADC_PIN_CH, ADC_WIDTH_BIT_12, &raw));
    float v_meas = raw * ADC_SCALE;   // volts
    //v_meas = first_order_filter_1(v_meas);



    // PI control
    float u = PI_cont(v_target, v_meas);

    // Duty 8-bit
    int duty = (int)lroundf(u * 255.0f);
    if (duty < 0)   duty = 0;
    if (duty > 255) duty = 255;

    // Apply PWM
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));


    gpio_set_level(GPIO_TRIGGER, level);
    level = !level;
}

void pwm_init(){
    // ===== Configure LEDC PWM: 100 kHz, 8-bit, GPIO25 =====
    ledc_timer_config_t tcfg = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = (ledc_timer_bit_t)8, // 8-bit
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = 100000,//100kHz
        .clk_cfg          = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&tcfg));

    ledc_channel_config_t ccfg = {
        .gpio_num   = PWM_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_0,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 0,
        .hpoint     = 0,
        .flags.output_invert = 0,
    };

    ESP_ERROR_CHECK(ledc_channel_config(&ccfg));
}
// ----------------- app_main -----------------
void app_main(void)
{
    // Disable Task Watchdog globally : if we don't make this
    // the muc restart each 10s 
    #ifdef CONFIG_ESP_TASK_WDT
    esp_task_wdt_deinit();
    #endif

    /*
    // Ensure Wi‑Fi/BT are off so ADC2 remains available (ignore errors if not started)
    #if CONFIG_ESP_WIFI_ENABLED
    (void)esp_wifi_stop();
    (void)esp_wifi_deinit();
    #endif
    #if CONFIG_BT_ENABLED || CONFIG_BT_CONTROLLER_ENABLED || CONFIG_BT_BLE_ENABLED
    (void)esp_bt_controller_disable();
    (void)esp_bt_controller_mem_release(ESP_BT_MODE_BTDM);
    #endif
    */

    pwm_init();
    // ===== Configure ADC2: GPIO14, 11 dB, 12-bit =====
    // Note: ADC2 unavailable when Wi-Fi is active.
    ESP_ERROR_CHECK(adc2_config_channel_atten(ADC_PIN_CH, ADC_ATTEN_DB_11));


    

    // GPIO trigger (debug pin)
    gpio_reset_pin(GPIO_TRIGGER);
    gpio_set_direction(GPIO_TRIGGER, GPIO_MODE_OUTPUT);



    // Before your sampling loop INITIALIZE THE CAPACITOR
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

    // Wait ~5 tau (5*To = 5*22.2 ms ≈ 111 ms)
    //esp_rom_delay_us(120000);  // 120 ms, blocks CPU (no RTOS tick)
    esp_rom_delay_us(1200000);  // 120 ms, blocks CPU (no RTOS tick)


    // ===== Sampling loop =====

    while(1) {
        uint32_t t_now = esp_timer_get_time();

        step_run();

        // ~50 µs pacing (busy wait), like your sketch
        while (esp_timer_get_time() < (t_now + Ts_us)) {
            // spin
        }
    }


}
