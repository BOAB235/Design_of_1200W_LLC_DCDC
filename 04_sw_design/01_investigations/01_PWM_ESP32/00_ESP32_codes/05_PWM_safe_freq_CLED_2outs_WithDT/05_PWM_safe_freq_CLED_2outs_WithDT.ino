#include "driver/ledc.h"

#define PWM_A_GPIO 13
#define PWM_B_GPIO 12
#define GPIO_TRIGGER 11


/*
High speed mode enables a glitch-free changeover of timer settings.

enum ledc_mode_t
Values:

enumerator LEDC_HIGH_SPEED_MODE
LEDC high speed speed_mode

enumerator LEDC_LOW_SPEED_MODE
LEDC low speed speed_mode

enumerator LEDC_SPEED_MODE_MAX
LEDC speed limit
*/
#ifndef LEDC_HIGH_SPEED_MODE
  #define LEDC_HIGH_SPEED_MODE ((ledc_mode_t)0)
#endif

#ifndef LEDC_LOW_SPEED_MODE
  #define LEDC_LOW_SPEED_MODE ((ledc_mode_t)1)
#endif



const uint32_t resolution_bits = 8; // max freq 314kHz, 9bits ==> 157kHz
const uint32_t pwm_freq = 100000;  // 100 kHz
const uint32_t pwm_ticks = 1 << resolution_bits;  // 256 ticks per period
const uint32_t base_duty = pwm_ticks / 2;  // 128 = 50%
const uint32_t dead_time_ticks = 2;  

void setup() {
  pinMode(GPIO_TRIGGER, OUTPUT);

  ledc_timer_config_t timer = {
    .speed_mode       = LEDC_HIGH_SPEED_MODE,//LEDC_LOW_SPEED_MODE
    .duty_resolution  = LEDC_TIMER_8_BIT,
    .timer_num        = LEDC_TIMER_0,
    .freq_hz          = pwm_freq,
    //.clk_cfg          = LEDC_AUTO_CLK
    .clk_cfg          = LEDC_USE_APB_CLK      // Force use of APB clock (80 MHz)
  };
  ledc_timer_config(&timer);

  // PWM A - High Side
  ledc_channel_config_t pwm_a = {
    .gpio_num   = PWM_A_GPIO,
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .channel    = LEDC_CHANNEL_0,
    .timer_sel  = LEDC_TIMER_0,
    .duty       = base_duty - dead_time_ticks,  // shortened duty
    .hpoint     = dead_time_ticks               // delayed rising edge
  };
  ledc_channel_config(&pwm_a);

  // PWM B - Low Side (complementary)
  ledc_channel_config_t pwm_b = {
    .gpio_num   = PWM_B_GPIO,
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .channel    = LEDC_CHANNEL_1,
    .timer_sel  = LEDC_TIMER_0,
    .duty       = base_duty - dead_time_ticks,
    .hpoint     = base_duty + dead_time_ticks   // delayed to allow dead time
  };
  ledc_channel_config(&pwm_b);
}

void loop() {
  // Toggle frequency (optional)
  ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, 200000);
  digitalWrite(GPIO_TRIGGER, HIGH);
  delayMicroseconds(135);

  ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0,100000);
  digitalWrite(GPIO_TRIGGER, LOW);
  delayMicroseconds(119);
}
/*
Osciloscpe
in 100kHz dead time = 72ns ~= 2/100k/255 = 78ns
in 200khz dead time = 38ns ~= 2/200k/255 = 39ns
*/
