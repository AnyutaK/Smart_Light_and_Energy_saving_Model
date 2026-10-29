#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <LPC17xx.h>

/* =====================================
   SMART ROOM LIGHTING SYSTEM
   ALS LPC1768 TRAINER KIT
   ===================================== */

/* CLOCK */
#define PCLK_HZ (SystemCoreClock / 4UL)

/* =====================================
   ADC CONFIGURATION
   ===================================== */

/* LDR input: P0.23 / AD0.0 via CND. */
#define ADC_CHANNEL       0
#define ADC_SAMPLES_AVG   16

/* Day / Evening / Night thresholds */
#define ADC_DAY_MIN       2500
#define ADC_NIGHT_MAX     1200
#define ADC_HYST          100

/* =====================================
   LED BRIGHTNESS
   ===================================== */

#define BRIGHTNESS_DAY       0
#define BRIGHTNESS_EVENING  40
#define BRIGHTNESS_NIGHT    90

/* =====================================
   AUTOMATIC ECO MODE
   ===================================== */

#define ECO_DELAY_MS      20000UL
#define ECO_PERCENT       60

/* =====================================
   PWM CONFIGURATION
   ===================================== */

/* P1.23 / PWM1.4 -> LD10
   Connection: CNB -> CNB1
*/
#define PWM_CHANNEL       4
#define PWM_FREQ_HZ       1000u
#define PWM_ACTIVE_LOW    0

/* =====================================
   LCD CONFIGURATION - RELOCATED
   ===================================== */

/* New connection:
   CNA -> CNAD

   LCD D4 -> P0.4
   LCD D5 -> P0.5
   LCD D6 -> P0.6
   LCD D7 -> P0.7
   LCD RS -> P0.8
   LCD EN -> P0.9
*/

#define LCD_GPIO          LPC_GPIO0
#define LCD_D4_SHIFT      4
#define LCD_DATA_MASK     (0xFUL << LCD_D4_SHIFT)

#define LCD_RS_BIT        (1UL << 8)
#define LCD_EN_BIT        (1UL << 9)

/* LCD write-only mode */
#define LCD_USE_RW        0
#define LCD_RW_BIT        0UL

/* =====================================
   ENERGY ESTIMATION
   ===================================== */

/* Simulated 10 W lamp */
#define LED_RATED_MW      10000UL

/* =====================================
   TIMING CONFIGURATION
   ===================================== */

#define SAMPLE_PERIOD_MS  100UL
#define LCD_REFRESH_MS    250UL
#define SCREEN_ROTATE_MS  2000UL

#endif
