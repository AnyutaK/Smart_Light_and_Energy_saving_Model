#include "adc.h"
#include "config.h"

#define ADCR_PDN        (1u << 21)
#define ADCR_START_NOW  (1u << 24)
#define ADCR_START_MASK (7u << 24)
#define ADGDR_DONE      (1u << 31)

static uint16_t g_last = 0;

static void adc_pin_setup(void)
{
    /* Select ADC function and "no pull-up / no pull-down" (PINMODE = 10) */
    switch (ADC_CHANNEL) {
    case 0: /* P0.23 */
        LPC_PINCON->PINSEL1  = (LPC_PINCON->PINSEL1  & ~(3u << 14)) | (1u << 14);
        LPC_PINCON->PINMODE1 = (LPC_PINCON->PINMODE1 & ~(3u << 14)) | (2u << 14);
        break;
    case 1: /* P0.24 */
        LPC_PINCON->PINSEL1  = (LPC_PINCON->PINSEL1  & ~(3u << 16)) | (1u << 16);
        LPC_PINCON->PINMODE1 = (LPC_PINCON->PINMODE1 & ~(3u << 16)) | (2u << 16);
        break;
    case 2: /* P0.25 */
        LPC_PINCON->PINSEL1  = (LPC_PINCON->PINSEL1  & ~(3u << 18)) | (1u << 18);
        LPC_PINCON->PINMODE1 = (LPC_PINCON->PINMODE1 & ~(3u << 18)) | (2u << 18);
        break;
    case 3: /* P0.26 */
        LPC_PINCON->PINSEL1  = (LPC_PINCON->PINSEL1  & ~(3u << 20)) | (1u << 20);
        LPC_PINCON->PINMODE1 = (LPC_PINCON->PINMODE1 & ~(3u << 20)) | (2u << 20);
        break;
    case 4: /* P1.30 (ADC function is "11") */
        LPC_PINCON->PINSEL3  = (LPC_PINCON->PINSEL3  & ~(3u << 28)) | (3u << 28);
        LPC_PINCON->PINMODE3 = (LPC_PINCON->PINMODE3 & ~(3u << 28)) | (2u << 28);
        break;
    case 5: /* P1.31 (ADC function is "11") */
        LPC_PINCON->PINSEL3  = (LPC_PINCON->PINSEL3  & ~(3u << 30)) | (3u << 30);
        LPC_PINCON->PINMODE3 = (LPC_PINCON->PINMODE3 & ~(3u << 30)) | (2u << 30);
        break;
    default:
        break;
    }
}

void adc_init(void)
{
    uint32_t clkdiv;

    LPC_SC->PCONP    |= (1u << 12);        /* power the ADC            */
    LPC_SC->PCLKSEL0 &= ~(3u << 24);       /* ADC PCLK = CCLK/4        */

    adc_pin_setup();

    /* ADC clock = PCLK / (CLKDIV+1) must be <= 13 MHz. Aim for ~4.5 MHz. */
    clkdiv = PCLK_HZ / 4500000u;
    if (clkdiv > 255u) clkdiv = 255u;

    LPC_ADC->ADCR = (1u << ADC_CHANNEL)    /* select channel           */
                  | (clkdiv << 8)
                  | ADCR_PDN;              /* operational, software start */
}

uint16_t adc_read_raw(void)
{
    uint32_t v;
    uint32_t guard = 200000u;

    LPC_ADC->ADCR &= ~ADCR_START_MASK;     /* make sure idle           */
    LPC_ADC->ADCR |= ADCR_START_NOW;       /* start conversion now     */

    do {
        v = LPC_ADC->ADGDR;                /* reading clears DONE      */
        if (--guard == 0u) {
            LPC_ADC->ADCR &= ~ADCR_START_MASK;
            return g_last;                 /* timeout: keep old value  */
        }
    } while ((v & ADGDR_DONE) == 0u);

    LPC_ADC->ADCR &= ~ADCR_START_MASK;
    g_last = (uint16_t)((v >> 4) & 0xFFFu);
    return g_last;
}

uint16_t adc_read_avg(void)
{
    uint32_t sum = 0;
    uint32_t i;
    for (i = 0; i < ADC_SAMPLES_AVG; i++) {
        sum += adc_read_raw();
    }
    return (uint16_t)(sum / ADC_SAMPLES_AVG);
}

uint16_t adc_to_percent(uint16_t raw)
{
    return (uint16_t)(((uint32_t)raw * 100u) / 4095u);
}
