// This code sets up two MCPWM timers on the ESP32 to generate complementary PWM signals
// with configurable dead time and duty cycle, suitable for power electronics applications.

#include "arduino.h" // Include the Arduino core library
#include "driver/mcpwm.h" // Include the MCPWM driver for advanced PWM control
#include "soc/mcpwm_reg.h" // Include register-level definitions for MCPWM
#include "soc/mcpwm_struct.h" // Include MCPWM structure definitions

#define GPIO_PWM0A_OUT 13   // Define GPIO 13 as output for MCPWM0A (High side)==> CH1
#define GPIO_PWM0B_OUT 12   // Define GPIO 12 as output for MCPWM0B (Low side) ==> CH2
#define GPIO_TRIGGER 11     // Define GPIO 11 as test trigger ==> CH3

int frequency = 200000; // Set desired PWM frequency Hz

float dutyCycleA = 50; // Set duty cycle for Timer 0 %
//float dutyCycleB = 50; // Set duty cycle for Timer 1 %
int deadTime_ns = 20; // Set dead time nanoseconds

int resolution_Mhz = 160; // Set clock resolution to 160 MHz
int deadTime_ticks = deadTime_ns / (1000 / resolution_Mhz); // Convert dead time from ns to clock ticks

void MCPWM_SetUP() { // Function to configure MCPWM settings

  mcpwm_group_set_resolution(MCPWM_UNIT_0, 1000000 * resolution_Mhz); // Set MCPWM group resolution
  mcpwm_timer_set_resolution(MCPWM_UNIT_0, MCPWM_TIMER_0, 1000000 * resolution_Mhz); // Set resolution for Timer 0
  mcpwm_timer_set_resolution(MCPWM_UNIT_0, MCPWM_TIMER_1, 1000000 * resolution_Mhz); // Set resolution for Timer 1

  mcpwm_config_t pwm_config; // Create MCPWM config structure
  pwm_config.frequency = frequency; // Assign frequency to config
  pwm_config.cmpr_a = 0; // Initial duty cycle A set to 0%
  pwm_config.cmpr_b = 0; // Initial duty cycle B set to 0%
  pwm_config.counter_mode = MCPWM_UP_COUNTER; // Use up-counting mode
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0; // Set duty mode to active high

  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config); // Initialize Timer 0 with config
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config); // Initialize Timer 1 with config

  mcpwm_set_timer_sync_output(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_SWSYNC_SOURCE_TEZ); // Configure sync output on TEZ (timer event zero)

  mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, deadTime_ticks, deadTime_ticks); // Enable dead time for Timer 0
  mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, deadTime_ticks, deadTime_ticks); // Enable dead time for Timer 1

  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, dutyCycleA); // Set duty cycle for Timer 0 Generator A
  //mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_GEN_A, dutyCycleB); // Set duty cycle for Timer 1 Generator A

  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_PWM0A_OUT); // Assign GPIO 13 to MCPWM0A
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, GPIO_PWM0B_OUT); // Assign GPIO 12 to MCPWM0B
}

void setup() { 
  MCPWM_SetUP(); // Call MCPWM setup during startup
  pinMode(GPIO_TRIGGER , OUTPUT);
}

void loop() {
  // Set frequency to 200 kHz

mcpwm_timer_stop(MCPWM_UNIT_0, MCPWM_TIMER_0)
  mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 200000);
  mcpwm_timer_start(MCPWM_UNIT_0, MCPWM_TIMER_0);
  //mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_1, 200000);
  digitalWrite(GPIO_TRIGGER , HIGH);

  delay(1); // wait 1 ms

  // Set frequency to 100 kHz
  mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 100000);
  //mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_1, 100000);
  digitalWrite(GPIO_TRIGGER , LOW);

  delay(1); // wait 1 ms
}
