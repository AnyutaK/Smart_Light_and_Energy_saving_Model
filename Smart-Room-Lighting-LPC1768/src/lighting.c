#include "lighting.h"
#include "config.h"

static light_cat_t g_cat        = LIGHT_DAY;
static uint8_t     g_first      = 1;
static uint32_t    g_cat_since  = 0;     /* time the category last changed */
static uint8_t     g_eco        = 0;
static uint8_t     g_brightness = 0;

/* Plain classification without hysteresis (used for the first reading). */
static light_cat_t classify_plain(uint16_t adc)
{
    if (adc >= ADC_DAY_MIN)   return LIGHT_DAY;
    if (adc <  ADC_NIGHT_MAX) return LIGHT_NIGHT;
    return LIGHT_EVENING;
}

/* Classification with hysteresis so the LED does not flicker at a border. */
static light_cat_t classify_hyst(uint16_t adc)
{
    int v = (int)adc;

    switch (g_cat) {
    case LIGHT_DAY:
        if (v < (ADC_DAY_MIN - ADC_HYST)) {
            return (v < ADC_NIGHT_MAX) ? LIGHT_NIGHT : LIGHT_EVENING;
        }
        break;
    case LIGHT_EVENING:
        if (v >= (ADC_DAY_MIN + ADC_HYST))        return LIGHT_DAY;
        if (v <  (ADC_NIGHT_MAX - ADC_HYST))      return LIGHT_NIGHT;
        break;
    case LIGHT_NIGHT:
        if (v >= (ADC_NIGHT_MAX + ADC_HYST)) {
            return (v >= (ADC_DAY_MIN + ADC_HYST)) ? LIGHT_DAY : LIGHT_EVENING;
        }
        break;
    default:
        break;
    }
    return g_cat;
}

static uint8_t base_brightness(light_cat_t c)
{
    switch (c) {
    case LIGHT_DAY:     return BRIGHTNESS_DAY;
    case LIGHT_EVENING: return BRIGHTNESS_EVENING;
    default:            return BRIGHTNESS_NIGHT;
    }
}

void lighting_init(void)
{
    g_cat        = LIGHT_DAY;
    g_first      = 1;
    g_cat_since  = 0;
    g_eco        = 0;
    g_brightness = 0;
}

uint8_t lighting_update(uint16_t adc_value, uint32_t now_ms)
{
    light_cat_t newcat;
    uint8_t base;

    if (g_first) {
        g_cat       = classify_plain(adc_value);
        g_cat_since = now_ms;
        g_first     = 0;
    } else {
        newcat = classify_hyst(adc_value);
        if (newcat != g_cat) {
            g_cat       = newcat;
            g_cat_since = now_ms;        /* category changed: restart eco timer */
        }
    }

    base  = base_brightness(g_cat);
    g_eco = 0;

    if (base > 0u && (uint32_t)(now_ms - g_cat_since) >= ECO_DELAY_MS) {
        g_eco = 1;
        base  = (uint8_t)(((uint32_t)base * ECO_PERCENT) / 100u);
    }

    g_brightness = base;
    return g_brightness;
}

light_cat_t lighting_get_category(void) { return g_cat; }
uint8_t     lighting_get_brightness(void) { return g_brightness; }
uint8_t     lighting_eco_active(void)     { return g_eco; }

const char *lighting_category_name(light_cat_t c)
{
    switch (c) {
    case LIGHT_DAY:     return "DAY";
    case LIGHT_EVENING: return "EVENING";
    default:            return "NIGHT";
    }
}
