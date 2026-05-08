# Half-bridge PWM control with the ESP32<a class="title_class" id="title_1"></a>

## Table of contents
* [Voltage Gain – Theoretical Overview](#title_2)
* [Inputs and Specifications](#title_3)
* [Transfo ratio and Voltage Gain](#title_4)
* [Lm, Lr, Cr tank](#title_5)
* [References <a id="referencesID"></a>  ](#title_7)

## Test Setup

To reduce inductance, i use coaxial cables to connect the gpios to the oscilloscope: See below



<div style="width: 19cm; display: flex;">
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/02_setupe/setup1.jpg" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 35:</b> The setup
    </div>
  </div>
  <div style="width: 1cm;"></div>
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/02_setupe/setup2.jpg" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 36:</b> The ESP32-S3 mini
    </div>
  </div>
</div>
<br>





<div style="width: 19cm; display: flex;">
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/02_setupe/setup3.jpg" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 37:</b> ESP32 with the coaxial cables
    </div>
  </div>
  <div style="width: 1cm;"></div>
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/02_setupe/setup4.jpg" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 38:</b> The coaxial cable winding
    </div>
  </div>
</div>
<br>





<div style="width: 19cm; display: flex;">
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/02_setupe/setup5.jpg" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 39:</b> Other view of the ESP32 + cables
    </div>
  </div>
  <div style="width: 1cm;"></div>
  <div style="width: 9cm; text-align: center;">

  </div>
</div>
<br>



## The best method to generate PWM with glitch-free transitions

### The C code for PWM frequency and dead-time implementation

```c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/mcpwm_prelude.h"
#include "esp_log.h"
#include "soc/mcpwm_struct.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include <inttypes.h>
#include "hal/gpio_hal.h"
#include "soc/mcpwm_periph.h"
#include "soc/gpio_sig_map.h"   
#include "hal/gpio_types.h"
#include "esp_rom_gpio.h"       
#include "soc/system_reg.h"

#define GPIO_PWM_OUT_A    13
#define GPIO_PWM_OUT_B    12
#define GPIO_TRIGGER      11


uint32_t freq_master = 160000000; // 160 MHz

void init_mcpwm(void) {
    // I use this API initialization juste to enable the MCPWM peripheral
    // I try to do it without this API, but it does not work
    mcpwm_timer_handle_t timer;
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = freq_master,
        .period_ticks = 1600,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));
    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));

    // LOW LEVEL initialization without using the API ///////////////
    //period of PT0_clk = Period of PWM_clk * (PWM_TIMER_PRESCALE + 1)
    MCPWM0.timer[0].timer_cfg0.timer_prescale = 0 ;// 160Mhz

    //period shadow register of PWM timer, default: 255;
    MCPWM0.timer[0].timer_cfg0.timer_period = 1600 ;// 100khz 

    // USE SHADOW register: update in TEZ (begining of period )
    //0: immediate, 1: TEZ, 2:sync, 3: TEZ or sync
    MCPWM0.timer[0].timer_cfg0.timer_period_upmethod= 1; // TEZ

    MCPWM0.operators[0].gen_stmp_cfg.gen_a_upmethod = 1; // Timestamp A updates at TEZ
    MCPWM0.operators[0].gen_stmp_cfg.gen_b_upmethod = 1; // Timestamp B updates at TEZ

    MCPWM0.operators[1].gen_stmp_cfg.gen_a_upmethod = 1; // For operator 1, A at TEZ
    MCPWM0.operators[1].gen_stmp_cfg.gen_b_upmethod = 1; // For operator 1, B at TEZ

    //counter direction, 0: increment 1: decrement
    MCPWM0.timer[0].timer_status.timer_direction = 0; // Incrementing

    // GPIO13 → MCPWM Group 0, Operator 0, Generator A
    gpio_set_direction(GPIO_NUM_13, GPIO_MODE_OUTPUT);
    esp_rom_gpio_connect_out_signal(GPIO_NUM_13,
        mcpwm_periph_signals.groups[0].operators[0].generators[0].pwm_sig,
        false, false);

    // GPIO12 → MCPWM Group 0, Operator 1, Generator A
    gpio_set_direction(GPIO_NUM_12, GPIO_MODE_OUTPUT);
    esp_rom_gpio_connect_out_signal(GPIO_NUM_12,
        mcpwm_periph_signals.groups[0].operators[1].generators[0].pwm_sig,
        false, false);

    /*
    gen_utez: Action when the timer Equals Zero (TEZ, counter = 0)
    gen_utea: Action when the timer Equals Compare A (TEA, counter = compare A value)
    gen_uteb: Action when the timer Equals Compare B (TEB, counter = compare B value)

    0: No change 1: Set output LOW 2: Set output HIGH 3: Toggle output
    */

    MCPWM0.operators[0].generator[0].gen_utez = 1;  // LOW at timer=0
    MCPWM0.operators[0].generator[0].gen_utea = 2;  // HIGH at L point (300)
    MCPWM0.operators[0].generator[0].gen_uteb = 1;  // LOW at H point (400)    

    MCPWM0.operators[1].generator[0].gen_utez = 1;  // LOW at timer=0
    MCPWM0.operators[1].generator[0].gen_utea = 2;  // HIGH at L point (600)
    MCPWM0.operators[1].generator[0].gen_uteb = 1;  // LOW at H point (750)

    // Start MCPWM timer 0 in continuous mode
    MCPWM0.timer[0].timer_cfg1.timer_start = 2;
}

void init_trigger(void) {
    // configure GPIO for trigger
    gpio_reset_pin(GPIO_TRIGGER);
    gpio_set_direction(GPIO_TRIGGER, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_TRIGGER, 0); // Set initial level to LOW
}

void modify_pwm(uint32_t freq_hz , int dead_time_ticks) {
    //float delay_time_us = (int) 1000000/(freq_old*2) ; 
    int period_in, lpointA, hpointA, lpointB, hpointB;

    int period_ticks = (int)  freq_master/freq_hz ;
    // int half_period = (int)period_ticks/2;
    // int dead_time_ticks = (int) (dead_time_ns * freq_master / 1000000000.0); 
    // Convert ns to ticks

    // Update period and dead time
    lpointA = 0;
    hpointA = (int) (period_ticks - dead_time_ticks)/2;
    lpointB = hpointA + dead_time_ticks;
    hpointB = period_ticks -dead_time_ticks+1;

    while (MCPWM0.timer[0].timer_status.timer_value >= 10); // Wait for TEZ
    


    MCPWM0.timer[0].timer_cfg0.timer_period = period_ticks;
    // pwmA
    MCPWM0.operators[0].timestamp[0].gen = lpointA; // L point
    MCPWM0.operators[0].timestamp[1].gen = hpointA; // H point

    // pwmB
    MCPWM0.operators[1].timestamp[0].gen = lpointB; // L point
    MCPWM0.operators[1].timestamp[1].gen = hpointB; // H point


}

void app_main(void) {
    init_mcpwm();
    init_trigger();

    while (1) {
        modify_pwm(156000 , 4);
        gpio_set_level(GPIO_TRIGGER, 1);
        esp_rom_delay_us(100);  

        modify_pwm(73000 , 4);
        gpio_set_level(GPIO_TRIGGER, 0);
       esp_rom_delay_us(110);
    }
}
```

### Oscilloscope Measurements and Validation



<div style="width: 19cm; display: flex;">
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/01_Scope_screenshots/07_MCPWM_PTZ_all_regis/scope5.png" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 1:</b> Low frequancy 73kHz
    </div>
  </div>
  <div style="width: 1cm;"></div>
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/01_Scope_screenshots/07_MCPWM_PTZ_all_regis/scope6.png" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 2:</b> High frequancy 156kHz
    </div>
  </div>
</div>
<br>





<div style="width: 19cm; display: flex;">
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/01_Scope_screenshots/07_MCPWM_PTZ_all_regis/scope1.png" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 3:</b> Transition high to low frequencies: Freq before
    </div>
  </div>
  <div style="width: 1cm;"></div>
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/01_Scope_screenshots/07_MCPWM_PTZ_all_regis/scope2.png" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 4:</b> Transition high to low frequencies: Freq after
    </div>
  </div>
</div>
<br>





<div style="width: 19cm; display: flex;">
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/01_Scope_screenshots/07_MCPWM_PTZ_all_regis/scope3.png" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 5:</b> Transition low to high frequencies: Freq before
    </div>
  </div>
  <div style="width: 1cm;"></div>
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/01_Scope_screenshots/07_MCPWM_PTZ_all_regis/scope4.png" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 6:</b> Transition low to high frequencies: Freq after
    </div>
  </div>
</div>
<br>





<div style="width: 19cm; display: flex;">
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/01_Scope_screenshots/07_MCPWM_PTZ_all_regis/scope8.png" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 7:</b> Dead time ~25ns, in low freq, CH1 rise
    </div>
  </div>
  <div style="width: 1cm;"></div>
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/01_Scope_screenshots/07_MCPWM_PTZ_all_regis/scope9.png" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 8:</b> Dead time ~25ns, in low freq, CH1 fall
    </div>
  </div>
</div>
<br>





<div style="width: 19cm; display: flex;">
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/01_Scope_screenshots/07_MCPWM_PTZ_all_regis/scope10.png" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 9:</b> Dead time ~25ns, in the transition
    </div>
  </div>
  <div style="width: 1cm;"></div>
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/01_Scope_screenshots/07_MCPWM_PTZ_all_regis/scope11.png" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 10:</b> Dead time ~25ns, in high freq, CHA fall
    </div>
  </div>
</div>
<br>





<div style="width: 19cm; display: flex;">
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/01_Scope_screenshots/07_MCPWM_PTZ_all_regis/scope12.png" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 11:</b> Dead time ~25ns, in high freq, CHA rise
    </div>
  </div>
  <div style="width: 1cm;"></div>
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/01_Scope_screenshots/07_MCPWM_PTZ_all_regis/scope13.png" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 12:</b> zoom of the dead time
    </div>
  </div>
</div>
<br>





<div style="width: 19cm; display: flex;">
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/01_Scope_screenshots/07_MCPWM_PTZ_all_regis/scope14.png" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 13:</b> The signal overshoot of CH2
    </div>
  </div>
  <div style="width: 1cm;"></div>
  <div style="width: 9cm; text-align: center;">
    <img src="./01_PWM_ESP32/01_Scope_screenshots/07_MCPWM_PTZ_all_regis/scope15.png" style="width: 9cm;">
    <div style="width: 9cm; word-wrap: break-word;">
        <b>Figure 14:</b> overview of the overshoots
    </div>
  </div>
</div>
<br>



## Other tested methodes

# END
