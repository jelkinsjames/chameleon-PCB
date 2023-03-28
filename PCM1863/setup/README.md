# PCM1863 Setup

Once you have a functional physical ADC board, you need to set it up. The steps are:
- Installing the necessary packages on your Linux embedded device
- Setting up the physical Potato and ADC
- Running commands to prep embedded device for I2C communication

---

## Getting the Le Potato up and running
This section uses information from Eric Klavins's GitHub repositories. His repository on installing Linux is linked [here](https://github.com/klavins/potato-recipies/blob/main/big-linux/README.md). Building off the repo, this sections details how to get a [Le Potato](https://libre.computer/products/aml-s905x-cc/) up and running to use with the PCM1863 based ADC.



### Getting a Linux distro
In order to interface with the ADC, Le Potato needs an operating system. We are using Linux. A downloadable distribution of Linux that is compatible with Le Potato is provided below. The website with similar Ubuntu 22.04 distributions is linked [here](https://distro.libre.computer/ci/ubuntu/22.04/).

> [ubuntu-22.04.1-preinstalled-desktop-arm64+aml-s905x-cc.img.xz](https://distro.libre.computer/ci/ubuntu/22.04/ubuntu-22.04.1-preinstalled-desktop-arm64%2Baml-s905x-cc.img.xz)

In order to get the distribution of Linux on Le Potato, you have to flash it. we are using [Balena Etcher](https://www.balena.io/etcher#download-etcher) to flash our distro. There are two options, "Flash from file" or "Flash from URL." Flashing from a URL will save you time, so we recommend it. Copy the *https://distro.libre.computer/ci/ubuntu/22.04/ubuntu-22.04.1-preinstalled-desktop-arm64%2Baml-s905x-cc.img.xz* URL into the "Use Image URL." Select a target to flash your distro to (your MicroSD). Select "Flash!" and wait for the etcher to finish.

Once completed, eject the MicroSD card. Your MicroSD card now contains a distribution of Linux to run on Le Potato!

Note: you may have to format and partition your MicroSD. We recommend using the *Window's Disk Manager* to create partitions and format MicroSD cards.

### Turning on Le Potato for the first time
Once you have a distribution of Linux, you can start to get your Le Potato working.

First, plug in your MicroSD card and then power the Le Potato. **Make sure you insert the MicroSD before powering the device on.**

Your Le Potato should start booting. The initial boot may take a few minutes - do not unplug the Le Potato during this time. If you are having difficulties booting, try removing any connected USB devices, then boot again.

Complete the initial setup of your device by creating an account and filling out the startup popups.

Once you complete the startup popups, navigate to the terminal and check to make sure your distribution of Linux has a directory called `device-tree`. To check if you have the directory, run:

```bash
ls /sys/kernel/config/
```

### Running Le Potato in console mode
The Le Potato does not have enough computing power to run the Ubuntu GUI well. Open the terminal and *change the Le Potato to console mode* by running:

```bash
sudo init 3
```
To permanently change the Le Potato to run in console mode run the command:

```bash
sudo systemctl set-default multi-user.target
```

Your Le Potato now has a functional distribution of Linux that it can use to interact with the ADC. If you prefer to SSH into your Le Potato, Eric Klavins has a tutorial section on SSHing linked [here](https://github.com/klavins/potato-recipies/blob/main/big-linux/README.md).

---

## Setting up the physical ADC
In order to talk to the ADC, the Le Potato needs to be connected to it. The pinout for the Le Potato is linked [here](https://docs.google.com/spreadsheets/d/1U3z0Gb8HUEfCIMkvqzmhMpJfzRqjPXq7mFLC-hvbKlE/edit#gid=0).

#### Power pins
The first step is to ground the ADC by connecting the Le Potato and ADC to a common ground. Connecting the devices' grounds to each other is sufficient for most applications.

Once the VCC adapters arrive, the device will be powered by a 5V USB connection. For now, the device can be powered by connecting pins 2 or 4 on the main header or pin 3 on the three pin header of the Le Potato to the `VCC` pin on the ADC.

If all four LEDs light up, you are good to move on. If all four LEDs do not light up, there may be a problem with the physical board. If either of the 3.3V power LEDs are not on, you can connect 3.3V from the Le Potato pins 1 or 17 on the main header or pin 8 of the eight pin header.

#### I2C pins
I2C communication consists of two parts - data and clock. On the Le Potato, pin 3 is `I2C_SDA_AO` for I2C data on channel A and pin 5 is `I2C_SCK_AO` for I2C clock on channel A. We use channel A, but you can also use pin 27 as `I2C_SDA_B` for I2C data on channel B and pin 28 as `I2C_SCK_B` for I2C clock on channel B.

Regardless of the channel, the I2C data pin on the Le Potato connects to the `SDA` pin on the ADC. ADC v.4 is mislabeled, and the `MOSI` pin is the `SDA` pin.

The I2C clock pin on the Le Potato connects to the `SCK` pin on the ADC. ADV v.4 is mislabeled, and the `MC` pin is the `SCK` pin.

---

## Setting up Le Potato I2C communication
Once the Le Potato is connected to the ADC, the Le Potato needs to talk to the ADC. The Le Potato can talk to the ADC using a communication protocol called I2C. I2C has a data and a clock line. The pinout for the Le Potato is linked [here](https://docs.google.com/spreadsheets/d/1U3z0Gb8HUEfCIMkvqzmhMpJfzRqjPXq7mFLC-hvbKlE/edit#gid=0).

The Le Potato has two I2C channels that share a single I2C bus. The first channel is i2c-ao and uses pins 3 and 5. The second channel is called i2c-b and uses pins 27 and 28. Linux is not able to access these channels. You can check the status by running:

```bash
ls /dev/i2c*
```

The command should only show `i2c-0` - the connection from the HDMI port.

To make the other two channels visible to Linux, we follow the [LibreTech Recipe](https://github.com/libre-computer-project/libretech-overlays). LibreTech is the company that makes Le Potato.

```bash
git clone https://github.com/libre-computer-project/libretech-overlays.git
cd libretech-overlays
make
sudo cp -r overlays/*.dtbo /lib/firmware
sudo ./overlay.sh add libretech-cc-i2c-ao
sudo ./overlay.sh add libretech-cc-i2c-b
```

If you run the command to check I2C channels again, you should see three channels: `i2c-0`, `i2c-1`, and `i2c-2`.

```bash
ls /dev/i2c*
```

Now that Linux recognizes the channels, we can see what devices are on the channels by running:

```bash
i2cdetect -l
```

After running the command, you should see this (or a similar) return:

```bash
i2c-0	unknown   	DesignWare HDMI                 	N/A
i2c-1	unknown   	Meson I2C adapter               	N/A
i2c-2	unknown   	Meson I2C adapter               	N/A
```

The two *Meson I2C adapter* channels are the Le Potato I2C channels.

### Testing I2C setup
Following all of the above steps should yield a Le Potato that can interact with the ADC. To check if the setup was completed correctly, you can check the I2C channel of a connected I2C device.

Because we setup our Le Potato to talk over channel one, we will run a command to show the address of any devices connected to the I2C line:

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

### Commands to run every time the Le Potato reboots
Currently, every time the Le Potato reboots (the power is turned off), you will have to run the commands listed below to add the Le Potato I2C channels. These commands should be run in the `libretech-overlays` directory.

```bash
sudo ./overlay.sh add libretech-cc-i2c-ao
sudo ./overlay.sh add libretech-cc-i2c-b
```
