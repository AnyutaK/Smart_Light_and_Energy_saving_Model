#ifndef PWM_H
#define PWM_H

#include <stdint.h>

void    pwm_init(void);
void    pwm_set_duty(uint8_t percent);   /* 0..100, values >100 are clamped */
uint8_t pwm_get_duty(void);

#endif
