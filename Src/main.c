#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

/* Only the registers needed for the four LEDs on PD12--PD15. */
#define RCC_AHB1ENR    ( *( volatile uint32_t * ) 0x40023830U )
#define GPIOD_MODER    ( *( volatile uint32_t * ) 0x40020C00U )
#define GPIOD_ODR      ( *( volatile uint32_t * ) 0x40020C14U )

#define GPIOD_CLOCK    ( 1U << 3 )
#define LEDS           ( ( 1U << 12 ) | ( 1U << 13 ) | ( 1U << 14 ) | ( 1U << 15 ) )

void SystemInit( void )
{
	/* FreeRTOS uses the Cortex-M4F port, so enable the FPU. */
	*( volatile uint32_t * ) 0xE000ED88U |= ( 0xFU << 20 );
}

static void led_init( void )
{
	RCC_AHB1ENR |= GPIOD_CLOCK;

	/* PD12--PD15 = output. */
	GPIOD_MODER &= ~( ( 3U << 24 ) | ( 3U << 26 ) | ( 3U << 28 ) | ( 3U << 30 ) );
	GPIOD_MODER |= ( ( 1U << 24 ) | ( 1U << 26 ) | ( 1U << 28 ) | ( 1U << 30 ) );

	GPIOD_ODR &= ~LEDS;
}

static void blink_task( void * argument )
{
	( void ) argument;

	for( ;; )
	{
		GPIOD_ODR ^= LEDS;
		vTaskDelay( pdMS_TO_TICKS( 500U ) );
	}
}

int main( void )
{
	led_init();

	if( xTaskCreate( blink_task, "blink", 128U, NULL, 1U, NULL ) != pdPASS )
	{
		for( ;; )
		{
		}
	}

	vTaskStartScheduler();

	for( ;; )
	{
	}
}
