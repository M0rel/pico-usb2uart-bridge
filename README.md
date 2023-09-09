# Raspberry Pi Pico USB2UART Bridge

## Setup

Functionality was tested using WSL.

### Environment setup

Install required dependencies

``` bash
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
```

### Sources obtaining

Clone the main project with a pico-sdk and tinyusb as submodules.

``` bash
git clone git@github.com:M0rel/pico-usb2uart-bridge.git
git submodule init 
git submodule update
```

### Project build

From a root folder run the following command:

``` bash
cmake -B build -S . && make -C build/
```

### Deploy into target

Copy a file (pico-usb2uart-bridge.uf2) from build folder to the raspberry pico file-system


