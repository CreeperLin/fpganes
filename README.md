# fpganes
NES on FPGA template project

## Components

- asm: some 6502 assembly code
- ctrl: controller, used to load ROM and emulate joysticks via UART
- roms: test NES ROMs in iNES format
- serial: submodule used to control UART port
- src: NES on FPGA implementation

## Usage
ctrl:

first clone the serial library and build

    git submodule update
    cd serial
    make
    make install
requirements see [wjwwood/serial](https://github.com/wjwwood/serial)

then build the controller

    cd ctrl
    ./build.sh
run the controller

    ./run.sh path/to/rom path/to/uart_port

