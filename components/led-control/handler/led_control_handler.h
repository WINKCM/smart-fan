#ifndef led_control_handler_h
#define led_control_handler_h

#include "led_control_mode.h"

void led_control_led_off_handler(led_control_t *led_control);
void led_control_led_on_handler(led_control_t *led_control);
void led_control_led_blink_handler(led_control_t *led_control);
void led_control_led_blink_close_handler(led_control_t *led_control);
void led_control_led_breath_handler(led_control_t *led_control);
void led_control_led_breath_close_handler(led_control_t *led_control);

#endif
