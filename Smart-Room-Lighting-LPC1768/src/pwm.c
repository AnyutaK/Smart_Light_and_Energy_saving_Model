#include "pwm.h"
#include "config.h"

/* PWM period in 1 us ticks, e.g. 1 kHz -> 1000 ticks */
#define PWM_PERIOD_TICKS  (1000000u / PWM_FREQ_HZ)

static uint8_t g_duty = 0;

static void pwm_write_match(uint32_t value)
{
    switch (PWM_CHANNEL) {
    case 1: LPC_PWM1->MR1 = value; break;
    case 2: LPC_PWM1->MR2 = value; break;
    case 3: LPC_PWM1->MR3 = value; break;
    case 4: LPC_PWM1->MR4 = value; break;
    case 5: LPC_PWM1->MR5 = value; break;
    case 6: LPC_PWM1->MR6 = value; break;
    default: break;
    }
}

void pwm_init(void)
{
    LPC_SC->PCONP    |= (1u << 6);          /* power PWM1               */
    LPC_SC->PCLKSEL0 &= ~(3u << 12);        /* PWM1 PCLK = CCLK/4       */

    LPC_PWM1->TCR = (1u << 1);              /* hold counter in reset    */
    LPC_PWM1->PR  = (PCLK_HZ / 1000000u) - 1u;   /* 1 us tick           */
    LPC_PWM1->MCR = (1u << 1);              /* reset counter on MR0     */
    LPC_PWM1->MR0 = PWM_PERIOD_TICKS;

    pwm_write_match(0);
    LPC_PWM1->LER = 1u | (1u << PWM_CHANNEL);        /* latch MR0 + MRx */
    LPC_PWM1->PCR = (1u << (8 + PWM_CHANNEL));       /* enable output   */
    LPC_PWM1->TCR = (1u << 0) | (1u << 3);           /* counter + PWM   */

    /* ALS trainer board: PWM1.4 = P1.23, PINSEL3[15:14] = 10. */
#if PWM_CHANNEL == 4
    LPC_PINCON->PINSEL3 = (LPC_PINCON->PINSEL3 & ~(3u << 14)) | (2u << 14);
#else
#error "This ALS build supports PWM1.4 on P1.23 only"
#endif

    g_duty = 0;
    pwm_set_duty(0);
}

void pwm_set_duty(uint8_t percent)
{
    uint32_t level;
    uint32_t match;

    if (percent > 100u) percent = 100u;
    g_duty = percent;

    level = PWM_ACTIVE_LOW ? (100u - percent) : percent;

    if (level == 0u) {
        match = 0u;                              /* output stays low  */
    } else if (level >= 100u) {
        match = PWM_PERIOD_TICKS + 1u;           /* never matches: stays high */
    } else {
        match = (PWM_PERIOD_TICKS * level) / 100u;
    }

    pwm_write_match(match);
    LPC_PWM1->LER = (1u << PWM_CHANNEL);         /* latch at next cycle */
}

uint8_t pwm_get_duty(void)
{
    return g_duty;
}
