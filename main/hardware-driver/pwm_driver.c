#include "stdio.h"

#include "driver/ledc.h"

#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_12_BIT
#define LEDC_DUTY (4095)
#define LEDC_FREQUENCY (10000)

void pwm_init(void *user_data, int pwm_gpio)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = pwm_gpio,
        .duty = 0,
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void pwm_set_duty(void *user_data, int pwm_duty)
{
    int temp = LEDC_DUTY * pwm_duty / 100;
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, temp);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}
