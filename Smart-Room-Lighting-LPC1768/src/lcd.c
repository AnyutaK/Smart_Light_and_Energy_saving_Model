#include "lcd.h"
#include "config.h"
#include "timer.h"

/* HD44780 16x2 LCD, 4-bit mode. RW is assumed tied to GND (write only). */

static void lcd_pulse_enable(void)
{
    LCD_GPIO->FIOSET = LCD_EN_BIT;
    delay_us(2);
    LCD_GPIO->FIOCLR = LCD_EN_BIT;
    delay_us(60);
}

static void lcd_write_nibble(uint8_t nibble)
{
    LCD_GPIO->FIOCLR = LCD_DATA_MASK;
    LCD_GPIO->FIOSET = ((uint32_t)(nibble & 0x0Fu)) << LCD_D4_SHIFT;
    lcd_pulse_enable();
}

static void lcd_write_byte(uint8_t value, uint8_t is_data)
{
    if (is_data) LCD_GPIO->FIOSET = LCD_RS_BIT;
    else         LCD_GPIO->FIOCLR = LCD_RS_BIT;

    lcd_write_nibble((uint8_t)(value >> 4));     /* high nibble first */
    lcd_write_nibble((uint8_t)(value & 0x0Fu));
}

static void lcd_command(uint8_t cmd)
{
    lcd_write_byte(cmd, 0);
    if (cmd <= 0x03u) delay_ms(3);               /* clear / home are slow */
}

void lcd_init(void)
{   uint32_t pins;
    /* LCD is mapped to P0.4-P0.9 to leave P0.23 available for ADC. */
    /* Configure P0.4 to P0.9 as GPIO */
LPC_PINCON->PINSEL0 &=
    ~((3UL << 8)  |
      (3UL << 10) |
      (3UL << 12) |
      (3UL << 14) |
      (3UL << 16) |
      (3UL << 18));
    /* Pins as GPIO outputs */
    pins = LCD_DATA_MASK | LCD_RS_BIT | LCD_EN_BIT;
#if LCD_USE_RW
    pins |= LCD_RW_BIT;
#endif
    LCD_GPIO->FIODIR |= pins;
    LCD_GPIO->FIOCLR  = pins;

    delay_ms(50);                                /* power-up wait     */

    /* Standard HD44780 "initialise by instruction" sequence */
    LCD_GPIO->FIOCLR = LCD_RS_BIT;
    lcd_write_nibble(0x3); delay_ms(5);
    lcd_write_nibble(0x3); delay_us(200);
    lcd_write_nibble(0x3); delay_us(200);
    lcd_write_nibble(0x2); delay_us(200);        /* switch to 4-bit   */

    lcd_command(0x28);   /* 4-bit, 2 lines, 5x8 font */
    lcd_command(0x08);   /* display off              */
    lcd_command(0x01);   /* clear                    */
    lcd_command(0x06);   /* entry mode: increment    */
    lcd_command(0x0C);   /* display on, cursor off   */
}

void lcd_clear(void)
{
    lcd_command(0x01);
}

void lcd_write_row(uint8_t row, const char *text)
{
    uint8_t i;
    uint8_t ended = 0;

    lcd_command((row == 0u) ? 0x80u : 0xC0u);    /* set DDRAM address */

    for (i = 0; i < 16u; i++) {
        char c = ' ';
        if (!ended) {
            if (text[i] == '\0') ended = 1;
            else c = text[i];
        }
        lcd_write_byte((uint8_t)c, 1);
    }
}
