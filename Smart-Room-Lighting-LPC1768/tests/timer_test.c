/*
 * timer_test.c - TEST 4: are the millisecond and microsecond timers correct?
 *
 * Build INSTEAD of main.c. Expected: the LED (PWM pin) blinks exactly once per
 * second (0.5 s on, 0.5 s off). Compare with a phone stopwatch over 60 s.
 * In the debugger g_seconds should increase by 1 per real second.
 * If it runs too fast/slow, the SystemCoreClock value does not match the real
 * CPU clock set by the startup/system file - fix that first.
 */
#include "config.h"
#include "timer.h"
#include "pwm.h"

volatile uint32_t g_seconds = 0;
volatile uint32_t g_us_check = 0;

int main(void)
{
    uint32_t last = 0;
    uint8_t on = 0;
    uint32_t t0;

    timer_init();
    pwm_init();

    /* quick delay_us check: 10000 us should read as ~10000 */
    t0 = LPC_TIM0->TC;
    delay_us(10000);
    g_us_check = LPC_TIM0->TC - t0;

    while (1) {
        if ((uint32_t)(millis() - last) >= 500u) {
            last += 500u;
            on = (uint8_t)!on;
            pwm_set_duty(on ? 100u : 0u);
            if (!on) g_seconds++;
        }
    }
}
