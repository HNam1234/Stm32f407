#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#define LEDS    (GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)

static void led_init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOD_CLK_ENABLE();

    gpio.Pin = LEDS;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOD, &gpio);
    HAL_GPIO_WritePin(GPIOD, LEDS, GPIO_PIN_RESET);
}

static void blink_task(void *argument)
{
    (void)argument;

    for (;;)
    {
        HAL_GPIO_TogglePin(GPIOD, LEDS);
        vTaskDelay(pdMS_TO_TICKS(500U));
    }
}

int main(void)
{
    HAL_Init();

    led_init();

    if (xTaskCreate(blink_task, "blink", 128U, NULL, 1U, NULL) != pdPASS)
    {
        for (;;)
        {
        }
    }

    vTaskStartScheduler();

    for (;;)
    {
    }
}
