# openbmc-stm32mpu

This repository aims to bring STM32MPU support into [OpenBMC project](https://github.com/openbmc/openbmc).

This **main** branch will remain empty. Please be sure to target the branch corresponding to the *Yocto version* you want to use, or the [*OpenSTLinux*](https://wiki.st.com/stm32mpu/wiki/OpenSTLinux_distribution) tag on which the meta-st is based on.

## Disclaimer

The Yocto layer distributed here guarantees only minimal support for STM32MPU platforms within the OpenBMC project. Configuration and interconnection with the host chip and chassis must be done entirely by the user. For further information, please refer to the official OpenBMC project documentation.

## Versioning 

For each release, you can identify the version by the different information below :
* **Branch** : the name of the Yocto version used.
* **Tag ST** : the version of OpenSTLinux used as basis for *meta-st*.
* **Tag OpenBMC** : the version of OpenBMC project release.