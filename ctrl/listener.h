#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

// UART port configuration, adjust according to implementation
int baud_rate = 38400;
serial::bytesize_t byte_size = serial::eightbits;
serial::parity_t parity = serial::parity_odd;
serial::stopbits_t stopbits = serial::stopbits_one;

// to send data to fpga via UART, call:
// 
// void uart_send(const byte *data, int size)
// data: bytes to send 
// size: number of bytes to send
//
// alternatives:
// void uart_send(const std::vector<byte> &data)
// 
// if expecting to receive data from input after send, call:
// void uart_send(const byte *data, int send_count, byte* recv, int recv_count)
// data: bytes to send
// send_count: number of bytes to send
// recv: pointer of byte array to store the bytes received (size>=recv_count)
// recv_count: number of bytes expected to receive
//
// alternatives:
// void uart_send(const std::vector<byte> data, byte* recv, int recv_count)

void on_init(byte* header) {
    // method called on starting the fpga
}

void upload_rom(byte* prg_data, int prg_size, byte* chr_data, int chr_size) {
    // method called to upload the ROM to fpga (PRG ROM & CHR ROM)
}

void on_input(input_type input) {
    // on receiving input from keyboard
    std::cout<<"on input:"<<(int)input<<std::endl;
    switch (input) {
        case P1_UP:
            break;
        case P1_DOWN:
            break;
        case P1_LEFT:
            break;
        case P1_RIGHT:
            break;
        case P1_A:
            break;
        case P1_B:
            break;
        case SELECT:
            break;
        case START:
            break;
    }

}
