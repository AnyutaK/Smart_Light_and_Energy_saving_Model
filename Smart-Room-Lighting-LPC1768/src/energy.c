#include "energy.h"
#include "config.h"

/*
 * Accumulators use units of (mW * percent * ms), kept in 64-bit integers.
 *   mW*ms        = acc / 100
 *   mWh          = acc / (100 * 3,600,000)
 *   0.1 mWh      = acc * 10 / 360,000,000 = acc / 36,000,000
 */
#define ACC_PER_TENTH_MWH  36000000ULL

static uint32_t           g_last_ms   = 0;
static unsigned long long g_used_acc  = 0;
static unsigned long long g_base_acc  = 0;
static unsigned long long g_elapsed_ms = 0;

void energy_init(uint32_t now_ms)
{
    g_last_ms     = now_ms;
    g_used_acc    = 0;
    g_base_acc    = 0;
    g_elapsed_ms  = 0;
}

void energy_update(uint32_t now_ms, uint8_t duty_percent)
{
    unsigned long long dt = (uint32_t)(now_ms - g_last_ms);
    g_last_ms = now_ms;

    if (duty_percent > 100u) duty_percent = 100u;

    g_used_acc   += (unsigned long long)LED_RATED_MW * duty_percent * dt;
    g_base_acc   += (unsigned long long)LED_RATED_MW * 100ULL * dt;
    g_elapsed_ms += dt;
}

uint32_t energy_used_tenth_mwh(void)
{
    return (uint32_t)(g_used_acc / ACC_PER_TENTH_MWH);
}

uint32_t energy_baseline_tenth_mwh(void)
{
    return (uint32_t)(g_base_acc / ACC_PER_TENTH_MWH);
}

uint32_t energy_saved_percent(void)
{
    if (g_base_acc == 0ULL) return 0u;
    return (uint32_t)(((g_base_acc - g_used_acc) * 100ULL) / g_base_acc);
}

uint32_t energy_elapsed_s(void)
{
    return (uint32_t)(g_elapsed_ms / 1000ULL);
}
