#include "driver/ledc.h"

#define PWM_INPUT_GPIO 13  // Connect to PWM input of Gate Driver (e.g., UCC21520)
#define GPIO_TRIGGER 11     // Define GPIO 11 as test trigger ==> CH3


int duty = 128;  // 50% duty (8-bit resolution)

void setup() {
  pinMode(GPIO_TRIGGER , OUTPUT);
  ledc_timer_config_t ledc_timer = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_8_BIT,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 100000,  // Start at 100kHz
    .clk_cfg = LEDC_AUTO_CLK
  };
  ledc_timer_config(&ledc_timer);

  ledc_channel_config_t ledc_channel = {
    .gpio_num = PWM_INPUT_GPIO,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .timer_sel = LEDC_TIMER_0,
    .duty = duty,
    .hpoint = 0
  };
  ledc_channel_config(&ledc_channel);
}

void loop() {
  // Switch to 200kHz
  ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, 200000);
  digitalWrite(GPIO_TRIGGER , HIGH);
  delayMicroseconds(135);

  // Switch back to 100kHz
  ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, 100000);
  digitalWrite(GPIO_TRIGGER , LOW);
  delayMicroseconds(119);
}
