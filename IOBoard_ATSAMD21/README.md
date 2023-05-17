# I/O Board - ATSAMD21G18-A

---

This repository contains the hardware and software components of the I/O board for the Chameleon project.
The goal for this portion of the project is to create a peripheral to control [YASE](https://github.com/klavins/YASE). It can interface with any CPU using the I2C bus, and will allow users to modify parameters in YASE. The I/O board has rotary encoders, switches, and LEDs that the user can configure using I2C commands.

---

## Hardware

#### Rotary Encoders
To allow users to modify YASE parameters with a knob, we are using rotary encoders. While rotary encoders may appear visually similar to a potentiometer you may find on a guitar or analog synthesizer module, they are fundamentally different. Instead of behaving like a variable resistor, rotary encoders are more similar to a rotary, dual-pole, dual-throw switch. Rotary encoders also spin indefinitely and do not have a start and end like a potentiometer.

The [How To Mechatronics](https://howtomechatronics.com/tutorials/arduino/rotary-encoder-works-use-arduino/) website has a great tutorial and explanation for how rotary encoders work.

A rotary encoder has 3 pins. The middle pin (C) is a shared power pin, which is connected to a power source. As the encoder spins, a brush powered by the middle pin makes contact with the left (A) and right (B) pins in sequence.

<p align = "center">
  <img src = "figs/rotaryencoderexample.png">
</p>

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

#### PCB Design

The biggest design consideration made for the PCB was making it compatible with other Eurorack modules. Eurorack modules have the following size constraints:
- Height - 128.5mm/5.06"
- Width - in increments of 0.2", called a Horizontal Pitch (HP)
- Mounting holes - 3.2mm diameter
- Left mounting holes aligned 3mm from top and bottom edge, and 7.45mm from the left edge
- Right mounting holes aligned in HP increments from the left holes

For the IO board, I chose a width of 15HP. I found this size to be a good width for fitting all the components we needed. The mounting holes are 13HP apart, as this way there was plenty of clearance from the hole to right edge.

I placed the rotary encoders, LEDs, and switches on a grid of 0.1" so that I could make them symmetrical across the middle of the board. I used a smaller grid for the ATSAMD21G18 and bypass capacitors to make routing easier. The ATSAMD21G18 has both internal pull up and pull down resistors, so there was no need to include extra resistors on the board, except for the resistors used for the LEDs. I found 10k ohms to yield a good brightness for the LEDs in testing.

For power, I am using a 3.3v LDO. I chose this instead of stealing power from the main computer in hopes that it may be a little more stable and allow for  expansion in the future.

Next to every power pin on the SAMD21, I included two bypass capacitors. I chose 0.1uF and 10uF capacitors as it is typically recommended the larger bypass capacitor are 100x the smaller's capacitance. Ideally, I would have used both a tantalum and a ceramic capacitor for better performance, but as this board is purely digital I am not overly concerned with noise. Ceramic capacitors are not polar, so it is a little easier to assemble the boards using both ceramic.

All capacitors and resistors use the 0603 package for consistancy.

The entire back copper layer of my PCB is a ground plane. This makes it very easy to ground anything that I need by placing a via. Once all of my components were connected to the ATSAMD21G18, I made the rest of the front copper layer a +3.3v power plane. This meant I did not have to route any power traces on the board, vastly simplifying the amount of time it took to design the PCB.

The switches are single throw, dual pole. This choice was made largely because they are much cheaper than single throw, single pole switches and readily available. I connected the middle pin to power, and the top pin to the SAM's GPIO.

#### Programming/Debugging

I use the Microchip PICkit4 to debug/program the SAMD21. I used the recommended debug header provided by KiCad 6. While this seemed to be easy initially, I realized that there was no easily accessible documentation as to what the PICkit4's 8 pins do! Different resources were saying different things, so it required a little guess and check work. To make matters worse, for some silly reason pin 1 of the PICkit4 is the furthest **RIGHT** instead of furthest left like every other convention you might be used to. The only clue I had to this was the arrow pointing at that pin.

To make things much easier for you, here is a list of the PICkit4 pins and where to plug them into the IO Board (from right to left)

1. MCLR
2. VDD
3. GND
4. Not connected
5. SWCLK
6. Not connected  
7. Not connected
8. SWDIO

The recommended header I used on my PCB has this in the same order, except for SWCLK and SWDIO, which are switched. I may change this in the future, but connecting two wires in a slightly different order is not a huge pain.

## Software

Programming the ATSAMD21G18 is fairly straight forward (with a few exceptions mostly due to Microchip's bad documentation). I am using the PICkit4 debugger to program the SAM. To write the code, I initially thought I may be able to use the Arduino IDE. This which would vastly simplify the programming, and may still be possible. The Arduino IDE supports the SAMD21, however it does not have support for the PICkit4. It supports the Atmel ICE debugger, which no longer exists since Microchip bought Atmel and now uses the PICkit4 as it's preferred debugger even though the ATSAM family of chips were originally produced by Atmel. *Can you tell I'm annoyed about this?*

I'm sure it is possible to trick the Arduino IDE into thinking that the PICkit4 is an Atmel ICE, but I do not know how to do that.

Instead, I am using Microchip's [MPLAB X IDE](https://www.microchip.com/mplab) to program my SAMD21. In true Microchip fashion, this turned out to be way more work than I wanted it to be! My goal was to use the Harmony3 tool to assist with configuring the chip. This is supposed to be installed by default, but for some reason it was not.

I had to clone the [GitHub repository for the MPLAB Harmony Content Manager](https://github.com/Microchip-MPLAB-Harmony/contentmanager), run it from the terminal, and use that to install the required packages for Harmony3. Once all of that was finished, I was finally able to use Harmony to write most of the code I needed. While this certainly saved me from reading the datasheet for hours and hours to learn how to program the chip, it led to the problem of me not understanding how the code worked at all!

#### SERCOM I2C Functionality
My first big mistake was not enabling smart mode for the SERCOM I2C module inside Harmony3. Smart mode automates most of the I2C transactions, which greatly simplifies the coding. Before I realized that I should be using smart mode, I ran into a very strange issue with the address match interrupt. The code generate by Harmony3 included an interrupt manager. For whatever reason, the interrupt manager would not reset the address match interrupt and instead would get stuck in a time-out state. It took me weeks to figure out what was going on, and in the end it just took me turning smart mode on and Harmony rewrote the interrupt manager to deal with the issue I was having.

Once smart mode was enabled, all that is left to do is write a callback function for the SERCOM I2C module. Using a switch statement allows you to choose desired behavior when certain events take place. For my code, I only care about receiving and sending data, and do not care about doing anything specific on address match or on an error. A basic form of this callback function is shown below.

```C
uint8_t rxReg;
uint8_t txReg;
bool I2C_Callback(SERCOM_I2C_SLAVE_TRANSFER_EVENT event, uintptr_t contextHandle) {
    switch(event) {
      case SERCOM_I2C_SLAVE_TRANSFER_EVENT_RX_READY:
          rxReg = SERCOM0_I2C_ReadByte();
          // You can execute any code you want here, I use another switch statement
          switch(rxReg) {
            case 0x00:
              ENC0_val = 0;
              txReg = ENC0_val;
              break;
            case 0x01:
              txReg = ENC0_val;
              break;
          }
        break;
      case SERCOM_I2C_SLAVE_TRANSFER_EVENT_TX_READY:
          SERCOM0_I2C_WriteByte(txReg);
          txReg = 0;
        break;
    }
    return true;
}
```

For this project, we will always read a value before writing a value. The value we write is determined by the value previously read. Because of this, I fill `txReg` with the correct data based on what we read from the I2C host. For example, if the host wishes to read the value of `ENC0`, it will first write `0x01` to `rxReg`. We next check the value of `rxReg` and enter the correct case statement. As shown above we just fill `txReg` with `ENC0_val`.

Once this host write operation has finished, the host will initiate a read operation. The IOBoard will respond with the contents of `txReg` and then reset the value to 0. My callback function always returns `true`. If you return false instead, the I2C acknowledgement bit will not be set high. This can be useful to tell the host that data is not ready for transmission. For my application there are no cases in which we wont send valid data, so I always return `true`.

The events you can add to your callback function are as follows:
- `SERCOM_I2C_SLAVE_TRANSFER_EVENT_RX_READY`
- `SERCOM_I2C_SLAVE_TRANSFER_EVENT_TX_READY`
- `SERCOM_I2C_SLAVE_TRANSFER_EVENT_ADDR_MATCH`
- `SERCOM_I2C_SLAVE_TRANSFER_EVENT_STOP_BIT_RECEIVED`
- `SERCOM_I2C_SLAVE_TRANSFER_EVENT_ERROR`

Now that we have a completed callback function, we just need to tell the SERCOM module to use this function by calling `SERCOM0_I2C_CallbackRegister(I2C_Callback, 0)` in our main function.

#### Talking to the IOBoard
The following is a full list of all values you can set and read from the IOBoard. Sending an I2C read command of the following 'registers' will return the corresponding values.
- Encoder 0, reset `0x00`, read value `0x01`
- Encoder 1, reset `0x10`, read value `0x11`
- Encoder 2, reset `0x20`, read value `0x21`
- Encoder 3, reset `0x30`, read value `0x31`
- All switches, read `0x40`, least significant 4 bits are correspondent to state of each switch in order SW3, SW2, SW1, SW0. For example, the returned value 0x09 would mean that switch 3 and switch 0 are on.
- All buttons, read `0x50`, same data format as the switches. When read, these values are reset to 0.
