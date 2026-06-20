# STM32F407 FreeRTOS LED demo

Bare-metal STM32F407G-DISC1 project built with STM32CubeIDE and the GNU Arm
toolchain. FreeRTOS, STM32 HAL, CMSIS device headers and CMSIS core are tracked
as Git submodules.

One FreeRTOS task blinks all four Discovery board LEDs every 500 ms:

| LEDs | GPIO |
| --- | --- |
| Green, orange, red, blue | PD12--PD15 |

Clone with the kernel submodule:

```sh
git clone --recurse-submodules https://github.com/HNam1234/Stm32f407.git
```

The firmware currently uses the reset-default 16 MHz HSI clock. If the clock
tree is changed, update `configCPU_CLOCK_HZ` in `Inc/FreeRTOSConfig.h`.

GPIO uses `HAL_GPIO_Init()` and `HAL_GPIO_TogglePin()`. Do not call
`HAL_Init()` or `HAL_Delay()` unless HAL is moved to a timer-based time source:
FreeRTOS owns SysTick in this project.
