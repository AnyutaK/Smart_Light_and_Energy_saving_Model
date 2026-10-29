/*
 * lcd_test.c - TEST 3: does the LCD show text?
 *
 * Build INSTEAD of main.c. Expected: row 0 "LCD TEST OK", row 1 shows a
 * counter that goes up every second.
 * Nothing shown? Check pin defines in config.h, the contrast knob on the
 * kit, and whether the kit LCD is really in 4-bit mode (use the lab example).
 * Garbage characters usually mean D4..D7 are swapped or the init timing/mode
 * differs from the kit's working example.
 */
#include <stdio.h>
#include "config.h"
#include "timer.h"
#include "lcd.h"

int main(void)
{
    char row[40];
    uint32_t n = 0;

    timer_init();
    lcd_init();
    lcd_write_row(0, "LCD TEST OK");

    while (1) {
        sprintf(row, "Count: %lu", (unsigned long)n++);
        lcd_write_row(1, row);
        delay_ms(1000);
    }
}
