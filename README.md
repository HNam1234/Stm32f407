# STM32F407 FreeRTOS LED demo

Bare-metal STM32F407G-DISC1 project built with STM32CubeIDE and the GNU Arm
toolchain. FreeRTOS Kernel is tracked as a Git submodule.

Four equal-priority tasks blink the Discovery board LEDs independently:

| LED | GPIO | Period |
| --- | --- | ---: |
| Green | PD12 | 250 ms |
| Orange | PD13 | 500 ms |
| Red | PD14 | 750 ms |
| Blue | PD15 | 1000 ms |

Clone with the kernel submodule:

```sh
git clone --recurse-submodules https://github.com/HNam1234/Stm32f407.git
```

The firmware currently uses the reset-default 16 MHz HSI clock. If the clock
tree is changed, update `configCPU_CLOCK_HZ` in `Inc/FreeRTOSConfig.h`.
