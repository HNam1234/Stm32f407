#ifndef STM32F4XX_HAL_CONF_H
#define STM32F4XX_HAL_CONF_H

/* Enable only the HAL drivers used by the application. */
#define HAL_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED

#define HSE_VALUE                 8000000U
#define HSI_VALUE                 16000000U
#define HSE_STARTUP_TIMEOUT        100U
#define LSE_STARTUP_TIMEOUT        5000U
#define EXTERNAL_CLOCK_VALUE       12288000U
#define VDD_VALUE                 3300U
#define TICK_INT_PRIORITY          15U
#define USE_RTOS                   0U

#define PREFETCH_ENABLE            1U
#define INSTRUCTION_CACHE_ENABLE   1U
#define DATA_CACHE_ENABLE          1U

#define assert_param( expr )      ( ( void ) 0U )

#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_uart.h"

/* To add a peripheral, enable its HAL_<NAME>_MODULE_ENABLED macro above
 * and include its stm32f4xx_hal_<name>.h header here. Its .c source is
 * already compiled from the HAL submodule. */

#endif
