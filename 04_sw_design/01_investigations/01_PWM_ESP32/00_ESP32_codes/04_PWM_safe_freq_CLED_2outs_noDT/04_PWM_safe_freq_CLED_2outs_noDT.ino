#include "driver/ledc.h"

#define PWM_A_GPIO 13  // GPIO for PWM A (high side)
#define PWM_B_GPIO 12  // GPIO for PWM B (low side)
#define GPIO_TRIGGER 11

const uint32_t pwm_freq = 100000; // 100 kHz
const uint32_t resolution_bits = 8; //8 bits max freq = 312.5 kHz , 9bits ==> 156kHz
const uint32_t max_duty = (1 << resolution_bits);  // 256
const uint32_t duty = max_duty / 2; // 128 = 50%

void setup() {
  pinMode(GPIO_TRIGGER, OUTPUT);

  // Timer configuration
  ledc_timer_config_t timer = {
    .speed_mode       = LEDC_LOW_SPEED_MODE,
    .duty_resolution  = LEDC_TIMER_8_BIT,
    .timer_num        = LEDC_TIMER_0,
    .freq_hz          = pwm_freq,
    .clk_cfg          = LEDC_USE_APB_CLK      // Force use of APB clock (80 MHz)
  };
  ledc_timer_config(&timer);

  // PWM A - starts at hpoint=0
  ledc_channel_config_t pwm_a = {
    .gpio_num   = PWM_A_GPIO,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel    = LEDC_CHANNEL_0,
    .timer_sel  = LEDC_TIMER_0,
    .duty       = duty,
    .hpoint     = 0
  };
  ledc_channel_config(&pwm_a);

  // PWM B - starts at hpoint = duty (half-period delay = 180°)
  ledc_channel_config_t pwm_b = {
    .gpio_num   = PWM_B_GPIO,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel    = LEDC_CHANNEL_1,
    .timer_sel  = LEDC_TIMER_0,
    .duty       = duty,
    .hpoint     = duty  // Phase shifted by 180°
  };
  ledc_channel_config(&pwm_b);
}

void loop() {
  // Toggle frequency (optional)
  ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, 200000);
  digitalWrite(GPIO_TRIGGER, HIGH);
  delayMicroseconds(135);

  ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, 100000);
  digitalWrite(GPIO_TRIGGER, LOW);
  delayMicroseconds(119);
}
// see https://www.hackster.io/daniel232/esp32-generate-two-180-phase-shifted-pwms-c1010f
