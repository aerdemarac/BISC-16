#include "cpu.h"
#include <string.h>

int main(void){
    CPU* cpu = cpu_init();
    MemByte* mem = mem_init();

    uint32_t rom[] = {
        encoder_modI(ADDI, GP0, 31),
        encoder_modI(ADDI, GP1, 9),
        encoder_modR(ADD,  GP0, GP1)
    };

    size_t rom_size = sizeof(rom) / sizeof(rom[0]);
    memcpy(mem,rom , rom_size * DWORD);

    for(size_t i = 0 ; i < rom_size ; i++){
        fetch(cpu,mem);
        decode(cpu);
        execute(cpu,mem);
    }

    cpu_free(&cpu);
    mem_free(&mem);
    return 0;
}

