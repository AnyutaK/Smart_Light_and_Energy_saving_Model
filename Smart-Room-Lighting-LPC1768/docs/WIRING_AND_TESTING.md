# Wiring and verification

## LDR voltage divider

```text
3.3 V --- LDR ---+--- P0.23 / AD0.0
                 |
               10 kΩ
                 |
                GND
```

## External LED

```text
P1.23 / PWM1.4 --- 300 Ω --- LED anode (+)
                                  LED cathode (-) --- GND
```

The resistor must be in series with the LED. Keep all LPC1768 signals at their permitted voltage and current levels. Disconnect power before rewiring.

## LCD

CNA to CNAD ribbon; P0.4–P0.7 are LCD D4–D7, P0.8 is RS, P0.9 is EN. LCD is in write-only 4-bit mode.

## Demonstration checklist

1. LCD starts and rotates status screens.
2. Bright light raises ADC reading; darkening the LDR lowers it.
3. External LED is off in DAY, dim in EVENING, bright in NIGHT.
4. Leave the LDR in a non-DAY category for 20 seconds; ECO reduces brightness.
5. Change category; ECO clears and its timer restarts.
6. Verify runtime and energy estimates update on the LCD.

## Debugging notes

- If ADC remains near zero, verify the LDR divider has 3.3 V and common GND.
- If the LED is stuck at one brightness, inspect the ADC raw reading and thresholds first.
- The ADC uses software-triggered conversion and polls DONE; it does not use burst mode.
