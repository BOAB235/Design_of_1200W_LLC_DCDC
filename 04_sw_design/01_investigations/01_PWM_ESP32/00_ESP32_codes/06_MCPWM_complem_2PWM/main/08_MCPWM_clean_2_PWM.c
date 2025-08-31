#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/mcpwm_prelude.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define GPIO_PWM_OUT_A    13
#define GPIO_PWM_OUT_B    12
#define GPIO_TRIGGER      11

static const char *TAG = "PWM_CLEAN_SWITCH";

const int resolution_hz = 160 * 1000 * 1000; // 160 MHz resolution
const int frequency1 = 200000;  // 200 kHz
const int frequency2 = 100000;  // 100 kHz
const float duty_cycle = 50.0f; // 50%

mcpwm_timer_handle_t timer;
mcpwm_oper_handle_t oper;
mcpwm_cmpr_handle_t comparator;
mcpwm_gen_handle_t generator_a;
mcpwm_gen_handle_t generator_b;

void set_pwm_frequency(int freq_hz) {
    uint32_t period_ticks = resolution_hz / freq_hz;
    uint32_t compare_ticks = period_ticks / 2;

    ESP_ERROR_CHECK(mcpwm_timer_set_period(timer, period_ticks));
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, compare_ticks));
    // shadow updates will apply cleanly via flags on timer and comparator
}

void app_main(void)
{
    // Timer configuration
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = resolution_hz,
        .period_ticks = resolution_hz / frequency1,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .flags.update_period_on_empty = true  // clean transition
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

    // Operator
    mcpwm_operator_config_t operator_config = {
        .group_id = 0
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &oper));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(oper, timer));

    // Comparator
    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true
    };
    ESP_ERROR_CHECK(mcpwm_new_comparator(oper, &comparator_config, &comparator));

    // Generator A (main output on GPIO 13)
    mcpwm_generator_config_t gen_config_a = {
        .gen_gpio_num = GPIO_PWM_OUT_A
    };
    ESP_ERROR_CHECK(mcpwm_new_generator(oper, &gen_config_a, &generator_a));

    // Generator B (complementary output on GPIO 12)
    mcpwm_generator_config_t gen_config_b = {
        .gen_gpio_num = GPIO_PWM_OUT_B
    };
    ESP_ERROR_CHECK(mcpwm_new_generator(oper, &gen_config_b, &generator_b));

    // Set HIGH on timer empty (TEZ)
    mcpwm_gen_timer_event_action_t on_empty = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .event = MCPWM_TIMER_EVENT_EMPTY,
        .action = MCPWM_GEN_ACTION_HIGH
    };
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generator_a, on_empty));

    // Set LOW on compare match
    mcpwm_gen_compare_event_action_t on_compare = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .comparator = comparator,
        .action = MCPWM_GEN_ACTION_LOW
    };
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(generator_a, on_compare));

    // Dead time between A and B (25ns @160MHz = 4 ticks)
    mcpwm_dead_time_config_t deadtime_config = {
        .posedge_delay_ticks = 4,
        .negedge_delay_ticks = 4,
        .flags.invert_output = true
    };
    ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(generator_a, generator_b, &deadtime_config));

    // Start PWM
    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    // GPIO trigger (debug pin)
    gpio_reset_pin(GPIO_TRIGGER);
    gpio_set_direction(GPIO_TRIGGER, GPIO_MODE_OUTPUT);

    // Main loop: switch between frequencies
    while (1) {
        set_pwm_frequency(frequency1); // 200 kHz
        gpio_set_level(GPIO_TRIGGER, 1);
        vTaskDelay(pdMS_TO_TICKS(139));

        set_pwm_frequency(frequency2); // 100 kHz
        gpio_set_level(GPIO_TRIGGER, 0);
        vTaskDelay(pdMS_TO_TICKS(267));
    }
}
