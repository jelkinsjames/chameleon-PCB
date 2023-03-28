# PCM1863 Usage

Once the board is complete and [setup](https://github.com/jelkinsjames/chameleon-PCB/tree/main/PCM1863/setup) with the Le Potato, the device can be used. There are several functionalities that can be used with the board:
- Reading and writing registers
- Initial register settings
- I2S output

---

## Reading and writing to registers
The ADC is an I2C compatible device so it can communicate with the Le Potato through I2C.

### Commands to run every time the Le Potato reboots
Currently, every time the Le Potato reboots (the power is turned off), you will have to run the commands listed below to add the Le Potato I2C channels. These commands should be run in the `libretech-overlays` directory.

```bash
sudo ./overlay.sh add libretech-cc-i2c-ao
sudo ./overlay.sh add libretech-cc-i2c-b
```

# TODO: Finish writing how to for read and write in lab
