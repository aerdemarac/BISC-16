#include "cpu.h"
#include <string.h>

extern int RUN;

int main(void){
    CPU* cpu = cpu_init();
    MemByte* mem = mem_init();

    uint32_t rom[] = {
        encoder_modI(MOVI,GP0,5), //0
        encoder_modI(SUBI, GP0, 1), //4
        encoder_modI(BRC,CON_NON_ZERO,4), //8
        encoder_modR(KILL,IGN,IGN)  // 12
        
    };

    size_t rom_size = sizeof(rom) / sizeof(rom[0]);
    memcpy(mem,rom , rom_size * DWORD);

    while(RUN){
        fetch(cpu,mem);
        decode(cpu);
        execute(cpu,mem);
    }

    cpu_free(&cpu);
    mem_free(&mem);
    return 0;
}

