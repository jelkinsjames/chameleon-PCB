# PCM1863 Usage

Once the board is complete and [setup](https://github.com/jelkinsjames/chameleon-PCB/tree/main/PCM1863/setup) with the Le Potato, the device can be used. There are several functionalities that can be used with the board:
- Software setup
- Detecting I2C devices
- Reading and writing registers
- Initial register settings
- I2S output

---

## Software setup
The ADC is an I2C compatible device so it can communicate with the Le Potato through I2C.

### Installing I2C development library
To read and write register with the Le Potato we use I2C. To use I2C, we need to install the I2C development library `libi2c-dev`.

```bash
sudo apt install libi2c-dev
```

Navigate to the `i2c` directory in the `potato-recipes` directory. Compile the read and write files by running:

```bash
gcc read.c -li2c -o read
gcc write.c -li2c -o write
```

### Installing device drivers
Le Potato comes with device drivers preinstalled. A list of the sound drivers can be seen by running the command from the `root` directory:

```bash
ls /lib/modules/6.0.12*/kernel/sound/soc/codecs/
```

The `snd-soc-pcm186x-i2c` driver can be used for the PCM1863. The driver can be installed with `modprobe` by running the command in any directory:

```bash
sudo modprobe snd-soc-pcm186x-i2c
```

The full documentation for the drivers in the Linux Kernel is linked [here](https://github.com/torvalds/linux/tree/master/sound/soc/codecs).

---

## Detecting
Before you read and write to the ADC, you should check to see if your device is connected and identifiable as an I2C device.

If you run the command to check I2C channels from the `root` directory, you should see three channels: `i2c-0`, `i2c-1`, and `i2c-2`:

```bash
ls /dev/i2c*
```

To check what is on each channel, you can use `i2cdetect` from any directory. Including the `-l` will show a list of what is on each channel:

```bash
i2cdetect -l
```

You should see an output similar to:

```bash
i2c-0	unknown   	DesignWare HDMI                 	N/A
i2c-1	unknown   	Meson I2C adapter               	N/A
i2c-2	unknown   	Meson I2C adapter               	N/A
```

The 0 channel should be the HDMI and the 1 and 2 channels are Le Potato I2C communication channels.

To see what is connected on each channel, you can run `i2cdetect` again but specify the channel you want. To look at what is on channel 1 (the channel associated with Le Potato I2C pins 3 and 5), you can run:

```bash
sudo i2cdetect -y 1
```

The result should be the address of the connected device. The ADC v.4 had an address of 4a, so the result was:

```bash
0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:                         -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- 4a -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --  
```

If your result is all `--`, there is something wrong and there is no detectable connected device on the channel. Debugging and troubleshooting is required.

---

## Reading and writing
Once the necessary software is installed and prepared, you can read and write to registers using I2C communication. Before reading and writing, you have to check if your device is connected (detecting).

### Commands to run every time the Le Potato reboots
Currently, every time the Le Potato reboots (the power is turned off), you will have to run the commands listed below to add the Le Potato I2C channels. These commands should be run in the `libretech-overlays` directory.

```bash
sudo ./overlay.sh add libretech-cc-i2c-ao
sudo ./overlay.sh add libretech-cc-i2c-b
```

### Reading and writing to registers
Once the ADC is detectable on an I2C channel, you can read and write to PCM1863 registers using the `i2c` functions in the `potato-recipes` directory.

To read from a register on the first channel, run the program `read`. To read the contents of register 0x20 (program will work with hexadecimal, decimal, and binary):

```bash
sudo ./read 0x20
```

If the ADC is functional, you should see the contents of the register:

>Register 0x20, 00000001

To write to a register on the first channel, run the program `write`. To set the contents of register 0x20 to 00000000:

```bash
sudo ./write 0x20 00000000
```

The contents of the registers 20 should be changed from 00000001 to (=>) 00000000:

>Register 20: 0x01, 00000001 => 0x00, 00000000

---

## Initial register settings
The first step is to check the status of the registers.

```bash
sudo ./read 0x72
sudo ./read 0x72
sudo ./read 0x78
```

- Register 0x72 should read 00000000: board is powered down (default)
- Register 0x73 should read 00000111: invalid sampling frequency (no clocks have been set up yet)
- Register 0x78 should read 00000111: DVDD, AVDD, and LDO are functional

If the status registers are good, you can set up the clocks and enable the device using register 0x20:

```bash
sudo /write 0x20 01010001
```

 The bits mean in order from left to right:
- 01 clock = SCK
- 0 source external
- 1 chip is primary for I2S
- 0 ADC clock = SCK
- 0 DSP2 clock = SCK
- 0 DSP1 clock = SCK
- 1 enable  

Once you write to the setup register, you should check the status of the registers again.

```bash
sudo ./read 0x72
sudo ./read 0x72
sudo ./read 0x78
```

- Register 0x72 should read 00001111: board is powered down (default)
- Register 0x73 should read 00000100: frequency range is
- Register 0x78 should read 00000111: DVDD, AVDD, and LDO are functional


## Commands for Julian:

The order of the commands is important and correct. After logging into the lab potato, run these commands in order (you can SSH to copy and paste if you want).

```bash
cd libretech-overlays
sudo ./overlay.sh add libretech-cc-i2c-ao
sudo ./overlay.sh add libretech-cc-i2c-b
cd ~
cd potato-recipes
cd i2c
sudo ./write 0x20 01010001
sudo ./write 0x0d 00000001
sudo ./write 0x0e 00000001
sudo ./write 0x0b 01000100
```
