/*
 * pwm_test.c - TEST 2: does the LED fade with PWM?
 *
 * Build INSTEAD of main.c. Expected: the LED slowly ramps 0% -> 100% -> 0%
 * forever. If it only switches fully on/off, or never lights, the pin is not
 * PWM-capable or the wiring/polarity is wrong (check PWM_ACTIVE_LOW and confirm CNB -> CNB1, LD10, P1.23).
 * g_pwm_duty can be watched in the debugger.
 */
#include "config.h"
#include "timer.h"
#include "pwm.h"

volatile uint8_t g_pwm_duty = 0;

int main(void)
{
    int duty;

    timer_init();
    pwm_init();

    while (1) {
        for (duty = 0; duty <= 100; duty += 10) {
            g_pwm_duty = (uint8_t)duty;
            pwm_set_duty((uint8_t)duty);
            delay_ms(300);
        }
        for (duty = 100; duty >= 0; duty -= 10) {
            g_pwm_duty = (uint8_t)duty;
            pwm_set_duty((uint8_t)duty);
            delay_ms(300);
        }
    }
}
