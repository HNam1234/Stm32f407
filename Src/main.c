#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#if defined(DEBUG)
#include <stdio.h>
extern void initialise_monitor_handles(void);
#define log_printf(...)  do { printf(__VA_ARGS__); fflush(stdout); } while (0)
#else
#define log_printf(...)  do { } while (0)
#endif

#define LEDS    (GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)
#define PWM_PERIOD_US    20000U
#define PWM_STEP_MS      1000U
#define PWM_STEPS        3U
#define LCD_I2C_TIMEOUT  100U
#define UART4_RX_QUEUE_LENGTH 64U

#define LCD_RS           0x01U
#define LCD_ENABLE       0x04U
#define LCD_BACKLIGHT    0x08U

static I2C_HandleTypeDef lcd_i2c;
static TIM_HandleTypeDef pwm_timer;
static UART_HandleTypeDef esp_uart;
static uint16_t lcd_i2c_addr;
static QueueHandle_t uart4_rx_queue;
static uint8_t uart4_rx_byte;

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

static HAL_StatusTypeDef lcd_write4(uint8_t nibble, uint8_t rs)
{
    uint8_t data[3];

    data[0] = (nibble & 0xF0U) | LCD_BACKLIGHT | rs;
    data[1] = data[0] | LCD_ENABLE;
    data[2] = data[0];

    return HAL_I2C_Master_Transmit(&lcd_i2c, lcd_i2c_addr, data,
                                   sizeof(data), LCD_I2C_TIMEOUT);
}

static HAL_StatusTypeDef lcd_write(uint8_t value, uint8_t rs)
{
    if (lcd_write4(value, rs) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return lcd_write4(value << 4U, rs);
}

static HAL_StatusTypeDef lcd_init(void)
{
    vTaskDelay(pdMS_TO_TICKS(50U));

    if (lcd_write4(0x30U, 0U) != HAL_OK)
    {
        return HAL_ERROR;
    }
    vTaskDelay(pdMS_TO_TICKS(5U));

    if (lcd_write4(0x30U, 0U) != HAL_OK)
    {
        return HAL_ERROR;
    }
    vTaskDelay(pdMS_TO_TICKS(1U));

    if ((lcd_write4(0x30U, 0U) != HAL_OK) ||
        (lcd_write4(0x20U, 0U) != HAL_OK) ||
        (lcd_write(0x28U, 0U) != HAL_OK) ||
        (lcd_write(0x08U, 0U) != HAL_OK) ||
        (lcd_write(0x01U, 0U) != HAL_OK))
    {
        return HAL_ERROR;
    }
    vTaskDelay(pdMS_TO_TICKS(2U));

    return lcd_write(0x06U, 0U) == HAL_OK &&
           lcd_write(0x0CU, 0U) == HAL_OK ? HAL_OK : HAL_ERROR;
}

static HAL_StatusTypeDef lcd_print(const char *text)
{
    while (*text != '\0')
    {
        if (lcd_write((uint8_t)*text, LCD_RS) != HAL_OK)
        {
            return HAL_ERROR;
        }
        text++;
    }

    return HAL_OK;
}

static HAL_StatusTypeDef i2c_init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C2_CLK_ENABLE();

    gpio.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOB, &gpio);

    lcd_i2c.Instance = I2C2;
    lcd_i2c.Init.ClockSpeed = 100000U;
    lcd_i2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
    lcd_i2c.Init.OwnAddress1 = 0U;
    lcd_i2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    lcd_i2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    lcd_i2c.Init.OwnAddress2 = 0U;
    lcd_i2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    lcd_i2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    return HAL_I2C_Init(&lcd_i2c);
}

static uint16_t lcd_find_address(void)
{
    uint16_t address;

    for (address = 0x20U; address <= 0x3FU; address++)
    {
        if (HAL_I2C_IsDeviceReady(&lcd_i2c, address << 1U, 2U,
                                  LCD_I2C_TIMEOUT) == HAL_OK)
        {
            return address << 1U;
        }
    }

    return 0U;
}

static void lcd_task(void *argument)
{
    HAL_StatusTypeDef status;

    (void)argument;

    log_printf("\r\n[lcd] task started\r\n");

    status = i2c_init();
    log_printf("[lcd] I2C2 init: %d\r\n", (int)status);

    if (status == HAL_OK)
    {
        lcd_i2c_addr = lcd_find_address();

        if (lcd_i2c_addr == 0U)
        {
            log_printf("[lcd] no PCF8574 at 0x20-0x3F\r\n");
        }
        else
        {
            log_printf("[lcd] found at 0x%02lX\r\n",
                       (unsigned long)(lcd_i2c_addr >> 1U));

            status = lcd_init();
            log_printf("[lcd] init: %d\r\n", (int)status);

            if (status == HAL_OK)
            {
                status = lcd_print("Hello");
                log_printf("[lcd] print: %d\r\n", (int)status);
            }
        }
    }

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000U));
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
    uint32_t step = 0U;

    (void)argument;

    for (;;)
    {
        __HAL_TIM_SET_COMPARE(&pwm_timer, TIM_CHANNEL_1, pulse_us[step]);
        step = (step + 1U) % PWM_STEPS;
        vTaskDelay(pdMS_TO_TICKS(PWM_STEP_MS));
    }
}

static HAL_StatusTypeDef uart4_init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_UART4_CLK_ENABLE();

    /* PA0 = UART4_TX, PA1 = UART4_RX (both alternate function 8). */
    gpio.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF8_UART4;
    HAL_GPIO_Init(GPIOA, &gpio);

    esp_uart.Instance = UART4;
    esp_uart.Init.BaudRate = 115200U;
    esp_uart.Init.WordLength = UART_WORDLENGTH_8B;
    esp_uart.Init.StopBits = UART_STOPBITS_1;
    esp_uart.Init.Parity = UART_PARITY_NONE;
    esp_uart.Init.Mode = UART_MODE_TX_RX;
    esp_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    esp_uart.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&esp_uart) != HAL_OK)
    {
        return HAL_ERROR;
    }

    HAL_NVIC_SetPriority(UART4_IRQn, 5U, 0U);
    HAL_NVIC_EnableIRQ(UART4_IRQn);

    return HAL_UART_Receive_IT(&esp_uart, &uart4_rx_byte, 1U);
}

void UART4_IRQHandler(void)
{
    HAL_UART_IRQHandler(&esp_uart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t task_woken = pdFALSE;

    if ((huart->Instance == UART4) && (uart4_rx_queue != NULL))
    {
        (void)xQueueSendFromISR(uart4_rx_queue, &uart4_rx_byte, &task_woken);
        (void)HAL_UART_Receive_IT(&esp_uart, &uart4_rx_byte, 1U);
        portYIELD_FROM_ISR(task_woken);
    }
}

static void uart4_task(void *argument)
{
    char line[64];
    uint8_t length = 0U;
    uint8_t byte;

    (void)argument;

    log_printf("[uart4] ready: 115200 8N1\r\n");

    for (;;)
    {
        if (xQueueReceive(uart4_rx_queue, &byte, portMAX_DELAY) == pdPASS)
        {
            if ((byte == '\r') || (byte == '\n'))
            {
                if (length > 0U)
                {
                    line[length] = '\0';
                    log_printf("[uart4] RX: %s\r\n", line);
                    length = 0U;
                }
            }
            else if ((byte >= 32U) && (byte <= 126U))
            {
                if (length < (sizeof(line) - 1U))
                {
                    line[length++] = (char)byte;
                }
                else
                {
                    line[length] = '\0';
                    log_printf("[uart4] RX: %s\r\n", line);
                    length = 0U;
                }
            }
        }
    }
}
int main(void)
{
    HAL_Init();

#if defined(DEBUG)
    initialise_monitor_handles();
    log_printf("[boot] started\r\n");
#endif

    led_init();
    pwm_init();

    uart4_rx_queue = xQueueCreate(UART4_RX_QUEUE_LENGTH, sizeof(uart4_rx_byte));
    if ((uart4_rx_queue == NULL) || (uart4_init() != HAL_OK))
    {
        for (;;)
        {
        }
    }
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

    if (xTaskCreate(lcd_task, "lcd", 256U, NULL, 1U, NULL) != pdPASS)
    {
        for (;;)
        {
        }
    }

    if (xTaskCreate(uart4_task, "uart4", 256U, NULL, 1U, NULL) != pdPASS)
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
