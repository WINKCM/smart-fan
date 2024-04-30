# LED CONTROL
此组件是一个led控制组件，可以切换led的四种状态：打开、关闭、呼吸和闪烁。

## depend
- high resolution timer ： block driver components下的high resolution timer。

## get start
- 使用 ```led_control_handle_t led_control_create(led_control_config_t led_control_config)```创建句柄，注意检查传回是否未NULL；
    - 注意 ```led_control_config```内要注册pwm的初始化和pwm设定的回调函数。
- 使用 ```led_control_error_t led_control_task_start(led_control_handle_t led_control_handle); ``` 打开led控制的任务，此时led控制真正运行。默认创建了一个占用1024 *4，优先级为4的freertos任务；
- 使用 ```led_control_error_t led_control_switch_led_state(led_control_handle_t led_control_handle, led_control_state_t led_state); ```切换led状态，内部保证了内存安全。
- 使用 ```led_control_error_t led_control_task_stop(led_control_handle_t led_control_handle); ```停止led控制任务，内部保证了内存安全。
- 使用 ```led_control_error_t led_control_delete(led_control_handle_t led_control_handle);```删除句柄、回收内存。
