# PCM1863 Setup

Once you have a functional board, you need to use it. The steps are:
- Installing the necessary packages on your Linux embedded device
- Setting up the physical Potato and ADC
- Running commands to prep embedded device for I2C communication *(required every time)*
- Setting up the ADC
- Using the ADC

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

---

## Setting up Le Potato I2C communication
