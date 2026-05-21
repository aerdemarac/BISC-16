#include "cpu.h"
#include "qencode.h"
#include <stdio.h>
#include <string.h>

extern int RUN;

int main(void){
    CPU* cpu = cpu_init();
    MemByte* mem = mem_init();

    /* ROM storage that contains code to be copied to main memory 
     * Quick Encoder Macros can be used for quick code generation as seen below
    */

    /*Program Binary Goes Here*/
    uint32_t rom[] = { 
        CALL_CLEAR(),
        MOVI(GP0, 0xA), 
        MOVI(GP1, 0x30), 
        MOVI(GP2, 0xA), 
        MOVI(GP3,0x30), 
        MOVI(GP4,0x31), 
        ADD(GP1, GP0),
        CMPI(GP0,10),
        BRC(CON_NON_ZERO, 48),
        CALL_PUTCHAR(GP4),
        CALL_PUTCHAR(GP3),
        JMP(52),
        CALL_PUTCHAR(GP1),
        CALL_PUTCHAR(GP2),
        SUBI(GP0, 1),
        SUBI(GP1, 1),
        CALL_SLEEP(1000),
        CMPI(GP0, 0),
        BRC(CON_NON_ZERO,28),
        KILL()         
    }; // Example Counter Program

    size_t rom_size = sizeof(rom) / sizeof(rom[0]);
    memcpy(mem,rom , rom_size * DWORD);

    while(RUN){
        fetch(cpu,mem);
        decode(cpu);
        execute(cpu,mem);
    }

    fflush(stdout);
    cpu_free(&cpu);
    mem_free(&mem);
    return 0;
}

