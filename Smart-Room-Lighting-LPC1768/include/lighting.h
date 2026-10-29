#ifndef LIGHTING_H
#define LIGHTING_H

#include <stdint.h>

typedef enum {
    LIGHT_DAY = 0,
    LIGHT_EVENING,
    LIGHT_NIGHT
} light_cat_t;

void         lighting_init(void);
/* Feed a new ADC reading (0..4095) and the current time in ms.
 * Returns the LED brightness in percent (0..100) after the eco rule. */
uint8_t      lighting_update(uint16_t adc_value, uint32_t now_ms);

light_cat_t  lighting_get_category(void);
const char  *lighting_category_name(light_cat_t c);
uint8_t      lighting_get_brightness(void);
uint8_t      lighting_eco_active(void);

#endif
