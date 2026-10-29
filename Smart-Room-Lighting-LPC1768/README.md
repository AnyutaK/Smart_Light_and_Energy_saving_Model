# Smart Room Lighting and Energy Saving System — LPC1768

An embedded-C mini-project that senses ambient light using an LDR, selects a lighting level, controls an external LED with hardware PWM, and displays status and estimated energy savings on a 16×2 LCD.

## Features

- **Automatic lighting:** DAY (0%), EVENING (40%), NIGHT (90%).
- **LDR input:** 12-bit ADC, channel AD0.0 on P0.23, with 16-sample averaging.
- **Hysteresis:** configurable threshold margin to reduce rapid category switching.
- **ECO mode:** after 20 seconds in an unchanged non-DAY category, duty cycle becomes 60% of normal (40% → 24%; 90% → 54%).
- **LCD:** four rotating screens for mode/brightness, energy estimate, savings/runtime and raw ADC reading.
- **Energy model:** software estimate against a hypothetical 10 W lamp at 100% duty; **not a measurement of real LED power**.

## Hardware

- ALS LPC1768 ARM Cortex-M3 trainer kit
- LDR, 10 kΩ resistor, breadboard and jumper wires
- External LED with 300 Ω series resistor
- Onboard 16×2 LCD

| Signal | Connection | Role |
| --- | --- | --- |
| LDR divider output | P0.23 / AD0.0 (CND pin 1 per ALS manual) | Analog input |
| LDR divider supply | 3.3 V | Supply (not 5 V) |
| LDR divider ground | Common GND | ADC reference |
| LED PWM | P1.23 / PWM1.4 (CNB pin 1 per manual) | External LED via 300 Ω resistor |
| LED ground | Common GND | LED return |
| LCD | CNA → CNAD | P0.4–P0.9, 4-bit LCD interface |

The LCD was relocated from CND to CNA to free P0.23 for the LDR ADC input. Check physical connector orientation against your trainer kit manual before wiring.

## Operation

```text
Ambient light → LDR + 10 kΩ voltage divider → AD0.0
→ 12-bit ADC → averaged reading → DAY / EVENING / NIGHT
→ hysteresis and ECO timing → PWM1.4 → external LED
                              ↘ LCD + energy estimate
```

With the LDR connected between 3.3 V and the ADC junction, and the 10 kΩ resistor between that junction and ground, increasing illumination generally increases the ADC reading and decreases requested LED brightness.

## Source layout

- `src/main.c` — integration and LCD screen scheduling
- `src/adc.c` — ADC setup and readings
- `src/pwm.c` — PWM setup and duty cycle
- `src/lcd.c` — 4-bit LCD driver
- `src/timer.c` — timebase and delays
- `src/lighting.c` — classification, hysteresis and ECO mode
- `src/energy.c` — estimated energy and savings
- `include/` — module headers and `config.h`
- `tests/` — standalone peripheral test programs (build **one** test main at a time)
- `docs/` — wiring and test guidance

## Building and flashing

1. Open Keil µVision and create or open an LPC1768 target using the appropriate CMSIS device/startup files from your installed device pack.
2. Add all seven application `.c` files from `src/` and add `include/` to the C include paths.
3. **Do not** include any `tests/*_test.c` in the integrated target; each test has its own `main()`.
4. Enable HEX generation and build the project.
5. Flash the generated HEX using Flash Magic with the board's supported UART ISP configuration, then reset the board to normal execution mode.
6. Verify the LCD, change illumination over the LDR, and wait 20 seconds to observe ECO mode.

The ZIP is based on the supplied Keil project source. This cleaned repository has not been recompiled or independently hardware-tested after packaging. The Keil project file, generated binaries and device-pack startup files are intentionally not included; create a target for your own Keil installation.

## Calibration

Default thresholds are `ADC_DAY_MIN = 2500`, `ADC_NIGHT_MAX = 1200`, `ADC_HYST = 100`. Inspect the raw ADC screen under bright, normal and dark conditions and adjust the thresholds in `include/config.h` if needed.

## Limitations

ECO is based on time spent in the same light category, **not occupancy detection**. Energy consumption is estimated from duty cycle, nominal power and runtime, not measured with a current or power sensor. This prototype uses three brightness categories rather than closed-loop lux regulation.

## Contributors

Add all team members and their contributions here before publishing.
