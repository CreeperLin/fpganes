#include <iostream>
#include <cstdio>
#include <fstream>
#include <serial/serial.h>
#include <vector>
#include <termios.h>
typedef std::uint8_t byte;
typedef std::uint16_t word;
using namespace std;

serial::Serial serPort;

void uart_send(const byte *data, int send_count, byte* recv, int recv_count) {
    serPort.write(data,send_count);
    if (!recv_count) return;
    try {
        serPort.read(recv,recv_count);
    } catch (std::exception &e) {
        cout<<"recv error:"<<e.what()<<endl;
    }
}
void uart_send(const std::vector<byte> data, byte* recv, int recv_count) {
    uart_send(&data[0],data.size(),recv,recv_count);
}
void uart_send(const std::string &data) { serPort.write(data); }
void uart_send(const byte *data, int size) { serPort.write(data,size); }
void uart_send(const std::vector<byte> &data) { serPort.write(data); }

enum input_type {
    NONE=0,
    P1_UP, P1_DOWN, P1_LEFT, P1_RIGHT, P1_A, P1_B,
    P2_UP, P2_DOWN, P2_LEFT, P2_RIGHT, P2_A, P2_B,
    SELECT, START
};
input_type key_map[256];

byte header[20];
byte* prg_data;
int prg_size;
byte* chr_data;
int chr_size;

#include "listener.h"
int load_rom(char* rom_path) {
    ifstream fin(rom_path);
    if (!fin.is_open()) {
        return 1;
    }
    fin.read(reinterpret_cast<char*>(header),16);
    int prg_count = (int)(header[4]);
    cout<<"PRG ROM size:\t"<<prg_count<<" x 16KiB"<<endl;
    int chr_count = (int)(header[5]);
    cout<<"CHR ROM size:\t"<<chr_count<<" x 8KiB"<<endl;
    int f6 = (int)(header[6]);
    int mirror = f6 & 1;
    if (f6&8) {
        cout<<"Mirroring:\tignored"<<endl;
    } else if (mirror) {
        cout<<"Mirroring:\thorizontal"<<endl;
    } else {
        cout<<"Mirroring:\tvertical"<<endl;
    }
    int prg_ram = f6&2;
    if (prg_ram) {
        cout<<"PRG RAM found"<<endl;
    }
    int mapper_no = (f6>>4)&15;
    cout<<"Mapper #:\t"<<mapper_no<<endl;
    int f7 = (int)(header[7]);
    int prg_ram_count = (int)(header[8]);
    if (prg_ram) {
        cout<<"PRG RAM size:\t"<<prg_ram_count<<"x 8KiB"<<endl;
    }
    
    prg_size = prg_count << 14;
    prg_data = new byte[prg_size+10];
    cout<<"reading PRG ROM: "<<prg_size<<endl;
    fin.read(reinterpret_cast<char*>(prg_data),prg_size);
    for (int i=0;i<0x100; ++i) {
        printf("%02x ", (byte)prg_data[i]);
        if (!((i+1)%32)) printf("\n");
    }

    chr_size = chr_count << 13;
    chr_data = new byte[chr_size+10];
    cout<<"reading CHR ROM: "<<chr_size<<endl;
    fin.read(reinterpret_cast<char*>(chr_data),chr_size);
    for (int i=0;i<0x100; ++i) {
        printf("%02x ", (byte)chr_data[i]);
        if (!((i+1)%32)) printf("\n");
    }
    fin.close();
}

int init_port(char* port) {
    serPort.setPort(port);
    serPort.setBaudrate(baud_rate);
    serPort.setBytesize(byte_size);
    serPort.setParity(parity);
    serPort.setStopbits(stopbits);
    try {
        serPort.open();
    } catch (std::exception &e) {
        cout<<"failed to open port: "<<e.what()<<endl;
        return 1;
    }
    cout<<"initialized UART port on: "<<port<<endl;
    return 0;
}

void ctrl() {
    int key[15];
    std::ifstream fin("./keymap.txt");
    for (int i=0;i<14;++i) {
        char c;
        fin>>c;
        key[i]=(int)c;
    }
    key_map[key[0]]=P1_UP;
    key_map[key[1]]=P1_DOWN;
    key_map[key[2]]=P1_LEFT;
    key_map[key[3]]=P1_RIGHT;
    key_map[key[4]]=P1_A;
    key_map[key[5]]=P1_B;
    key_map[key[6]]=P2_UP;
    key_map[key[7]]=P2_DOWN;
    key_map[key[8]]=P2_LEFT;
    key_map[key[9]]=P2_RIGHT;
    key_map[key[10]]=P2_A;
    key_map[key[11]]=P2_B;
    key_map[key[12]]=SELECT;
    key_map[key[13]]=START;
    fin.close();

    char c;
    termios stored_settings;
    tcgetattr(0,&stored_settings);
    termios new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_lflag &= (~ECHO);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0,TCSANOW,&new_settings);

    int run=0;
    while (1){
        c = getchar();
        input_type input = key_map[(int)c];
        cout<<"input: "<<c<<' '<<(int)input<<endl;
        if (c=='q') {
            tcsetattr(0,TCSANOW,&stored_settings);
            break;
        }
        else if (input!=NONE) on_input(input);
    }
}

int main(int argc, char** argv) {
    if (argc<3) {
        cout << "usage: path-to-rom com-port" << endl;
        return 1;
    }
    char* rom_path = argv[1];
    char* comport = argv[2];
    if (init_port(comport)) return 1;
    load_rom(rom_path);
    on_init(header);
    upload_rom(prg_data,prg_size,chr_data,chr_size); 
    cout<<"rom uploaded"<<endl;
    ctrl();
    serPort.close();
}
