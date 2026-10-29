#ifndef LCD_H
#define LCD_H

#include <stdint.h>

void lcd_init(void);
void lcd_clear(void);
void lcd_write_row(uint8_t row, const char *text);   /* row 0 or 1; pads/truncates to 16 chars */

#endif
