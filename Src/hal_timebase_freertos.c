#include "stm32f4xx_hal.h"

/* Keep HAL_GetTick() in sync with the FreeRTOS 1 ms tick. */
void vApplicationTickHook( void )
{
    HAL_IncTick();
}
