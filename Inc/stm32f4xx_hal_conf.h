#ifndef STM32F4XX_HAL_CONF_H
#define STM32F4XX_HAL_CONF_H

/* Enable only the HAL modules used by this project. */
#define HAL_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED

#define HSE_VALUE                 8000000U
#define HSI_VALUE                 16000000U
#define VDD_VALUE                 3300U
#define TICK_INT_PRIORITY          15U
#define USE_RTOS                   0U

#define assert_param( expr )      ( ( void ) 0U )

#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_gpio.h"

#endif
