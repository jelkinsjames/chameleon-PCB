notachevy
#4575

chameleonLab — Today at 1:45 PM
# PCM1863 Physical Board

In our project, we are using the PCM1863 as an analog-to-digital (ADC) converter. The goal of the chip is to take analog input, and change the signal into a readable I2S output.

## Components:
- 0.1uF: X7R, 50V (some are 16V, but we use 50V for all for ease of placement) Ceramic
Expand
message.txt
19 KB
﻿
chameleonLab#0277
# PCM1863 Physical Board

In our project, we are using the PCM1863 as an analog-to-digital (ADC) converter. The goal of the chip is to take analog input, and change the signal into a readable I2S output.

## Components:
- 0.1uF: X7R, 50V (some are 16V, but we use 50V for all for ease of placement) Ceramic
- 10uF: X7R, 10V Tantalum
- 2-2uF: X7R, Ceramic
- 20pF: X7R, 50V Ceramic
- 0.01uF: X7R, Ceramic film
# TODO: finish bill of materials

### Inputs (mono only):
Inputs can have a voltage from [0, 2.1Vrms] in single mode and [0, 4.2Vrms] in differential mode. We are using our device in differential mode.

The chip has two primary ADCs that can be used for stereo audio (for a total of four mono inputs). The chip has a secondary ADC that can be used for low power voltage monitoring and interrupts which we are not using.

The chip can support up to two microphones (analog or digital). Analog inputs are directed through Vin while digital microphones are directed through GPIO. We are not using microphones.

The chip supports two digital microphones using GPIO1 as data input and GPIO2 as a clock. The recommended clock frequency is 2.8224MHz (44.1kHz * 64). The recommended sampling frequency is 44.1kHz where SCK is 256 * the sampling frequency. We are leaving the auto clock detector enabled for digital microphones.

![](/images/digitalmic.png)

The suggested hardware for single-ended inputs is:

Place film-type capacitors of 0.01 µF as close as possible to the VINLx and VINRx pins, and terminate to GND as close as possible to the AGND pin in order to maximize the dynamic performance of ADC. To maintain common-mode rejection, match the series resistors as closely as possible. The suggested hardware for single-ended inputs instead of microphones is:

![](/images/single-ended.png)

Microphone bias can power electret microphones. Input 2.6V to pin 5 (MIC BIAS). Decouple and filter the power. An on-chip terminating resistor (to GND) can be enabled by writing to the *MIC_BIAS_CTRL (0x15)* register. The default is to bypass the onboard terminating resistor. We are going to use the terminating resistor.

DC blocking capacitors are used for all analog input signals. The pins are biased to a Vref of AVDD / 2. *Do not connect unused analog input pins.*

For single-ended analog inputs, use MD2, MD5, MD6. For setting the device to follower mode, use MD0 and MD1.

Types of inputs:
- Control voltage (CV) at 5V stepped down.
- 3.5mm Headphone jack.
- Guitar or bass (through adapter into a 3.5mm jack)

## Outputs:
- Digital I2S signal
- Other configurable outputs are not used for our project.
- GPIO pins have their polarities reversed

## Hardware control configurations:
- **To set to I2C mode:** Set MD0 low.
- **To set hardware address:** Set MDO1 low for I2C address: 0x94. Set MDO1 High for I2C address: 0x96.

The power-up sequence consists of the following steps:
1. Power-on reset
(a) Power up AVDD, DVDD and IOVDD
(b) Check if LDO is being driven with an external 1.8 V, or is an output. Enable LDO if required.
(c) Release digital reset
2. Wait until analog voltage reference is stable
3. Configure clock (PLL requires < 250 µs)
4. Fade-in audio ADC content

## Basic device configuration:
- The device by default starts in follower mode at 48 kHz
- Set global loss level to be –50 dB using the DSP coefficient method.
- Configure active mode
# TODO: how to configure active mode?
- Set Read Data without latch in register AUXADC_DATA_CTRL (0x58)
- Set interrupts (energysense and controlsense) using INT_EN (0x60)
- Set interrupt pulse for 3 ms (makes it easier to see it visually using INT_PLS (0x62))

## Update system settings:
- Read interrupt status INT_STAT register(0x61)
- Clear interrupt enable INT_EN (0x60)
- Check which input caused the interrupt; in this case, looking for (4R) SIGDET_STAT (0x32)
- Read new 4R data (for example, SIGDET_DC_LEVEL_CH4_R 0x57).
- Set SIGDET_DC_REF_CH4_R (0x55) to be the new value.
- Now that interrupt source is removed, we can clear the SIGDET_STAT register (0x32)
- Write 0xFF to SIGDET_STAT register (0x32).
- Write 0x00 to SIGDET_STAT register (0x32).
- Re-enable control Sense Interrupt in INT_EN (0x60)


## I2C communication:
- **SDA:** Pin 23, I/O I2C data.
- **SCL:** Pin 24, input I2C clock.
- **AD:** Pin 25, input I2C address 1.

- *MST_MODE (0x20):* Set high to select follower I2C mode.

7-bit follower address. The first 6 (MSB) bits of the address are factory set to **1001 01**. The final bit is set by the address pin (AD).

I2C status: The status of I2C can be read from register 0x72 through 0x75 and 0x78. Sample rate, power rail status, clock error, and clock ratios can be read from the status registers.

## Clocking:
**In follower mode, all clocks must be supplied externally.** The chip's master clock (MCK) must be of the form 2^n power of the sampling frequency. The chip's bit clock (BCK) should be running at 64 * sampling frequency. The word clock (left-right clock, LLRCK) determines the sampling point for the ADC. The chip must have its phase lock loop (PLL) programmed to generate audio clocks, but any incoming clock in the range [1MHz, 50MHz] will work.
- 12MHz clock is recommended for 44.1kHz sampling
- 192kHz sampling is only supported if using a CMOS oscillator (we are not using)
- DSP1 and DSP2 should be 256 * sampling frequency

<<<<<<< HEAD
![](/images/clocking.png)
=======
# TODO: investigate using 12MHz clock

![](clocking.png)
>>>>>>> 1a03b37f42354869c9ae66fb4a058da42aa7bd5b

We will be using the ADC in master mode. The PLL will automatically detect for standard audio sampling rates. The clocking modes are shown below:
![](/images/clockingmodes.png)

# TODO: what is our master clock speed? What is our bit clock speed? What is our LLRCK speed? What are DSP1 and DSP2?


---
# Registers:
The registers are split into two usable pages. Page 0 deals with device configuration and page 1 indirectly program coefficients for two fixed DSPs. Page 0 is the focus of this project. Change pages by writing to register 0x00 with the desired page number. Reset registers by writing 0xFE to register 0x00. Page 235 deals with registers that should not be edited, so we will not discuss them.

- 0x01: channel 1L gain. What do we want?
- 0x05: clipping and gain. set gain to all be the same: 11100110
- 0x0b: i2s receive: 00000000. 32 bit word length for single ended and stereo. LRCK 50% duty cycle. I2S format.
- 0x0d: serial data offset: 00000001. setting offset to be i2s.
- 0x0e: serial data offset: 00000001. setting offset to be i2s.
- 0x0f: i have no idea what this means.
- 0x10: GPIO 0-1 control: 00000101. DOUT2 is GPIO0.
- 0x12: GPIO 0-1 direction: 00000100. set GPIO0 as output.
- 0x1a: digital mic. 01010000. set mics to GPIO1 not GPIO0.
- 0x20: clocks. 01010001. use SCK in master mode.
- 0x72: read device status. run mode is 00001111 and power down is 00000000
- 0x73: read current sampling freq: how to lower to 44.1kHz?
- 0x78: read power status






### Input Selection:
Inputs can be mixed using the ADC input selection register *ADCX1_INPUT_SEL_X (0x06 --> 0x09).* Mixing left and right sources to create mono mixes can only be done in the digital mixer, post ADC conversion, or alternatively, other analog inputs can be connected for mixing. Here is a table describing possible mixes. [SE] represents single-ended where [DIFF] represents a differential input.

**ADCX1_INPUT_SEL_X:**
![ADCX1_INPUT_SEL_X](/images/muxmix.png)


### Channel Linking:
<<<<<<< HEAD
Stereo inputs should be linked and tracked across input channels.
![](/images/linking.png)
# TODO: do we want linking for our mono inputs?
=======
Stereo inputs should be linked and tracked across input channels. We are not using stereo audio, so we are not linking.
>>>>>>> 1a03b37f42354869c9ae66fb4a058da42aa7bd5b

### Clock sources:
- *CLK_MODE (0x20):* Clock selection. Bits [5:7] configure the OR and MUX for the master clock (MCLK).
- *MST_MODE (0x20):* Set master or follower mode. Bits [1:3] sets clock sources for ADC, DSP1, and DSP2. *We ignore these, and use the default settings.*
- *MST_SCK_SRC (0x20):* Sets the source of the SCKO in master mode. *We are using the chip in follower mode, so we are ignoring this register.*
- *CLKDET_EN (0x20):* **Set the auto clock detector bit to true. This step is important!**
- PLL configuration: registers 0x28 --> 0x2D
![](/images/pllregisters.png)
- *CLK_ERR_STAT (0x75):* Status of halt and error detector. Error detector is high if there is an unexpected ratio of BCK to LRCK. If an error is detected, the chip is put into standby mode.
- *AUXADC_DATA_CTRL (0x58):* Checks the DC detector as part of the *energysense* and *controlsense*.
- *SEC_ADC_INPUT_SEL (0x0A):* Scanning for thresholds and interrupts in active mode (autodetects in sleep mode). The chip does not auto switch from sleep to wake mode.

### Sense:
We are not using the chip in sleep mode. We will always have the chip in active mode.
- *DSP_CTRL (0x71):* Allows you to choose between FIR response and low-latency IIR response. The high pass filter to remove DC bias is enabled by default.

### Audio Format Selection:
Pin MD4 to select between left justified and I2S.
- *I2S_FMT (0x0B):* Choose between, left justified, right justified, time division multiplexed, and I2S. We are selecting I2S.
- *GPIO1_FUNC (0x10):* Enables second **DOUT** through GPIO1.


---
# PCB Creation:
Our ADC board centers around the PCM1863, but contains other peripherals. This section will discuss the design decisions for the PCM1863 and peripherals.

Ti recommended PCB practices are:
- Separate analog and digital sections where layout permits. Route analog lines away from digital lines. This routing technique prevents digital noise from coupling back into analog signals.
- The bottom copper plane can be a shared ground, whereas a ground plane can be used on the top layer as well. Separated planes for analog and digital grounds are not required to achieve data sheet performance.
- Place decoupling capacitors as close as possible to the supply pins, and in the same layer of the device, to yield the best results. Do not place vias between decoupling capacitors and the device.
- Place ground planes between the input traces to achieve the lowest crosstalk performance.

<<<<<<< HEAD
Here is the current KiCad 3D model for our PCB: ![ADC v.4](/images/pcm1863v4.png)

Here is the current KiCad schematic for our PCB: ![ADC v.4](/images/pcm1863v4schematic.png)
=======
Here is the current KiCad 3D model for our PCB: ![ADC v.4](pcm1863v5.png)

Here is the current KiCad schematic for our PCB: ![ADC v.4](pcm1863v5schematic.png)
>>>>>>> 1a03b37f42354869c9ae66fb4a058da42aa7bd5b

## The PCM1863 chip:
The PCM1863 has 30 pins. ![](/images/pcmpinout.png)

**Of the 30 pins, 12 are analog and 18 are digital.** The chip is split into a digital and an analog part so the noisy digital powers and signals do not interfere with the sensitive analog signals.

![](/images/digitalanalog.png)

---

### Example layouts:
Below are several example circuits given in the Ti datasheet:
![](/images/pcblayoutex.png)

![](/images/testcircuit.png)

![](/images/followermodeex.png)

---
### PCM1863 Pins and components:
This section details decisions made regarding the components connected to the PCM1863.

**Vin:**
The PCM1863 is able to handle 4-channels of analog inputs. There is a right and a left for each input. We are using mono not stereo audio. We are using differential pairs instead of stereo.

**Vbus:** A 5V power supply is used to power the LDOs that power the chip and its peripherals.

### PCM1863 Pinout:
**1  - VINL2/VIN1M:** Differential M input for input 1.
- This pin is connected to ground due to our differential pair being made of single-ended input.

**2  - VINR2/VIN2M:** Differential M input for input 2.
- This pin is connected to ground due to our differential pair being made of single-ended input.

**3  - VINL1/VIN1P:** Differential P input for input 1.

**4  - VINR1/VIN2P:** Differential P input for input 2.

**5  - MIC BIAS:** Microphone biased output
- We are not using Mic Bias, so we are grounding it.

**6  - VREF:** Reference voltage output decoupling point (typically, 0.5 AVDD)
- Connect 1-µF capacitor from this pin to AGND.

**7  - AGND:** Analog ground.
- Connected to GNDD through a ferrite bead.

**8  - AVDD:** Analog power supply (typically, 3.3 V)
- Connect 0.1-µF and 10-µF capacitors from this pin to AGND.

**9  - XO:** Crystal oscillator output

**10 - XI:** Crystal oscillator input or master clock input (1.8-V CMOS signal)

**11 - LDO:** Internal low-dropout regulator (LDO) decoupling output
- Connect 0.1-µF and 10-µF capacitors from this pin to DGND.
- We are using the LDO as a 1.8V output to power I/O devices.

**12 - GDND:** Digital ground.
- Connects to GNDA through a ferrite bead.

**13 - DVDD:** Digital power supply (typically, 3.3 V)
- Connect 0.1-µF and 10-µF capacitors from this pin to DGND.
- Place the smaller capacitor closer to the device.

**14 - IOVDD:** Power supply for I/O voltages (typically, 3.3 V or 1.8 V)
- Connected to DVDD (3.3V)
- # TODO Do I want to connect this to LDO to get 1.8V power for I/O components?

**15 - SCKI:** CMOS level (3.3 V) master clock input

**16 - LRCK:** In I2S mode: Audio data word clock (left right clock) input/output
- Schmitt trigger input has an internal pull-down of 50kOhms

**17 - BCK:** In I2S mode: Audio data bit clock input/output
- Schmitt trigger input has an internal pull-down of 50kOhms

**18 - DOUT:** In I2S mode: Audio data digital output

**19 - GPIO3/INTC:** GPIO 3 or interrupt C

**20 - GPIO2/INTB/DMCLK:** GPIO 2, interrupt B, or digital microphone clock output

**21 - GPIO1/INTA/DMIN:** GPIO 1, interrupt A, or digital microphone input

**22 - MISO/GPIO0/DMIN2:** In I2C mode: *GPIO0*
- Chip is set to I2C not SPI mode.

**23 - MOSI/SDA:** In I2C mode: *SDA* Serial data.
- Chip is set to I2C not SPI mode.

**24 - MC/SCL:** In I2C mode: *SCL* Serial bit clock.

**25 - MS/AD:** In I2C mode: *address pin*. I2C pin and address simultaneously.
- Chip is set to I2C not SPI mode.
- Low: I2C and address 0x94.
- High: SPI and address 0x96.

**26 - MD0:** Control method select pin
- High = SPI
- Low (or unconnected) = I2C

**27  - VINL4/VIN4M:** Differential M input for input 4.
- This pin is connected to ground due to our differential pair being made of single-ended input.

**28  - VINR4/VIN3M:** Differential M input for input 3.
- This pin is connected to ground due to our differential pair being made of single-ended input.

**29  - VINL3/VIN4P:** Differential P input for input 4.

**30  - VINR3/VIN3P:** Differential P input for input 3.

---

### Chip ratings:
**AVDD:** 3.3V

**DVDD:** 3.3V

**IOVDD:** 3.3V

**XI:** 1.8V

**VIN:** -1.7V to 5.0V

**Current:** MAX of 31mA

**Power:** MAX of 117.3mW

### Timing requirements for I2C control interface:
The PCM1863 has two I2C modes: *fast and standard*. If one number is given, the number is the minimum acceptable value. If two values are given, they are given in the form [MIN, MAX] acceptable values. **We will be using standard mode.** Both fast and standard modes follow the same timing constraints as show in the timing diagram:
![](/images/i2ctiming.png)

***STANDARD MODE:***

**cB:** Capacitive load for SDA and SCL lines - MAX 400pF

**vNH:** Noise margin at high level for each connected device - 0.2VDD

**fSCL:** SCL clock frequency - MAX 100kHz

**tBUF:** Bus free time between STOP and START conditions - 4.7us

**tLOW:** Low period of the SCL clock - 4.7us

**tHI:** High period of the SCL clock - 4us

**tRS-SU:** Setup time for repeated START condition - 4.7us

**tS-HD:** Hold time for START condition - 4us

**tRS-HD:** Hold time for repeated START condition - 4us

**tD-SU:** Data setup time - 250ns

**tD-HD:** Data hold time - [0, 900ns]

**tSCL-R:** Rise time of SCL signal - [(20 + 0.1cB)ns, 1000ns]

**tSCL-R1:** Rise time of SCL signal after a repeated START condition and after an acknowledge bit- [(20 + 0.1cB)ns, 1000ns]

**tSCL-F:** Fall time of SCL signal - [(20 + 0.1cB)ns, 1000ns]

**tSDA-R:** Rise time of SDA signal - [(20 + 0.1cB)ns, 1000ns]

**tSDA-F:** Fall time of SDA signal - [(20 + 0.1cB)ns, 1000ns]

**tP-SU:** Setup time for STOP condition - 4us

**tSP:** Pulse duration of spike suppressed -  MAX 50ns

---

**Timing information for follower mode:**

*We will be using follower mode.*
![](/images/timingfollowermode.png)

# TODO: can we do 44.1kHz sampling?
![](/images/plltimings.png)

---

### PLL Information:
The PLL is enabled by default to clock the fast DSPs.
- **PLLCK = (PLLCKIN * R * JD) / P**
- **PLLCK = (PLLCKIN * R * K) / P**

Where:
- R, J, D, and P are register programmable values
- **J:** is the integer part of K
- **D:** is the fractional part of K to *four digits of precision*

![](/images/pllexamples.png)

### Clock error procedure:
When a clock error is detected, the chip goes into standby mode. The steps are as follows:
- Mute audio output immediately (no ramp down)
- Wait until proper clock ratio returns (*Clock Waiting State*)
- Restart clock detection, PLL, and clock dividers
- Start output fade in

![](/images/clkerrorlogic.png)
![](/images/clkerrorlogic2.png)

Clock error detection can be ignored by disabling *CLKDET_EN*.

### Secondary ADC:
We are always using the chip in active mode, so the secondary ADC is not used in wake functions.

### DSPs:
DSP1 and DSP2 are fixed function processors. DSPs can be programmed by targeting their memory address.

### PCM1863 Suggested landing pattern:
The Ti suggested landing pattern for the chip and its TSSOP30 package:
<<<<<<< HEAD
![](/images/soldermaskex.png)


- ![](/images/i2cex.png)
- ![](/images/3.3vex.png)

## Peripherals;
=======
![](soldermaskex.png)
>>>>>>> 1a03b37f42354869c9ae66fb4a058da42aa7bd5b
message.txt
19 KB
