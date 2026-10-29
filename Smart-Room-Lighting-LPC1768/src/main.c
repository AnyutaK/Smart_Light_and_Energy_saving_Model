/*
 * Smart Room Lighting & Energy Saving System - LPC1768
 *
 * LDR/pot -> ADC -> lighting + eco logic -> PWM LED, status on 16x2 LCD.
 * The startup file / SystemInit() from the Keil device pack run before main().
 */
#include <stdio.h>
#include "config.h"
#include "timer.h"
#include "adc.h"
#include "pwm.h"
#include "lcd.h"
#include "lighting.h"
#include "energy.h"

#define NUM_SCREENS 4u

static void show_screen(uint8_t screen, uint16_t adc_raw)
{
    char r0[40];
    char r1[40];
    uint32_t t;

    switch (screen) {
    case 0: /* mode + light category, LED brightness, eco flag */
        sprintf(r0, "AUTO: %s", lighting_category_name(lighting_get_category()));
        sprintf(r1, "LED:%3u%% %s", (unsigned)lighting_get_brightness(),
                lighting_eco_active() ? "ECO" : "");
        break;

    case 1: /* estimated energy used vs full-brightness baseline */
        t = energy_used_tenth_mwh();
        sprintf(r0, "Used %lu.%lu mWh", (unsigned long)(t / 10u), (unsigned long)(t % 10u));
        t = energy_baseline_tenth_mwh();
        sprintf(r1, "Full %lu.%lu mWh", (unsigned long)(t / 10u), (unsigned long)(t % 10u));
        break;

    case 2: /* estimated saving and runtime */
        sprintf(r0, "Saved: %lu%% est", (unsigned long)energy_saved_percent());
        t = energy_elapsed_s();
        sprintf(r1, "Run %02lu:%02lu:%02lu",
                (unsigned long)(t / 3600u),
                (unsigned long)((t / 60u) % 60u),
                (unsigned long)(t % 60u));
        break;

    default: /* calibration help */
        sprintf(r0, "ADC raw: %4u", (unsigned)adc_raw);
        sprintf(r1, "D>%d E>%d", ADC_DAY_MIN, ADC_NIGHT_MAX);
        break;
    }

    lcd_write_row(0, r0);
    lcd_write_row(1, r1);
}

int main(void)
{
    uint32_t now;
    uint32_t last_sample = 0;
    uint32_t last_lcd    = 0;
    uint32_t last_screen = 0;
    uint8_t  screen      = 0;
    uint16_t adc_value;
    uint8_t  duty;

    timer_init();
    adc_init();
    pwm_init();
    lcd_init();

    lcd_write_row(0, "Smart Room Light");
    lcd_write_row(1, "Starting...");
    delay_ms(1000);

    /* first reading so the very first screen is already correct */
    adc_value = adc_read_avg();
    lighting_init();
    duty = lighting_update(adc_value, millis());
    pwm_set_duty(duty);
    energy_init(millis());
    show_screen(screen, adc_value);

    last_sample = last_lcd = last_screen = millis();

    while (1) {
        now = millis();

        if ((uint32_t)(now - last_sample) >= SAMPLE_PERIOD_MS) {
            last_sample = now;
            adc_value = adc_read_avg();
            duty = lighting_update(adc_value, now);
            pwm_set_duty(duty);
            energy_update(now, duty);
        }

        if ((uint32_t)(now - last_screen) >= SCREEN_ROTATE_MS) {
            last_screen = now;
            screen = (uint8_t)((screen + 1u) % NUM_SCREENS);
        }

        if ((uint32_t)(now - last_lcd) >= LCD_REFRESH_MS) {
            last_lcd = now;
            show_screen(screen, adc_value);
        }
    }
}
