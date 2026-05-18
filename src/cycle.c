#include "cpu.h"
#include "qencode.h"
#include <string.h>

extern int RUN;

int main(void){
    CPU* cpu = cpu_init();
    MemByte* mem = mem_init();

    /* ROM storage that contains code to be copied to main memory 
     * Quick Encoder Macros can be used for quick code generation as seen below
    */
    uint32_t rom[] = { 
        MOVI(GP0,'N'),
        SYSCALL(SYS_PUTCHAR,UDP),
        KILL(),   
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

