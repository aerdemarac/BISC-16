#include "cpu.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct Instruction{
    uint8_t opcode;
    uint8_t mode;
    uint8_t reg1;
    uint8_t reg2;
    uint16_t imm;
    uint16_t offset;
    uint32_t raw;
};

struct CPU{
    uint16_t gp_regs[8];
    uint16_t pc;
    uint16_t bp;
    uint16_t sp;
    uint16_t udp;
    Instruction ir;
    uint8_t  flags;
};



CPU* cpu_init(void){
    CPU* cpu = malloc(sizeof(CPU));
    if(cpu){
        memset(cpu,0,sizeof(CPU));
        cpu->bp = STACK_MAX;
        cpu->sp = STACK_MAX;
        cpu->pc = PROG_MIN;
        cpu->udp = USRDATA_MIN;
        return cpu;
    }
    printf("CPU Ret null");
    return NULL;
}
MemByte* mem_init(void){
    MemByte* mem = calloc(1,MEM_SIZE);
    if(!mem){
        printf("Mem Ret null");
        return NULL;
    }
    return mem;
}

void cpu_free(CPU** cpu_ptr){
    free(*cpu_ptr);
    *cpu_ptr = NULL;
    return;
}


void mem_free(MemByte** mem_ptr){
    free(*mem_ptr);
    *mem_ptr = NULL;
    return;
}

void fetch(CPU* cpu, MemByte* mem){
    memcpy(&(cpu->ir.raw), &(mem[cpu->pc]),4);
    cpu->pc += 4;
    return;
}

void decode(CPU* cpu){
    uint32_t raw = cpu->ir.raw;
    uint8_t mode = MOD_FIELD(raw);

    switch (mode) {
        case MOD_REG:
            cpu->ir.mode = MOD_REG;
            cpu->ir.opcode = OP_FIELD(raw);
            cpu->ir.reg1 = REG1_FIELD(raw);
            cpu->ir.reg2 = REG2_FIELD(raw);
            break;
        case MOD_IMM:
            cpu->ir.mode = MOD_IMM;
            cpu->ir.opcode = OP_FIELD(raw);
            cpu->ir.reg1 = REG1_FIELD(raw);
            cpu->ir.imm = IMM_FIELD(raw);
            break;
        case MOD_DER:
            cpu->ir.mode = MOD_REG;
            cpu->ir.opcode = OP_FIELD(raw);
            cpu->ir.reg1 = REG1_FIELD(raw);
            cpu->ir.reg2 = REG2_FIELD(raw);
            break;
        case MOD_OFS:
            cpu->ir.mode = MOD_REG;
            cpu->ir.opcode = OP_FIELD(raw);
            cpu->ir.reg1 = REG1_FIELD(raw);
            cpu->ir.reg2 = REG2_FIELD(raw);
            cpu->ir.offset = OFS_FIELD(raw);
            break;
    }
    return;
}

void execute(CPU* cpu){
    switch(cpu->ir.opcode){
        case MOVI:
            cpu->gp_regs[cpu->ir.reg1] = (uint16_t)(cpu->ir.imm);
            break;
        case MOV:
            cpu->gp_regs[cpu->ir.reg1] = cpu->gp_regs[cpu->ir.reg2];
            break;
    }
    return;
}
