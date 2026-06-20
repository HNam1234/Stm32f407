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

GPIO uses `HAL_GPIO_Init()` and `HAL_GPIO_TogglePin()`. `HAL_Init()` runs before
the scheduler; the FreeRTOS tick hook keeps `HAL_GetTick()` in sync afterward.
In a task, prefer `vTaskDelay()` over blocking with `HAL_Delay()`.

PA8 is configured as `TIM1_CH1` PWM for a servo-style 20 ms period. Its pulse
width changes from 1.0 ms to 1.5 ms to 2.0 ms every second; change
`PWM_STEP_MS` in `Src/main.c` to adjust that interval.

The Debug build logs PWM changes with `printf()` through semihosting; the text
appears in the STM32CubeIDE debug console. Release builds compile those logs
out, so no UART pin or external adapter is required.

## Adding another HAL peripheral

The project compiles the STM32F4 HAL source directory directly from the HAL
submodule. The common RCC and FLASH modules are enabled already. To use another
peripheral, enable its module and include its header in
`Inc/stm32f4xx_hal_conf.h`; for example, UART needs:

```c
#define HAL_UART_MODULE_ENABLED
#include "stm32f4xx_hal_uart.h"
```
