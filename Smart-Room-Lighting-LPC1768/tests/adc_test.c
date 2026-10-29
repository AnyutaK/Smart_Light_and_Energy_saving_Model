#include <LPC17xx.h>
#include <stdint.h>
#include <stdio.h>
#include "lcd.h"
#include "timer.h"

/* ALS LPC1768: POT1 connected to P1.31 / AD0.5 */

static void adc_init(void)
{
    /* Enable ADC peripheral */
    LPC_SC->PCONP |= (1UL << 12);

    /* Set ADC peripheral clock to CCLK/4 */
    LPC_SC->PCLKSEL0 &= ~(3UL << 24);

    /* Configure P1.31 as AD0.5 */
    LPC_PINCON->PINSEL3 &= ~(3UL << 30);
    LPC_PINCON->PINSEL3 |=  (3UL << 30);

    /* Disable pull-up/pull-down */
    LPC_PINCON->PINMODE3 |= (3UL << 30);

    /* ADC channel 5, safe clock divider */
    LPC_ADC->ADCR = (1UL << 5) |
                    (25UL << 8) |
                    (1UL << 21);
}

static unsigned int adc_read(void)
{
    unsigned long result;
    unsigned long timeout;

    /* Start conversion */
    LPC_ADC->ADCR &= ~(7UL << 24);
    LPC_ADC->ADCR |=  (1UL << 24);

    timeout = 1000000UL;

    /* Wait for conversion, but don't hang forever */
    do
    {
        result = LPC_ADC->ADDR5;

        if (result & (1UL << 31))
        {
            LPC_ADC->ADCR &= ~(7UL << 24);

            return (unsigned int)((result >> 4) & 0xFFF);
        }

        timeout--;

    } while (timeout > 0UL);

    /* Stop conversion */
    LPC_ADC->ADCR &= ~(7UL << 24);

    return 0xFFFFu;  /* Timeout indicator */
}

int main(void)
{
    unsigned int value;
    char line[17];

    SystemInit();
    SystemCoreClockUpdate();

    timer_init();
    lcd_init();

    lcd_write_row(0, "LCD INITIALIZED");
    lcd_write_row(1, "STARTING ADC...");
    delay_ms(2000);

    adc_init();

    lcd_write_row(0, "ADC INITIALIZED");
    lcd_write_row(1, "READING POT1...");
    delay_ms(2000);

    while (1)
    {
        value = adc_read();

        if (value == 0xFFFFu)
        {
            lcd_write_row(0, "ADC TIMEOUT");
            lcd_write_row(1, "CHECK ADC SETUP");
        }
        else
        {
            lcd_write_row(0, "ADC TEST - POT1");

            sprintf(line, "VALUE: %4u", value);
            lcd_write_row(1, line);
        }

        delay_ms(300);
    }
}
