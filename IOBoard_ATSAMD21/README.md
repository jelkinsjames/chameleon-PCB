# I/O Board - ATSAMD21G18-A

---

This repository contains the hardware and software components of the I/O board for the Chameleon project.
The goal for this portion of the project is to create a peripheral to control [YASE](https://github.com/klavins/YASE). It can interface with any CPU using the I2C bus, and will allow users to modify parameters in YASE. The I/O board has rotary encoders, switches, and LEDs that the user can configure using I2C commands.

---

## Hardware

#### Rotary Encoders
To allow users to modify YASE parameters with a knob, we are using rotary encoders. While rotary encoders may appear visually similar to a potentiometer you may find on a guitar or analog synthesizer module, they are fundamentally different. Instead of behaving like a variable resistor, rotary encoders are more similar to a rotary, dual-pole, dual-throw switch. Rotary encoders also spin indefinitely and do not have a start and end like a potentiometer.

A rotary encoder has 3 pins. The middle pin (C) is a shared power pin, which is connected to a power source. As the encoder spins, a brush powered by the middle pin makes contact with the left (A) and right (B) pins in sequence.

![Rotary encoder timing example](IOBoard_ATSAMD21/figs/rotaryencoderexample.png)

Since A and B are sent high at slightly different times, we can determine the direction of spin by measuring the state of A and B when a change in one is detected. This task is surprisingly simple to code.
Initially, we sample the value of `A` as value `aLast`. In a loop, we check to see if `aLast == A`. If `A != aLast`, we check the value of `B`. If `B == A`, we know `B` changed before `A` and the knob has been turned to the left. If `B != A`, then we know `A` changed first and the knob was turned to the right. Sample C code for managing one encoder is shown below.

```C
uint8_t encVal = 0;
_Bool aLast = A;
while(1) {
  if (A != aLast) {
    if(A != B) {
      encVal++;
    } else {
      encVal--;
    }
  }
  aLast = A;
}
```

I chose to use the PEC12R-3217F-S0024-ND rotary encoders because other similar projects used them. They include an extra switch that goes high when you press down on the knob. As an added bonus, they make a very satisfying clicking sound when pressed.

#### Microcontrollers

The state of the encoders, LEDs, and switches are managed by a microprocessor. Initially I prototyped using the Arduino UNO, due its ease of use. However, the ATMEGA328P microcontroller it uses does not have enough GPIO to support the amount of peripherals we decided to use.

We have 4 rotary encoders, 4 switches, and 4 LEDs. Each encoder uses 3 GPIO (2 for the encoder and one for the pushbutton switch). Each switch and LED uses one GPIO. On top of this, I need two pins for accessing the I2C bus. This totals 22 GPIO. I could have used two microcontrollers, however choosing one that has enough GPIO to begin with seemed like a smarter idea so that I wouldn't have to write two separate programs.

This GPIO requirement led me to the ATSAMD21G18, an ARM-based microcontroller used on the Arduino Zero. This microprocessor is honestly way more powerful than we will take advantage of, but they are relatively cheap, widely available, and have more than enough GPIO. This will allow expansion in the future if a different team decides they want to add something else!