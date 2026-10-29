# Smart Room Lighting and Energy Saving System using LPC1768

An automatic ambient-light-based lighting control system developed using the **LPC1768 ARM Cortex-M3 microcontroller**, featuring LDR sensing, ADC-based light detection, PWM brightness control, LCD status monitoring, automatic ECO mode, and estimated energy consumption.

Developed as part of the **Embedded Systems Mini Project** at Manipal Institute of Technology, Manipal.

## Project Overview

Conventional lighting systems often operate at a fixed brightness regardless of the amount of natural light available, resulting in unnecessary energy consumption.

This project implements a smart lighting system that automatically adjusts LED brightness based on ambient light conditions.

An LDR detects changes in light intensity, and a voltage-divider circuit converts these changes into an analog voltage. The LPC1768's built-in ADC converts this voltage into digital values, which are processed to classify the surrounding environment as Day, Evening, or Night.

Based on the detected lighting condition, the microcontroller adjusts an external LED's brightness using Pulse Width Modulation (PWM).

The system also incorporates hysteresis to prevent rapid switching, a timer-based ECO mode to reduce brightness, and an LCD interface for displaying operating information.

## Key Features

- **Automatic Lighting Control:** Adjusts LED brightness according to ambient light intensity.
- **LDR-Based Light Detection:** Uses a photoresistor and voltage-divider circuit.
- **12-Bit ADC:** Converts analog sensor voltage into digital values ranging from 0 to 4095.
- **PWM Brightness Control:** Regulates LED brightness using the LPC1768's PWM1.4 peripheral.
- **Day/Evening/Night Classification:** Provides three predefined brightness levels.
- **Hysteresis:** Prevents frequent switching when ADC readings fluctuate near thresholds.
- **Automatic ECO Mode:** Reduces LED brightness after 20 seconds in the same lighting category.
- **LCD Status Display:** Shows lighting conditions, brightness, ADC readings, runtime, and estimated energy consumption.
- **Energy Estimation:** Calculates approximate energy usage and savings compared with a full-brightness baseline.

## Hardware Requirements

| Component | Quantity | Purpose |
|---|---|---|
| LPC1768 ARM Cortex-M3 Trainer Kit | 1 | Main controller |
| LDR (Photoresistor) | 1 | Ambient light sensing |
| 10 kΩ Resistor | 1 | LDR voltage divider |
| 5 mm LED | 1 | Lighting output |
| 300 Ω Resistor | 1 | LED current limiting |
| Mini Breadboard | 1 | External circuit assembly |
| Jumper Wires | As required | Electrical connections |
| 16×2 LCD | 1 (onboard) | Status display |
| Ribbon Cable | 1 | LCD interface |

## Software Requirements

- Keil µVision
- Embedded C
- LPC1768 CMSIS/device support files
- Flash Magic
- Windows-based development environment for programming the trainer kit

## System Architecture

```text
                 Ambient Light
                       |
                       v
                LDR + 10k Resistor
                       |
                       v
                Voltage Divider
                       |
                       v
              LPC1768 ADC (AD0.0)
                       |
                       v
              ADC Signal Processing
                       |
                       v
             Lighting Classification
               /       |       \
             DAY    EVENING    NIGHT
              |        |         |
             0%       40%       90%
               \       |        /
                       |
                       v
                ECO Mode Logic
                       |
                       v
                PWM Controller
                       |
                       v
                  External LED

          +--------------------------+
          |       LPC1768            |
          |                          |
          |  Timer / Energy Estimate |
          |           |              |
          |           v              |
          |       16x2 LCD           |
          +--------------------------+
```

## Hardware Connections

### 1. LDR Sensor Interface

The LDR is connected to the LPC1768 through a voltage-divider circuit.

| Connection | LPC1768 |
|---|---|
| LDR supply | 3.3 V |
| Voltage-divider output | P0.23 / AD0.0 |
| Ground | GND |
| ADC interface | CND |

The circuit is arranged as follows:

```text
             3.3 V
               |
              LDR
               |
               +--------> P0.23 / AD0.0
               |
          10k Resistor
               |
              GND
```

The voltage-divider output is:

\[
V_{ADC}=3.3\times\frac{10000}{R_{LDR}+10000}
\]

As ambient light increases, LDR resistance decreases, causing the ADC input voltage to increase.

### 2. External LED Interface

| Connection | LPC1768 |
|---|---|
| PWM signal | P1.23 / PWM1.4 |
| Connector | CNB |
| Current-limiting resistor | 300 Ω |
| LED cathode | GND |

```text
P1.23 / PWM1.4
       |
    300 Ohm
       |
    LED (+)
    LED (-)
       |
      GND
```

PWM controls the average brightness of the LED by varying the duty cycle.

### 3. LCD Interface

The LCD is connected using 4-bit communication.

| LCD Signal | LPC1768 Pin |
|---|---|
| D4 | P0.4 |
| D5 | P0.5 |
| D6 | P0.6 |
| D7 | P0.7 |
| RS | P0.8 |
| EN | P0.9 |

**Connection:** CNA → CNAD

The LCD was relocated from CND to CNA to make P0.23 available for the external LDR's ADC input.

## Working Principle

### Step 1: Ambient Light Detection

The LDR changes resistance depending on the surrounding light intensity.

- High light intensity → Low LDR resistance.
- Low light intensity → High LDR resistance.

The voltage-divider circuit converts this resistance variation into an analog voltage.

### Step 2: Analog-to-Digital Conversion

The LPC1768 ADC converts the sensor voltage into a 12-bit digital value.

The ADC provides values from 0 to 4095.

The program averages 16 ADC samples to reduce fluctuations in the sensor readings.

### Step 3: Lighting Classification

The digital value is compared with predefined thresholds.

| Condition | ADC Reading | LED Brightness |
|---|---|---|
| Day | High | 0% |
| Evening | Medium | 40% |
| Night | Low | 90% |

The initial thresholds are configured in `config.h`:

```c
#define ADC_DAY_MIN       2500
#define ADC_NIGHT_MAX     1200
#define ADC_HYST          100
```

These thresholds can be adjusted based on actual LDR readings.

### Step 4: PWM Brightness Control

PWM controls LED brightness by varying the percentage of time the output remains HIGH during each cycle.

The duty cycle is calculated as:

\[
D=\frac{T_{ON}}{T_{PERIOD}}\times100
\]

For example:

- 0% duty cycle → LED OFF
- 40% duty cycle → Moderate brightness
- 90% duty cycle → High brightness

The LPC1768's PWM1.4 peripheral generates the PWM output.

### Step 5: Hysteresis

Hysteresis prevents rapid switching between lighting categories when ADC readings fluctuate around a threshold.

Instead of switching at exactly the same value in both directions, the system uses a small margin around the threshold.

This improves stability and reduces unwanted LED flickering.

### Step 6: Automatic ECO Mode

The system incorporates a timer-based energy-saving mode.

When the lighting category remains unchanged for 20 seconds, ECO mode activates and reduces the normal brightness to 60%.

| Lighting Condition | Normal Brightness | ECO Brightness |
|---|---|---|
| Day | 0% | 0% |
| Evening | 40% | 24% |
| Night | 90% | 54% |

When the lighting category changes, ECO mode is cancelled and its timer restarts.

**Note:** ECO mode is based on elapsed time within a lighting category. It does not detect room occupancy.

### Step 7: Energy Consumption Estimation

The system estimates energy consumption using an assumed 10 W lamp.

Estimated power:

\[
P_{estimated}=P_{rated}\times\frac{D}{100}
\]

Estimated energy:

\[
E=P_{estimated}\times t
\]

Where:

- \(P_{rated}\) is the assumed lamp power.
- \(D\) is the PWM duty-cycle percentage.
- \(t\) is operating time in hours.

Energy savings are calculated by comparing estimated consumption with a baseline operating continuously at full brightness.

The project uses a simulated lamp rating of 10 W, defined in `config.h`.

**Energy consumption is estimated in software and is not directly measured from the external LED.**

## LCD Display

The onboard 16×2 LCD displays four rotating information screens.

| Screen | Information |
|---|---|
| 1 | Lighting category and LED brightness |
| 2 | Estimated energy consumption compared with full brightness |
| 3 | Estimated energy savings and runtime |
| 4 | Raw ADC reading for calibration |

The display rotation interval is configurable in `config.h`.

## Software Architecture

The project uses modular Embedded C programming.

| File | Description |
|---|---|
| `main.c` | Main application and module integration |
| `adc.c` | ADC initialization and sensor readings |
| `pwm.c` | PWM initialization and LED brightness control |
| `lcd.c` | LCD initialization and display functions |
| `timer.c` | Timing and delay functions |
| `lighting.c` | Lighting classification, hysteresis and ECO logic |
| `energy.c` | Energy estimation and savings calculations |
| `config.h` | Hardware configuration and system parameters |

The corresponding header files contain function declarations and shared interfaces.

## Building and Running the Project

### Step 1: Create the Keil Project

1. Open Keil µVision.
2. Create a new project.
3. Select the NXP LPC1768 microcontroller.
4. Configure the required LPC1768 device support and startup files.
5. Add the project source files and required headers.

### Step 2: Configure the Hardware

Verify the hardware connections:

- LCD connected through CNA → CNAD.
- External LED connected to PWM1.4 through a 300 Ω resistor.
- LDR voltage divider connected to AD0.0.
- Sensor circuit supplied using 3.3 V and common ground.

### Step 3: Compile

Build the project in Keil and resolve any compilation or linking errors.

Enable HEX file generation under the target output settings.

### Step 4: Flash the Microcontroller

Use Flash Magic to program the generated HEX file onto the LPC1768.

Configure the correct device, COM port, oscillator frequency and ISP settings according to the trainer kit manual.

### Step 5: Run

Reset the microcontroller in normal execution mode.

The system begins reading ambient light and adjusting the LED automatically.

## Testing and Results

The project was tested using the ALS LPC1768 trainer kit.

### ADC Testing

Initially, the onboard potentiometer was used to verify ADC operation.

The ADC values changed as the potentiometer was rotated, confirming successful analog-to-digital conversion.

The external LDR circuit was subsequently connected for actual light sensing.

### PWM Testing

The PWM output was tested independently using the onboard LED and later an external LED.

The LED successfully changed brightness in response to PWM duty-cycle adjustments.

### LCD Testing

The LCD was tested using a counter program before integration.

The display successfully initialized and updated its contents.

### Integrated System Testing

The integrated system demonstrated automatic lighting classification, PWM brightness adjustment and ECO mode activation.

The LCD displayed system information through rotating screens.

### Demonstration Procedure

1. Illuminate the LDR using a phone flashlight.
2. Observe the ADC value increasing and the LED dimming.
3. Cover the LDR to simulate darkness.
4. Observe the LED brightness increasing.
5. Maintain the same lighting category for 20 seconds.
6. Observe ECO mode activating and reducing brightness.
7. Change the lighting condition to reset ECO mode.

## Challenges and Solutions

### 1. LCD Pin Conflict

**Challenge:** The original LCD connection occupied P0.23, which was required for the external LDR.

**Solution:** Relocated the LCD to CNA and modified the GPIO configuration to use P0.4–P0.9.

### 2. Keil Compilation and Linking Errors

**Challenge:** Initial compilation encountered variable declaration and undefined-symbol errors.

**Solution:** Corrected declarations for the compiler and ensured that the required source modules were included in the build.

### 3. ADC Reading Issues

**Challenge:** ADC readings initially remained extremely low.

**Solution:** Checked the LDR voltage-divider circuit and identified an incomplete 3.3 V supply connection.

### 4. External LED Integration

**Challenge:** The onboard LED was insufficient for a clear external lighting demonstration.

**Solution:** Used an external LED with a 300 Ω current-limiting resistor connected to the PWM output.

## Limitations

- Brightness control uses three predefined levels rather than continuous adjustment.
- ECO mode is timer-based and does not detect occupancy.
- Energy consumption is estimated rather than physically measured.
- LDR readings depend on sensor placement and ambient conditions.
- Fixed thresholds may require recalibration under different lighting environments.
- The prototype controls a low-power LED rather than a full-scale room lighting fixture.

## Future Enhancements

- PIR-based occupancy detection.
- Continuous adaptive brightness control.
- Real-time voltage and current monitoring.
- Automatic sensor calibration.
- User-configurable brightness thresholds.
- Wireless monitoring through Wi-Fi or Bluetooth.
- Integration with a suitable driver for higher-power lighting.

## Conclusion

The Smart Room Lighting and Energy Saving System demonstrates the integration of analog sensing, digital control, PWM output generation, LCD interfacing and timer-based automation using the LPC1768 ARM Cortex-M3 microcontroller.

The project successfully implements automatic lighting control based on ambient light conditions and provides software-based estimates of energy consumption and savings.

It also demonstrates practical embedded-systems development through hardware interfacing, register-level programming, peripheral testing, debugging and system integration.

## References

1. NXP Semiconductors, *LPC17xx User Manual (UM10360)*.
2. ARM, *Cortex-M3 Technical Reference Manual*.
3. Embedded Systems Laboratory Manual, Manipal Institute of Technology.
4. Keil, *µVision Development Tools Documentation*.

## Contributors

Developed as an Embedded Systems Mini Project at **Manipal Institute of Technology, Manipal**.

**Team Members:**

Aastha Khandelwal
Jonalagadda Bindu Keerthi
Anyuta Kumar
