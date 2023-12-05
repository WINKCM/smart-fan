#ifndef _bdc_motor_h
#define _bdc_motor_h

typedef struct
{
    int pwm_gpio_a_channel;
    int pwm_gpio_b_channel;
} bdc_motor_t;

bdc_motor_t *bdc_motor_create(int pwm_gpio_a, int pwm_gpio_a_channel, int pwm_gpio_b, int pwm_gpio_b_channel);
void bdc_motor_delete(bdc_motor_t *bdc_motor);

void bdc_motor_set_motor_speed(bdc_motor_t *bdc_motor, int speed);

#endif
