[![build](https://img.shields.io/travis/amq/tiva_oled_joystick.svg)](https://travis-ci.org/amq/tiva_oled_joystick)
[![analysis](https://img.shields.io/coverity/scan/11388.svg)](https://scan.coverity.com/projects/amq-tiva_oled_joystick)

Building
```
export GCC_DIR="$HOME/gcc-arm-none-eabi-6_2-2016q4"
export XDCTOOLS_DIR="$HOME/ti/xdctools_3_32_00_06_core"
export TIRTOS_DIR="$HOME/ti/tirtos_tivac_2_16_01_14"
make
```

Flashing
```
export UNIFLASH_DIR="$HOME/ti/uniflash_4.1"
sudo bash $UNIFLASH_DIR/dslite.sh --config TargetConfiguration.ccxml tiva_oled_joystick.out
```

Prerequisites
- [gcc-arm-none-eabi >= 4.9](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm)
- [xdctools >= 3.32](http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/tirtos/index.html)
- [tirtos_tivac >= 2.16](http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/tirtos/index.html)
- [uniflash >= 4.1](http://www.ti.com/tool/UNIFLASH)

Documentation
- [TI-RTOS User's Guide](http://www.ti.com/lit/ug/spruhd4m/spruhd4m.pdf)
- [SYS/BIOS (TI-RTOS Kernel) User's Guide](http://www.ti.com/lit/ug/spruex3q/spruex3q.pdf)

Image and font conversion was done using [LCD Image Converter](https://github.com/riuson/lcd-image-converter)
