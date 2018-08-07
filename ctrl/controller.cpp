#include <iostream>
#include <cstdio>
#include <fstream>
#include <serial/serial.h>
#include <vector>
#include <termios.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
typedef std::uint8_t byte;
typedef std::uint16_t word;
using namespace std;

serial::Serial serPort;

void uart_send(const byte *data, int send_count, byte* recv, int return_count) {
    serPort.write(data,send_count);
    if (!return_count) return;
    try {
        serPort.read(recv,return_count);
    } catch (std::exception &e) {
        cout<<"recv error:"<<e.what()<<endl;
    }
}
void uart_send(const std::vector<byte> data, byte* recv, int return_count) {
    uart_send(data.data(),data.size(),recv,return_count);
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
    for (int i=0;i<0x200; ++i) {
        printf("%02x ", (byte)prg_data[i]);
        if (!((i+1)%32)) printf("\n");
    }
    cout<<endl;

    chr_size = chr_count << 13;
    chr_data = new byte[chr_size+10];
    cout<<"reading CHR ROM: "<<chr_size<<endl;
    fin.read(reinterpret_cast<char*>(chr_data),chr_size);
    fin.close();
}

int init_port(char* port) {
    serPort.setPort(port);
    serPort.setBaudrate(baud_rate);
    serPort.setBytesize(byte_size);
    serPort.setParity(parity);
    serPort.setStopbits(stopbits);
    serPort.setTimeout(
        inter_byte_timeout,
        read_timeout_constant,
        read_timeout_multiplier,
        write_timeout_constant,
        write_timeout_multiplier
        );
    try {
        serPort.open();
    } catch (std::exception &e) {
        cout<<"failed to open port: "<<e.what()<<endl;
        return 1;
    }
    cout<<"initialized UART port on: "<<port<<endl;
    return 0;
}

void ctrl(char *kbd_path) {
    int key[15];
    std::ifstream fin("./keymap.txt");
    for (int i=0;i<14;++i) {
        int c;
        fin>>c;
        key[i]=c;
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

    termios stored_settings;
    tcgetattr(0,&stored_settings);
    termios new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_lflag &= (~ECHO);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0,TCSANOW,&new_settings);

    int run=0;
    int keys_fd;
    struct input_event t;
    keys_fd=open(kbd_path,O_RDONLY);
    if(keys_fd<=0)
    {   
        printf("error\n");
        return; 
    }   
    //char c;
    int input_array[16] = {0};
    while (1){
        read(keys_fd,&t,sizeof(struct input_event));
        if (t.type != 1 || t.value == 2) continue;
        printf("key %i state %i \n",t.code,t.value);
        //c = getchar();
        //cout<<"input: "<<c<<' '<<(int)input<<endl;
        input_type input = key_map[t.code];
        int c = (t.value == 0)?t.code:0;
        if (c==16) {
            tcsetattr(0,TCSANOW,&stored_settings);
            break;
        }
        else if (c==25) {
            byte send[6]={0};
            byte recv[64]={0};
            send[0]=0x05;
            send[1]=0x00;
            uart_send(send,2,recv,1);
            int pcl = recv[0];
            send[1] = 0x01;
            uart_send(send,2,recv,1);
            int pch = recv[0];
            word pc = pcl|(pch<<8);
            printf("pc lo: %02x hi: %02x pc:%04x\n",pcl,pch,pc);
            send[0] = 0x01;
            *reinterpret_cast<word*>(send+1)=pc;
            *reinterpret_cast<word*>(send+3)=16;
            uart_send(send,5,recv,16);
            for (int i=0;i<16;++i) {
                printf("%02x ",recv[i]);
            }
            printf("\n");
        }
        else if (c==19) {
            std::vector<byte> data;
            data.push_back(run?0x03:0x04);
            run = !run;
            uart_send(data);
        }
        else if (input!=NONE) {
            if (t.value==1) input_array[(int)input] = 1;
            else input_array[(int)input] = 0;
            on_input(input_array);
        }
    }
}

int main(int argc, char** argv) {
    if (argc<3) {
        cout << "usage: path-to-rom com-port input-device" << endl;
        return 1;
    }
    char* rom_path = argv[1];
    char* comport = argv[2];
    char* kbd_path = argv[3];
    if (init_port(comport)) return 1;
    cout<<"load rom?";
    char c;
    c = getchar();
    if (c=='y'){
        load_rom(rom_path);
        if (on_init(header)) return 1;
        upload_rom(prg_data,prg_size,chr_data,chr_size); 
        cout<<"rom uploaded"<<endl;
    }
    ctrl(kbd_path);
    serPort.close();
}
