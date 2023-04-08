# PCM1863 ADC

The purpose of the ADC in the system is to take analog inputs from control voltages (5V) and convert them into readable I2S digital signals. The digital signals will be processed by the embedded board to be modified by [YASE](https://github.com/klavins/yase) (a synthesizer engine created by Eric Klavins).

The ADC creation has three steps:
1. [Creating the physical ADC board](https://github.com/jelkinsjames/chameleon-PCB/tree/main/PCM1863/board)
2. [Setting up the ADC board and embedded board](https://github.com/jelkinsjames/chameleon-PCB/tree/main/PCM1863/setup)
3. [Using the ADC with the embedded board](https://github.com/jelkinsjames/chameleon-PCB/tree/main/PCM1863/usage)

We will be using the PCM1863 chip by TI for our ADC. The [PCM1863](https://www.ti.com/product/PCM1863?utm_source=google&utm_medium=cpc&utm_campaign=asc-null-null-GPN_EN-cpc-pf-google-wwe&utm_content=PCM1863&ds_k=PCM1863&DCM=yes&gclid=Cj0KCQiA6fafBhC1ARIsAIJjL8l6Lk9SO-Q0-fB0_5uYiveTayMeogbHzMJUxCBsQcNd4n_-5XY2ie4aAl2cEALw_wcB&gclsrc=aw.ds) is an audio-controlled analog-to-digital (ADC) converter. The chip is 110dB 2-channel software controlled. The chip is made by Texas Instruments (TI), and its datasheet is available here: [PCM1863 Datasheet](https://www.ti.com/lit/ds/symlink/pcm1861-q1.pdf?HQS=dis-dk-null-digikeymode-dsf-pf-null-wwe&ts=1676585219889&ref_url=https%253A%252F%252Fwww.ti.com%252Fgeneral%252Fdocs%252Fsuppproductinfo.tsp%253FdistId%253D10%2526gotoUrl%253Dhttps%253A%252F%252Fwww.ti.com%252Flit%252Fgpn%252Fpcm1861-q1). The chip is a TSSOP (DBT) 30 pin package. The chip is hardware and software controllable.

Drivers for the PCM 186x series are available [here](https://e2e.ti.com/support/audio-group/audio/f/audio-forum/773056/faq-linux-drivers-device-drivers-for-aic31xx-dac31xx-aic325x-aic320x-aic326x-aic321x). More driver information can be found on the [TI webpage](https://www.ti.com/product/PCM1863?utm_source=google&utm_medium=cpc&utm_campaign=asc-null-null-GPN_EN-cpc-pf-google-wwe&utm_content=PCM1863&ds_k=PCM1863&DCM=yes&gclid=Cj0KCQiA6fafBhC1ARIsAIJjL8l6Lk9SO-Q0-fB0_5uYiveTayMeogbHzMJUxCBsQcNd4n_-5XY2ie4aAl2cEALw_wcB&gclsrc=aw.ds) for the PCM1863.

The [Ti PCM186X Evaluation Board](https://www.ti.com/lit/ug/slau615/slau615.pdf?ts=1678288360067&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FPCM1864) has additional resources on how to design an example board. [PurePath Console 3 (PPC3)](https://www.ti.com/tool/PUREPATHCONSOLE) is a GUI for development of audio chips such as the PCM1863

Here is a block diagram of the chip we are using:

![](/chameleon-pcb/assets/images/blockdiagram.png)

---
