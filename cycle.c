#include "cpu.h"
#include <string.h>

int main(void){
    CPU* cpu = cpu_init();
    MemByte* mem = mem_init();

    uint32_t rom[] = {
        encoder_modI(MOVI, GP1, 69),
        encoder_modR(MOV,  GP0, GP1),
        encoder_modO(STRB, GP0, UDP, 0),
        encoder_modO(STRB, GP1, UDP, 1),
        encoder_modO(LDB,  GP6, UDP, 0),
        encoder_modO(LDB,  GP7, UDP, 1),
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

