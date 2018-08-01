# fpganes
NES on FPGA template project

**Fork first**
## Components

- asm: some 6502 assembly code
- ctrl: controller, used to load ROM and emulate joysticks via UART
- roms: test NES ROMs in iNES format
- serial: submodule used to control UART port
- src: NES on FPGA implementation

## Usage
ctrl:

First clone the serial library and build

    git submodule update
    cd serial
    make
    make install
requirements see [wjwwood/serial](https://github.com/wjwwood/serial)

Build the controller

    cd ctrl
    ./build.sh
Run the controller

    ./run.sh path/to/rom path/to/uart_port

The UART port should look like:

Linux: /dev/ttyUSB1
Windows: COM3

## Debug packet definition
Echo

    OP: 0x00
    number_of_bytes (word)
    bytes_echoed

CPU Memory Read

    OP: 0x01
    MEM_ADDR (word)
    BYTE_COUNT (word)
    BYTES
CPU Memory Write

    OP: 0x02
    MEM_ADDR (word)
    BYTE_COUNT (word)
    BYTES
Break

    OP: 0x03
Run

    OP: 0x04
CPU Register Read

    OP: 0x05
    REG_SELECT
CPU Register Write

    OP: 0x06
    REG_SELECT
    REG_VALUE
Query Break

    OP: 0x07
Query Error Code

    OP: 0x08
PPU Memory Read

    OP: 0x09
    MEM_ADDR (word)
    BYTE_COUNT (word)
    BYTES
PPU Memory Write

    OP: 0x0A
    MEM_ADDR (word)
    BYTE_COUNT (word)
    BYTES
PPU Disable

    OP: 0x0B
Cartridge Config

    OP: 0x0C
    iNES byte 4
    iNES byte 5
    iNES byte 6
    iNES byte 7
    iNES byte 8
Joystick input

    OP: 0x0D
    Joystick1 data
    Joystick2 data
## Task
- understand the project structure
- finish the Picture Process Unit (src/ppu/*)
- test the PPU using the controller
