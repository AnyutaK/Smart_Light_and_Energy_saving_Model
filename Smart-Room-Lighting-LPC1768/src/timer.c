#include "timer.h"
#include "config.h"

static volatile uint32_t g_ms = 0;

void SysTick_Handler(void)
{
    g_ms++;
}

void timer_init(void)
{
    /* Timer0: 1 us tick, free running (used for delay_us) */
    LPC_SC->PCONP    |= (1u << 1);              /* power Timer0            */
    LPC_SC->PCLKSEL0 &= ~(3u << 2);             /* Timer0 PCLK = CCLK/4    */
    LPC_TIM0->TCR = 2u;                         /* reset                   */
    LPC_TIM0->PR  = (PCLK_HZ / 1000000u) - 1u;  /* 1 MHz tick              */
    LPC_TIM0->TCR = 1u;                         /* enable                  */

    /* SysTick: 1 ms interrupt (used for millis / delay_ms) */
    g_ms = 0;
    SysTick_Config(SystemCoreClock / 1000u);
}

uint32_t millis(void)
{
    return g_ms;
}

void delay_ms(uint32_t ms)
{
    uint32_t start = g_ms;
    while ((uint32_t)(g_ms - start) < ms) {
        /* wait */
    }
}

void delay_us(uint32_t us)
{
    uint32_t start = LPC_TIM0->TC;
    while ((uint32_t)(LPC_TIM0->TC - start) < us) {
        /* wait */
    }
}
