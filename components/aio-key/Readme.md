# aio key

aio-key是针对一键开关电路的驱动，此按键在长按状态下会开启电源，再次长按，关闭电源。在开启电源后短按可以触发短按回调。开启后的led是可以控制的。

## depend
- led control ：block driver components下的led-control。（led-control内部依赖了block driver components下的high-resolution-timer，注意添加Cmakelist路径）；
- button : esp官方的按键控制器使用以下语句添加 ```idf.py add-dependency "espressif/button^3.2.0"```。

## get start
