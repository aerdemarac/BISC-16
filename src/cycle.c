#include <stdio.h>
#include "exec.h"
#include "cpu.h"
#include "qencode.h"

#define ROM_PATH argv[1]

int main(int argc,char** argv){
    banner();
    usage();

    argc_check(argc);

    FILE* rom_file = fopen(ROM_PATH,"rb");

    file_check(rom_file);
    extension_check(ROM_PATH,rom_file);

    CPU* cpu = cpu_init();
    MemByte* mem = mem_init();

    fread(mem,DWORD,PROG_MAX,rom_file);
    fclose(rom_file);

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
