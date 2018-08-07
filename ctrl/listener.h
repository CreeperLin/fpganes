#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

// UART port configuration, adjust according to implementation
int baud_rate = 38400;
serial::bytesize_t byte_size = serial::eightbits;
serial::parity_t parity = serial::parity_odd;
serial::stopbits_t stopbits = serial::stopbits_one;
int inter_byte_timeout = 50;
int read_timeout_constant = 5000;
int read_timeout_multiplier = 10;
int write_timeout_constant = 50;
int write_timeout_multiplier = 10;

//methods
int on_init(byte* header) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
    byte init[10];
    byte recv[10]={0};
    init[0] = 0x00;
    *reinterpret_cast<word*>(init+1) = 3;
    char test[10] = "NES";
    for (int i=0;i<3;++i) {
        init[3+i]=(byte)test[i];
        printf("%02x\n",init[3+i]);
    }
    uart_send(init,6,recv,3);
    for (int i=0;i<3;++i) {
        printf("%02x\n",recv[i]);
    }
    char *str = reinterpret_cast<char*>(recv);
    if (strcmp(str,test)) {
        printf("UART assert failed\n");
        return 1;
    }
    std::vector<byte> data;
    data.push_back(0x03);
    uart_send(data);
    data.clear();
    data.push_back(0x0B);
    uart_send(data);
    data.clear();
    data.push_back(0x0C);
    data.push_back(header[4]);
    data.push_back(header[5]);
    data.push_back(header[6]);
    data.push_back(header[7]);
    data.push_back(header[8]);
    uart_send(data);
    data.clear();
    return 0;
}

void upload_rom(byte* prg_data, int prg_size, byte* chr_data, int chr_size) {
    unsigned short blk_size = 0x400;
    int pld_size = 1+2+2+blk_size;
    byte* payload = new byte[pld_size];
    for (int i=0; i<prg_size/blk_size; ++i) {
        unsigned short offset = i * blk_size;
        unsigned short addr = 0x8000 + offset;
        payload[0] = 0x02;
        *reinterpret_cast<word*>(payload+1) = addr;
        *reinterpret_cast<word*>(payload+3) = blk_size;
        for (int j=0;j<blk_size;++j) {
            payload[5+j] = (byte)prg_data[offset+j];
        }
        uart_send(payload,pld_size);
    }
    std::cout<<"PRG ROM uploaded"<<std::endl;
    std::this_thread::sleep_for(1s);
    for (int i=0; i<chr_size/blk_size; ++i) {
        unsigned short offset = i * blk_size;
        unsigned short addr = offset;
        payload[0] = 0x0A;
        *reinterpret_cast<word*>(payload+1) = addr;
        *reinterpret_cast<word*>(payload+3) = blk_size;
        for (int j=0;j<blk_size;++j) {
            payload[5+j] = (byte)chr_data[offset+j];
        }
        uart_send(payload,pld_size);
    }
    std::cout<<"CHR ROM uploaded"<<std::endl;
    std::this_thread::sleep_for(1s);
    byte pcl = prg_data[prg_size-4];
    byte pch = prg_data[prg_size-3];
    printf("pc lo: %02x hi: %02x\n",pcl,pch);
    byte data[5];
    data[0] = 0x06;
    data[1] = 0x00;
    data[2] = (byte)pcl;
    uart_send(data,3);
    data[1] = 0x01;
    data[2] = (byte)pch;
    uart_send(data,3);
    delete[] payload;
}

void on_input(int* input_array) {
    //std::cout<<"on input:"<<(int)input<<std::endl;
    byte data[5]={0};
    data[0] = 0x0D;
    if (input_array[P1_UP]) data[1] |= 1<<3;
    if (input_array[P1_DOWN]) data[1] |= 1<<2;
    if (input_array[P1_LEFT]) data[1] |= 1<<1;
    if (input_array[P1_RIGHT]) data[1] |= 1;
    if (input_array[P1_A]) data[1] |= 1<<7;
    if (input_array[P1_B]) data[1] |= 1<<6;
    if (input_array[SELECT]) data[1] |= 1<<5;
    if (input_array[START]) data[1] |= 1<<4;
    if (input_array[P2_UP]) data[2] |= 1<<3;
    if (input_array[P2_DOWN]) data[2] |= 1<<2;
    if (input_array[P2_LEFT]) data[2] |= 1<<1;
    if (input_array[P2_RIGHT]) data[2] |= 1;
    if (input_array[P2_A]) data[2] |= 1<<7;
    if (input_array[P2_B]) data[2] |= 1<<6;
    printf("jp: %02x %02x\n",data[1],data[2]);
    uart_send(data,3);
}
