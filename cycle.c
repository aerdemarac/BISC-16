#include "cpu.h"
#include <string.h>

int main(void){
    CPU* cpu = cpu_init();
    MemByte* mem = mem_init();
    uint8_t rom[1024] = {0x10,0x00,0x00,0x15,0x00,0x00,0x10,0x10};
    memcpy(mem,rom , 8);
    for(size_t i = 0 ; i < 2 ; i++){
        fetch(cpu,mem);
        decode(cpu);
        execute(cpu);
    }
    cpu_free(&cpu);
    mem_free(&mem);
    return 0;
}

