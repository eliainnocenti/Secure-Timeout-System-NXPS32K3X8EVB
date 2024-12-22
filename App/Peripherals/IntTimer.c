/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "semphr.h"
#include "FreeRTOSConfig.h"

/* Application includes */
#include "globals.h"
// # include "secure_timeout_system.h" // Debug

/* Peripheral includes */
#include "uart.h"
#include "IntTimer.h"
#include "printf-stdarg.h"

/* Library includes. */
#include "S32K3X8EVB.h"

/* External variables and functions */
extern int userActivity;
extern int userActivityDetection;
extern int suspiciousActivity;
extern int suspiciousActivityDetection;
// extern uint32_t userActivity;
// extern uint32_t userActivityDetection;
// extern uint32_t suspiciousActivity;
// extern uint32_t suspiciousActivityDetection;

/* CPU clock frequency */
// #define configCPU_CLOCK_HZ      ( 143000000UL )      // Already defined in FreeRTOSConfig.h

/* Timer 0 and Timer 1 frequencies */
#define tmrTIMER_0_FREQUENCY	( 2UL )         // TODO: update (?)
#define tmrTIMER_1_FREQUENCY	( 2UL )         // TODO: update (?)

/* Timer IRQ numbers */
#define TIMER0_IRQ_num 8
#define TIMER1_IRQ_num 9

void vInitialiseTimers( void )
{
    printf("------------------- Initialization of Hardware Timers --------------------\n\n");

    /* Initialise Timer 0 */

    printf("Initialising Timer 0\n");

    S32K3X8_TIMER0->INTCLR = TIMER_INTCLR_Msk;             /* Clear any pending interrupts */
    S32K3X8_TIMER0->RELOAD   = ( configCPU_CLOCK_HZ /      /* Set reload value */
                                 tmrTIMER_0_FREQUENCY );
    S32K3X8_TIMER0->CTRL     = ( ( 1ul <<  3 ) |           /* Enable Timer interrupt. */
                                 ( 1ul <<  0 ) );          /* Enable Timer. */
    
    NVIC_SetPriority( TIMER0_IRQ_num, configMAX_SYSCALL_INTERRUPT_PRIORITY );
    NVIC_EnableIRQ( TIMER0_IRQ_num );

    printf("Timer 0 initialised\n");

    /* Initialise Timer 1 */

    printf("Initialising Timer 1\n");

    S32K3X8_TIMER1->INTCLR = TIMER_INTCLR_Msk;             /* Clear any pending interrupts */
    S32K3X8_TIMER1->RELOAD   = ( configCPU_CLOCK_HZ /      /* Set reload value */
                                 tmrTIMER_1_FREQUENCY );
    S32K3X8_TIMER1->CTRL     = ( ( 1ul <<  3 ) |           /* Enable Timer interrupt. */
                                 ( 1ul <<  0 ) );          /* Enable Timer. */

    NVIC_SetPriority( TIMER1_IRQ_num, configMAX_SYSCALL_INTERRUPT_PRIORITY );
    NVIC_EnableIRQ( TIMER1_IRQ_num );

    printf("Timer 1 initialised\n");

    printf("\n--------------------------------------------------------------------------\n");
    printf("\n");
}

void TIMER0_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Clear the interrupt */
    S32K3X8_TIMER0->INTCLR = TIMER_INTCLR_Msk;

    /* Main functionality */
    printf("Timer 0 Interrupt: looking for user activities...\n");
    printf("Timer 0: userActivity = %d\n", userActivity); // Debug
    userActivityDetection = (userActivity == 1) ? 1 : 0;
    printf("Timer 0: userActivityDetection = %d\n", userActivityDetection); // Debug

    /* Perform a context switch if necessary */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void TIMER1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Clear the interrupt */
    S32K3X8_TIMER1->INTCLR = TIMER_INTCLR_Msk;

    /* Main functionality */
    printf("Timer 1 Interrupt: looking for suspicious activities...\n");
    printf("Timer 1: suspiciousActivity = %d\n", suspiciousActivity); // Debug
    suspiciousActivityDetection = (suspiciousActivity == 1) ? 1 : 0;
    printf("Timer 1: suspiciousActivityDetection = %d\n", suspiciousActivityDetection); // Debug

    /* Perform a context switch if necessary */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
