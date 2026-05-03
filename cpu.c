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
    uint16_t  flags;
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
    if(!cpu_ptr ||!(cpu_ptr)){
        return;
    }
    free(*cpu_ptr);
    *cpu_ptr = NULL;
    return;
}


void mem_free(MemByte** mem_ptr){
    if(!mem_ptr ||!(mem_ptr)){
        return;
    }
    free(*mem_ptr);
    *mem_ptr = NULL;
    return;
}

void fetch(CPU* cpu, MemByte* mem){
    if(!cpu || !mem){
        return;
    }
    memset(&(cpu->ir.raw),0,DWORD);
    memcpy(&(cpu->ir.raw), &(mem[cpu->pc]),DWORD);
    cpu->pc += 4;
    return;
}

void decode(CPU* cpu){
    if(!cpu){
        return;
    }
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
            cpu->ir.mode = MOD_DER;
            cpu->ir.opcode = OP_FIELD(raw);
            cpu->ir.reg1 = REG1_FIELD(raw);
            cpu->ir.reg2 = REG2_FIELD(raw);
            break;
        case MOD_OFS:
            cpu->ir.mode = MOD_OFS;
            cpu->ir.opcode = OP_FIELD(raw);
            cpu->ir.reg1 = REG1_FIELD(raw);
            cpu->ir.reg2 = REG2_FIELD(raw);
            cpu->ir.offset = OFS_FIELD(raw);
            break;
    }
    return;
}

void execute(CPU* cpu,MemByte* mem){ // Needs to be tied up
    if(!cpu || !mem){
        return;
    }
    uint8_t reg1_index = cpu->ir.reg1;
    uint8_t reg2_index = cpu->ir.reg2;
    uint16_t* reg1_ptr = locate_reg(cpu,reg1_index);
    uint16_t* reg2_ptr = locate_reg(cpu,reg2_index);
    uint8_t* addr_ptr = locate_add(mem, *reg2_ptr);
    uint16_t imm = cpu->ir.imm;
    uint16_t offset = cpu->ir.offset;

    switch(cpu->ir.opcode){
        case LD:
            write_reg(cpu,reg1_ptr,addr_ptr + offset,WORD);
            break;
        case LDB:
            write_reg(cpu,reg1_ptr,addr_ptr + offset,HWORD);
            break;
        case STR:
            write_mem(mem,addr_ptr+ offset,reg1_ptr,WORD);
            break;
        case STRB:
            write_mem(mem,addr_ptr + offset,reg1_ptr,HWORD);
            break;
        case MOV:
            write_reg(cpu, reg1_ptr, reg2_ptr, WORD);
            break;
        case MOVI:
            write_reg(cpu, reg1_ptr, &imm,WORD);
            break;

        case ADD:
            
            break;
    }
    return;
}
uint16_t* locate_reg(CPU* cpu,uint8_t reg){
    if(!cpu){
        return NULL;
    }
    uint16_t* ptr = NULL;

    if(reg < 8U){ // If it is a general-purpose register
        ptr = &cpu->gp_regs[reg];
    }
    else{
        switch(reg){
            case BP: 
                ptr = &cpu->bp;
                break;
            case SP: 
                ptr = &cpu->sp;
                break;
            case PC: 
                ptr = &cpu->pc;
                break;
            case UDP: 
                ptr = &cpu->udp;
                break;
            case FL: 
                ptr = &cpu->flags;
                break;
        }
    }
    return ptr;
}
uint8_t* locate_add(MemByte* mem,uint16_t add_val){
    return &mem[add_val];
}
void write_reg(CPU* cpu,uint16_t* dest_ptr,void* src_ptr,size_t N){
    if(!cpu || !src_ptr){
        return;
    }
    memcpy(dest_ptr, src_ptr, N);
    return;

}


void write_mem(MemByte* mem, uint8_t* dest_addr,void* src_add,size_t N){
    if(!mem || !src_add) {
        return;
    }
    memcpy(dest_addr,src_add,N);
    return;
}



uint32_t encoder_modR(uint8_t op,uint8_t reg1,uint8_t reg2){
    uint32_t inst = ((uint32_t)(op) << 26)      | 
                    ((uint32_t)(MOD_REG) << 24) |
                    ((uint32_t)(reg1) << 20)    |
                    ((uint32_t)(reg2) << 16) ;
    return inst;
}
uint32_t encoder_modI(uint8_t op,uint8_t reg1,uint16_t imm){
    uint32_t inst = ((uint32_t)(op) << 26)      | 
                    ((uint32_t)(MOD_IMM) << 24) |
                    ((uint32_t)(reg1) << 20)    |
                    ((uint32_t)(imm) << 4) ;
    return inst;

}
uint32_t encoder_modD(uint8_t op,uint8_t reg1,uint8_t reg2){
    uint32_t inst = ((uint32_t)(op) << 26)      | 
                    ((uint32_t)(MOD_DER) << 24) |
                    ((uint32_t)(reg1) << 20)    |
                    ((uint32_t)(reg2) << 16) ;
    return inst;
}
uint32_t encoder_modO(uint8_t op,uint8_t reg1,uint8_t reg2,uint16_t ofs){
    uint32_t inst = ((uint32_t)(op) << 26)      | 
                    ((uint32_t)(MOD_OFS) << 24) |
                    ((uint32_t)(reg1) << 20)    |
                    ((uint32_t)(reg2) << 16)    |
                    ((uint32_t)(ofs) << 0) ;
    return inst;
}

/*---------------------LEGACY------------------*/
uint8_t* read_mem_hword(MemByte* mem,uint16_t mem_addr){ 
    if(!mem){
        return NULL;
    }
    return &mem[mem_addr];
}
uint16_t* read_mem_word(MemByte* mem,uint16_t mem_addr){
    if(!mem){
        return NULL;
    }
    static uint16_t val = 0;
    memcpy(&val, &mem[mem_addr],WORD); // Reading WORD size data from byte addressed mem requires this call
    return &val;
}
/*-------------------LEGACY--------------*/
