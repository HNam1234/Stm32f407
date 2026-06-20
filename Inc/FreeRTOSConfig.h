#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* The MCU starts from its 16 MHz HSI clock. */
#define configCPU_CLOCK_HZ                    16000000UL
#define configTICK_RATE_HZ                    1000U

#define configUSE_PREEMPTION                  1
#define configMAX_PRIORITIES                  3
#define configMINIMAL_STACK_SIZE              128U
#define configTOTAL_HEAP_SIZE                 ( 8U * 1024U )
#define configTICK_TYPE_WIDTH_IN_BITS         TICK_TYPE_WIDTH_32_BITS

#define configSUPPORT_DYNAMIC_ALLOCATION      1
#define configSUPPORT_STATIC_ALLOCATION       0
#define configUSE_IDLE_HOOK                   0
#define configUSE_TICK_HOOK                   0
#define configUSE_TIMERS                      0
#define INCLUDE_vTaskDelay                    1

/* STM32F407 has four implemented NVIC priority bits. */
#define configPRIO_BITS                       4
#define configKERNEL_INTERRUPT_PRIORITY       ( 15U << 4U )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY  ( 5U << 4U )

/* FreeRTOS owns these three Cortex-M exceptions. */
#define vPortSVCHandler                       SVC_Handler
#define xPortPendSVHandler                    PendSV_Handler
#define xPortSysTickHandler                   SysTick_Handler

/* Stop here if FreeRTOS detects an invalid configuration or state. */
#define configASSERT( x )                     do { if( !( x ) ) { for( ;; ) {} } } while( 0 )

#endif
