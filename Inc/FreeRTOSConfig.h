#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>

/* The project currently runs directly from the 16 MHz internal HSI clock. */
#define configCPU_CLOCK_HZ                         ( 16000000UL )
#define configTICK_RATE_HZ                         ( ( TickType_t ) 1000U )

#define configUSE_PREEMPTION                       1
#define configUSE_TIME_SLICING                     1
#define configUSE_TICKLESS_IDLE                    0
#define configMAX_PRIORITIES                       5
#define configMINIMAL_STACK_SIZE                   ( ( uint16_t ) 128U )
#define configMAX_TASK_NAME_LEN                    16
#define configTICK_TYPE_WIDTH_IN_BITS              TICK_TYPE_WIDTH_32_BITS
#define configIDLE_SHOULD_YIELD                    1

#define configSUPPORT_STATIC_ALLOCATION            0
#define configSUPPORT_DYNAMIC_ALLOCATION           1
#define configTOTAL_HEAP_SIZE                      ( 24U * 1024U )
#define configAPPLICATION_ALLOCATED_HEAP            0

#define configUSE_IDLE_HOOK                        0
#define configUSE_TICK_HOOK                        0
#define configUSE_MALLOC_FAILED_HOOK               1
#define configCHECK_FOR_STACK_OVERFLOW             2

#define configUSE_MUTEXES                          1
#define configUSE_RECURSIVE_MUTEXES                1
#define configUSE_COUNTING_SEMAPHORES              1
#define configUSE_QUEUE_SETS                       0
#define configQUEUE_REGISTRY_SIZE                  8
#define configUSE_TIMERS                           0

#define configUSE_TRACE_FACILITY                   0
#define configUSE_STATS_FORMATTING_FUNCTIONS       0
#define configGENERATE_RUN_TIME_STATS              0

/* STM32F407 implements four NVIC priority bits. */
#define configPRIO_BITS                            4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY    15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#define configKERNEL_INTERRUPT_PRIORITY            \
    ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << ( 8U - configPRIO_BITS ) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY       \
    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8U - configPRIO_BITS ) )

#define INCLUDE_vTaskPrioritySet                   1
#define INCLUDE_uxTaskPriorityGet                  1
#define INCLUDE_vTaskDelete                        1
#define INCLUDE_vTaskSuspend                       1
#define INCLUDE_vTaskDelayUntil                    1
#define INCLUDE_vTaskDelay                         1
#define INCLUDE_xTaskGetSchedulerState             1

/* Route the Cortex-M exception vectors to the FreeRTOS GCC port. */
#define vPortSVCHandler                            SVC_Handler
#define xPortPendSVHandler                         PendSV_Handler
#define xPortSysTickHandler                        SysTick_Handler

void vAssertCalled( const char * file, int line );
#define configASSERT( condition )                                      \
    do                                                                 \
    {                                                                  \
        if( ( condition ) == 0 )                                       \
        {                                                              \
            vAssertCalled( __FILE__, __LINE__ );                       \
        }                                                              \
    } while( 0 )

#endif /* FREERTOS_CONFIG_H */
