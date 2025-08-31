#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "esp_err.h"
#include "esp_timer.h"
#include "driver/ledc.h"
#include "driver/adc.h"
#include "esp_rom_sys.h"

// ----------------- Pins -----------------
#define ADC_PIN_CH  ADC2_CHANNEL_6   // GPIO14 (ADC2) - beware: ADC2 not available if Wi‑Fi is active
#define PWM_GPIO    25               // PWM output (LEDC)

// ----------------- Capture size -----------------
#define N 10000

// ----------------- PWM config -----------------
static const uint32_t PWM_FREQ = 100000;   // 100 kHz
static const uint8_t  PWM_RES  = 8;        // 8-bit (0..255)

// ----------------- Targets & scaling -----------------
// v_target: desired RC output voltage (in volts)
static float v_target   = 0.40f;           // volts

// Convert raw ADC (12-bit) to volts for ADC2 @ 11 dB (~0..3.3 V)
static const float V_SUPPLY   = 3.3f;                 // effective PWM high voltage
static const float ADC_SCALE  = V_SUPPLY / 4095.0f;   // raw -> volts

// Top-of-scale for logging in tab[] (0..1000)
//   - 1.10f to map 1.10 V -> 1000
//   - 3.30f to map 3.30 V -> 1000
static const float V_SCALE_TOP = 1.10f;

// ----------------- State & buffers -----------------
static int      tab[N];       // measurement scaled to 0..1000 (not duty)
static uint32_t ttab[N];      // timestamps in microseconds (like Arduino's micros())

static inline int clampi(int x, int lo, int hi) {
    return (x < lo) ? lo : (x > hi) ? hi : x;
}

// ----------------- PI controller (as in your Arduino code) -----------------
static const float To = 0.0222f;   // RC time constant (s)
static const float n  = 3.0f;      // Kp

static float integral  = 0.0f;
static float integral_ = 0.0f;

// NOTE: This mirrors your Arduino function literally.
// integral_ is never updated here; in the non-saturated branch, integral is set to integral_ (0).
// If you intended anti-windup, see the corrected variant below.
static float PI_cont(float v_tgt, float v_meas_in, float dt) {
    const float Kp = n;
    const float Ki = Kp / To;

    float e = v_tgt - v_meas_in;

    integral += e * dt;
    float PI_out_ = Kp * e + Ki * integral;

    const float PI_out_upper = 1.0f;
    const float PI_out_lower = 0.0f;

    float PI_out;
    if (PI_out_ > PI_out_upper) {
        PI_out = PI_out_upper;
    } else if (PI_out_ < PI_out_lower) {
        PI_out = PI_out_lower;
    } else {
        PI_out = PI_out_;
        // identical to your sketch:
        integral = integral_;
    }
    return PI_out;
}

/* ----------------- Optional: corrected anti-windup variant -----------------
static float PI_cont(float v_tgt, float v_meas_in, float dt) {
    const float Kp = n;
    const float Ki = Kp / To;

    const float u_min = 0.0f, u_max = 1.0f;

    float e = v_tgt - v_meas_in;
    float integral_candidate = integral + e * dt;
    float u_unsat = Kp * e + Ki * integral_candidate;

    bool pushing_high = (u_unsat > u_max) && (e > 0.0f);
    bool pushing_low  = (u_unsat < u_min) && (e < 0.0f);
    if (!pushing_high && !pushing_low) {
        integral = integral_candidate;   // commit only if not forcing further saturation
        u_unsat = Kp * e + Ki * integral;
    }

    if (u_unsat > u_max) u_unsat = u_max;
    if (u_unsat < u_min) u_unsat = u_min;
    return u_unsat;
}
----------------------------------------------------------------------------- */

void app_main(void)
{
    // ===== Wait for '#' on stdin (like Serial.read() == '#') =====
    int ch = -1;
    while ((ch = getchar()) != '#') {
        // block until '#' is received
    }

    // ===== ADC2 config (GPIO14 @ 11 dB, 12-bit) =====
    // IMPORTANT: ADC2 cannot be used while Wi‑Fi is active on ESP32.
    ESP_ERROR_CHECK(adc2_config_channel_atten(ADC_PIN_CH, ADC_ATTEN_DB_11));

    // ===== LEDC PWM config: 100 kHz, 8-bit, GPIO25 =====
    ledc_timer_config_t tcfg = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = (ledc_timer_bit_t)PWM_RES,  // 8-bit
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = PWM_FREQ,
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




    // Before your sampling loop INITIALIZE THE CAPACITOR
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

    // Wait ~5 tau (5*To = 5*22.2 ms ≈ 111 ms)
    esp_rom_delay_us(120000);  // 120 ms, blocks CPU (no RTOS tick)




    // ===== Sampling loop =====
    uint32_t t_prev = (uint32_t)esp_timer_get_time();

    for (int i = 0; i < N; i++) {
        uint32_t t_now = (uint32_t)esp_timer_get_time();

        // Measure RC output (volts)
        int raw = 0;
        ESP_ERROR_CHECK(adc2_get_raw(ADC_PIN_CH, ADC_WIDTH_BIT_12, &raw));
        float v_meas = raw * ADC_SCALE;

        // dt in seconds
        float dt = (float)(t_now - t_prev) * 1e-6f;
        t_prev = t_now;

        // Step setpoint after N/3 (same as Arduino)
        if (i > N/3) v_target = 0.3f;

        // PI control
        float PI_out = PI_cont(v_target, v_meas, dt);

        // Duty 8-bit (0..255)
        int duty = (int)lroundf(PI_out * 255.0f);
        duty = clampi(duty, 0, 255);

        // Apply PWM
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

        // LOGGING: store measurement scaled to 0..1000
        float scaled = (V_SCALE_TOP > 0.0f) ? (v_meas / V_SCALE_TOP) : 0.0f;
        int meas1000 = (int)lroundf(scaled * 1000.0f);
        tab[i]  = clampi(meas1000, 0, 1000);
        ttab[i] = t_now;  // like Arduino's micros()

        // pace to ~50 µs like your loop: while(micros() < t_now + 50);
        uint32_t t_target = t_now + 50U;
        while ((uint32_t)esp_timer_get_time() < t_target) {
            // busy wait
        }
    }

    // Stop PWM and print results
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
    printf("##START\r\n");
    for (int i = 0; i < N; i++) {
        // match Arduino print format: "timestamp \t value"
        printf("%u\t%d\r\n", (unsigned)ttab[i], tab[i]);
    }
    printf("END\r\n");

    // Halt
    while (1) { /* nothing */ }
}
