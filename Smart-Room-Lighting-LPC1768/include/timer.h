#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/* SysTick gives a 1 ms tick (millis); Timer0 is a 1 us free-running counter. */
void     timer_init(void);
uint32_t millis(void);               /* ms since timer_init(), wraps ~49 days */
void     delay_ms(uint32_t ms);
void     delay_us(uint32_t us);      /* us must be < ~4 billion */

#endif
