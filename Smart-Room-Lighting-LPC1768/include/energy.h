#ifndef ENERGY_H
#define ENERGY_H

#include <stdint.h>

/* All energy values are ESTIMATES: rated power x PWM duty x time. */
void     energy_init(uint32_t now_ms);
void     energy_update(uint32_t now_ms, uint8_t duty_percent);

uint32_t energy_used_tenth_mwh(void);       /* estimated use, units of 0.1 mWh  */
uint32_t energy_baseline_tenth_mwh(void);   /* same period at 100% brightness   */
uint32_t energy_saved_percent(void);        /* 0..100                           */
uint32_t energy_elapsed_s(void);            /* runtime in seconds               */

#endif
