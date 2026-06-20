#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#define LEDS    (GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)
#define PWM_PERIOD_US    20000U
#define PWM_STEP_MS      1000U
#define PWM_STEPS        3U
#define LOG_TIMEOUT_MS   20U

static TIM_HandleTypeDef pwm_timer;
static UART_HandleTypeDef log_uart;

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

static void uart_init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();

    gpio.Pin = GPIO_PIN_2;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &gpio);

    log_uart.Instance = USART2;
    log_uart.Init.BaudRate = 115200U;
    log_uart.Init.WordLength = UART_WORDLENGTH_8B;
    log_uart.Init.StopBits = UART_STOPBITS_1;
    log_uart.Init.Parity = UART_PARITY_NONE;
    log_uart.Init.Mode = UART_MODE_TX;
    log_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    log_uart.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&log_uart) != HAL_OK)
    {
        for (;;)
        {
        }
    }
}

static void pwm_init(void)
{
    GPIO_InitTypeDef gpio = {0};
    TIM_OC_InitTypeDef channel = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_TIM1_CLK_ENABLE();

    gpio.Pin = GPIO_PIN_8;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &gpio);

    /* TIM1 runs at 16 MHz from the default HSI clock: 1 timer count = 1 us. */
    pwm_timer.Instance = TIM1;
    pwm_timer.Init.Prescaler = 16U - 1U;
    pwm_timer.Init.CounterMode = TIM_COUNTERMODE_UP;
    pwm_timer.Init.Period = PWM_PERIOD_US - 1U;
    pwm_timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    pwm_timer.Init.RepetitionCounter = 0U;
    pwm_timer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    channel.OCMode = TIM_OCMODE_PWM1;
    channel.Pulse = 1000U;
    channel.OCPolarity = TIM_OCPOLARITY_HIGH;
    channel.OCFastMode = TIM_OCFAST_DISABLE;

    if ((HAL_TIM_PWM_Init(&pwm_timer) != HAL_OK) ||
        (HAL_TIM_PWM_ConfigChannel(&pwm_timer, &channel, TIM_CHANNEL_1) != HAL_OK) ||
        (HAL_TIM_PWM_Start(&pwm_timer, TIM_CHANNEL_1) != HAL_OK))
    {
        for (;;)
        {
        }
    }
}

static void pwm_task(void *argument)
{
    static const uint32_t pulse_us[] = {1000U, 1500U, 2000U};
    static const uint8_t log_message[][18] = {
        "PA8 PWM: 1.0 ms\r\n",
        "PA8 PWM: 1.5 ms\r\n",
        "PA8 PWM: 2.0 ms\r\n"
    };
    uint32_t step = 0U;

    (void)argument;

    for (;;)
    {
        __HAL_TIM_SET_COMPARE(&pwm_timer, TIM_CHANNEL_1, pulse_us[step]);
        HAL_UART_Transmit(&log_uart, log_message[step],
                          sizeof(log_message[step]) - 1U, LOG_TIMEOUT_MS);
        step = (step + 1U) % PWM_STEPS;
        vTaskDelay(pdMS_TO_TICKS(PWM_STEP_MS));
    }
}

int main(void)
{
    HAL_Init();

    led_init();
    uart_init();
    pwm_init();

    if (xTaskCreate(blink_task, "blink", 128U, NULL, 1U, NULL) != pdPASS)
    {
        for (;;)
        {
        }
    }

    if (xTaskCreate(pwm_task, "pwm", 128U, NULL, 1U, NULL) != pdPASS)
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
