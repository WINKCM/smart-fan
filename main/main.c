#include "stdio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "lvgl.h"
#include "driver/ledc.h"

#include "lv_port_disp.h"
#include "bdc_pcnt.h"

const static char *TAG = "fan main";

#define LED_PWM_TIMER LEDC_TIMER_1
#define LED_PWM_MODE LEDC_LOW_SPEED_MODE
#define LED_PWM_DUTY_RESOLUTION LEDC_TIMER_12_BIT
#define LED_PWM_FREQUENCY (10000)

#define LED_PWM_A1 13

#define LED_PWM_A1_CHANNEL 0

lv_obj_t *fan_speed = NULL;

void fan_speed_display_init(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_color(&style, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_text_font(&style, &lv_font_montserrat_48);
    /*Create an object with the new style*/
    fan_speed = lv_label_create(lv_scr_act());
    lv_obj_add_style(fan_speed, &style, 0);
    lv_label_set_text(fan_speed, "0");
    lv_obj_center(fan_speed);
}

bool led_pwm_init(int led_channel, int gpio_number)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LED_PWM_MODE,
        .timer_num = LED_PWM_TIMER,
        .duty_resolution = LED_PWM_DUTY_RESOLUTION,
        .freq_hz = LED_PWM_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .speed_mode = LED_PWM_MODE,
        .channel = led_channel,
        .timer_sel = LED_PWM_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = gpio_number,
        .duty = 0,
        .hpoint = 0};
    ledc_channel_config(&ledc_channel);

    return true;
}

void led_set_luminance(int speed, int led_channel)
{
    speed = speed > 100 ? 100 : speed;

    int speed_duty = ((float)speed / (float)100) * 4095;

    ledc_set_duty(LED_PWM_MODE, led_channel, speed_duty);
    ledc_update_duty(LED_PWM_MODE, led_channel);
}

void app_main(void)
{
    lv_init();
    lv_port_disp_init();
    fan_speed_display_init();
    led_pwm_init(LED_PWM_A1_CHANNEL, LED_PWM_A1);
    led_set_luminance(0, LED_PWM_A1_CHANNEL);

    bdc_pcnt_t *bdc_pcnt = bdc_pcnt_create();

    int last_speed = 0;

    while (1)
    {
        vTaskDelay(30 / portTICK_PERIOD_MS);
        int now_speed = bdc_pcnt_a_get_count(bdc_pcnt);
        if (last_speed != now_speed)
        {
            ESP_LOGI(TAG, "now_speed : %d", now_speed);
            //     led_set_luminance(last_speed, LED_PWM_A1_CHANNEL);
            led_set_luminance(now_speed, LED_PWM_A1_CHANNEL);
            char now_speed_string[10];
            sprintf(now_speed_string, "%d", now_speed);
            lv_label_set_text(fan_speed, now_speed_string);
            last_speed = now_speed;
        }

        lv_task_handler();
        // ESP_LOGI(TAG, "Free heap memory: %ld bytes", esp_get_free_heap_size());
    }
}
