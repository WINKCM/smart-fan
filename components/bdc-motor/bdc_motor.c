#include "stdio.h"

#include "esp_log.h"
#include "driver/ledc.h"

#include "bdc_motor.h"

const static char *TAG = "bdc motor";

#define LED_PWM_TIMER LEDC_TIMER_1
#define LED_PWM_MODE LEDC_LOW_SPEED_MODE
#define LED_PWM_DUTY_RESOLUTION LEDC_TIMER_12_BIT
#define LED_PWM_FREQUENCY (10000)

#define BDC_MOTOR_MAX_PWM_PUTY 4095 * 0.9
#define BDC_MOTOR_MIN_PWM_PUTY -4095 * 0.9

void led_pwm_init(int led_channel, int gpio_number)
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
}

bdc_motor_t *bdc_motor_create(int pwm_gpio_a, int pwm_gpio_a_channel, int pwm_gpio_b, int pwm_gpio_b_channel)
{
    if (pwm_gpio_a < 0 || pwm_gpio_b < 0 || pwm_gpio_a_channel < 0 || pwm_gpio_a_channel > 8 || pwm_gpio_b_channel < 0 || pwm_gpio_b_channel > 8 || pwm_gpio_a_channel == pwm_gpio_b_channel)
        return NULL;

    bdc_motor_t *bdc_motor = malloc(sizeof(bdc_motor_t));

    bdc_motor->pwm_gpio_a_channel = pwm_gpio_a_channel;
    bdc_motor->pwm_gpio_b_channel = pwm_gpio_b_channel;

    led_pwm_init(pwm_gpio_a_channel, pwm_gpio_a);
    led_pwm_init(pwm_gpio_b_channel, pwm_gpio_b);

    return bdc_motor;
}

void bdc_motor_delete(bdc_motor_t *bdc_motor)
{
    if (bdc_motor == NULL)
        return;
}

void bdc_motor_set_motor_speed(bdc_motor_t *bdc_motor, int speed)
{
    if (bdc_motor == NULL)
        return;

    speed = speed > BDC_MOTOR_MAX_PWM_PUTY ? BDC_MOTOR_MAX_PWM_PUTY : speed;
    speed = speed < BDC_MOTOR_MIN_PWM_PUTY ? BDC_MOTOR_MIN_PWM_PUTY : speed;

    if (speed > 0)
    {
        ledc_set_duty(LED_PWM_MODE, bdc_motor->pwm_gpio_a_channel, speed);
        ledc_update_duty(LED_PWM_MODE, bdc_motor->pwm_gpio_a_channel);

        ledc_set_duty(LED_PWM_MODE, bdc_motor->pwm_gpio_b_channel, 0);
        ledc_update_duty(LED_PWM_MODE, bdc_motor->pwm_gpio_b_channel);
    }

    if (speed <= 0)
    {
        ledc_set_duty(LED_PWM_MODE, bdc_motor->pwm_gpio_a_channel, 0);
        ledc_update_duty(LED_PWM_MODE, bdc_motor->pwm_gpio_a_channel);

        ledc_set_duty(LED_PWM_MODE, bdc_motor->pwm_gpio_b_channel, speed * -1);
        ledc_update_duty(LED_PWM_MODE, bdc_motor->pwm_gpio_b_channel);
    }
}
