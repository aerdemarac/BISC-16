/* Unlocking Hidden POSIX Methods */
#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 199309L 

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "cpu.h"


/* Required for Cross-Platfrom Sleep Behaviour */
#ifdef _WIN32
    #include <Windows.h> 
#endif

struct Instruction{
    uint32_t raw;
    uint16_t imm;
    uint16_t offset;
    uint8_t opcode;
    uint8_t mode;
    uint8_t reg1;
    uint8_t reg2;
    uint8_t reg3;
};

struct CPU{
    Instruction ir;
    uint16_t gp_regs[8];
    uint16_t pc;
    uint16_t bp;
    uint16_t sp;
    uint16_t udp;
    uint16_t flags; 
};

/* Flags Register is kept 16 bits long in order to
 * keep "locate_reg()" method standardized for 
 * all registers*/

int RUN = 1;
/*Manipulated to 0 If KILL instruction ran*/

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
    clear_decoded_fields(cpu);
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
        case MOD_OFS:
            cpu->ir.mode = MOD_OFS;
            cpu->ir.opcode = OP_FIELD(raw);
            cpu->ir.reg1 = REG1_FIELD(raw);
            cpu->ir.reg2 = REG2_FIELD(raw);
            cpu->ir.offset = OFS_FIELD(raw);
            break;
        case MOD_TRI:
            cpu->ir.mode = MOD_TRI;
            cpu->ir.opcode = OP_FIELD(raw);
            cpu->ir.reg1 = REG1_FIELD(raw);
            cpu->ir.reg2 = REG2_FIELD(raw);
            cpu->ir.reg3 = REG3_FIELD(raw);
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
    uint8_t reg3_index = cpu->ir.reg3;
    uint16_t* reg1_ptr = locate_reg(cpu,reg1_index);
    uint16_t* reg2_ptr = locate_reg(cpu,reg2_index);
    uint16_t* reg3_ptr = locate_reg(cpu,reg3_index);
    uint8_t* addr_ptr = locate_add(mem, *reg2_ptr);
    uint8_t* sp_addr = locate_add(mem, cpu->sp);
    uint16_t imm = cpu->ir.imm;
    uint16_t offset = cpu->ir.offset;
    uint16_t shift_reg = *reg2_ptr & 0x0F; // Masking to keep shift value the maximum 15 
    uint16_t shift_imm = imm & 0x0F; // shift by immediate value max 15
    uint16_t reg1_inital = *reg1_ptr;
    uint16_t br_condition = reg1_index;
    uint16_t temp = 0;

    switch(cpu->ir.opcode){
        case OP_LD: // Mode : 2
            memcpy(reg1_ptr,addr_ptr + offset,WORD);
            break;
        case OP_LDB: // Mode : 2
            memcpy(reg1_ptr,addr_ptr + offset,HWORD);
            break;
        case OP_STR: // Mode : 2
            memcpy(addr_ptr+ offset,reg1_ptr,WORD);
            break;
        case OP_STRB: // Mode : 2
            memcpy(addr_ptr + offset,reg1_ptr,HWORD);
            break;
        case OP_MOV: // Mode : 0
            memcpy(reg1_ptr, reg2_ptr, WORD);
            break;
        case OP_MOVI: // Mode : 1
            memcpy(reg1_ptr, &imm,WORD);
            break;
        /*---------------------------------------------*/
        case OP_ADD: // Mode : 0
            *reg1_ptr += *reg2_ptr;

            (*reg2_ptr && *reg1_ptr <= reg1_inital)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags, FL_CARRY) ;
            break;
        case OP_ADDI: // Mode : 1
            *reg1_ptr += imm;

            (imm && *reg1_ptr <= reg1_inital)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags, FL_CARRY) ;
            break;
        case OP_ADDS: // Mode : 3
            *reg1_ptr = *reg2_ptr + *reg3_ptr;

            (*reg1_ptr <= *reg2_ptr)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags, FL_CARRY) ;
            break;
        case OP_SUB: // Mode : 0
            *reg1_ptr -= *reg2_ptr;

            (*reg2_ptr && *reg1_ptr >= reg1_inital)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags,FL_CARRY) ;
            break;
        case OP_SUBI: // Mode : 1
            *reg1_ptr -= imm;

            (imm && *reg1_ptr >= reg1_inital)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags,FL_CARRY) ;
            break;
        case OP_SUBS: // Mode : 3
            *reg1_ptr = *reg2_ptr - *reg3_ptr;

            (*reg1_ptr >= *reg2_ptr)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags, FL_CARRY) ;
            break;
        /*---------------------------------------------*/
        case OP_AND: // Mode : 2
            *reg1_ptr = *reg2_ptr & *reg3_ptr;
            break;
        case OP_ANDI: // Mode : 2
            *reg1_ptr = *reg2_ptr & offset;

            break;
        case OP_OR: // Mode : 3
            *reg1_ptr = *reg2_ptr | *reg3_ptr;

            break;
        case OP_ORI: // Mode : 2
            *reg1_ptr = *reg2_ptr | offset;

            break;
        case OP_ZOR: // Mode : 3
            *reg1_ptr = *reg2_ptr ^ *reg3_ptr;
            break;
        case OP_ZORI: // Mode : 2
            *reg1_ptr = *reg2_ptr ^ offset;

            break;
        case OP_CMP: // Mode : 0
            if(*reg1_ptr > *reg2_ptr){
                SET_BIT(cpu->flags, FL_CARRY);
                CLEAR_BIT(cpu->flags,FL_ZERO);
            }
            else if(*reg1_ptr == *reg2_ptr){
                SET_BIT(cpu->flags, FL_ZERO);
                CLEAR_BIT(cpu->flags, FL_CARRY);
            }
            else{
                CLEAR_BIT(cpu->flags, FL_ZERO);
                CLEAR_BIT(cpu->flags, FL_CARRY);
            }
            break;
        case OP_CMPI: // Mode : 1
            if(*reg1_ptr > imm){
                SET_BIT(cpu->flags, FL_CARRY);
                CLEAR_BIT(cpu->flags,FL_ZERO);
            }
            else if(*reg1_ptr == imm){
                SET_BIT(cpu->flags, FL_ZERO);
                CLEAR_BIT(cpu->flags, FL_CARRY);
            }
            else{
                CLEAR_BIT(cpu->flags, FL_ZERO);
                CLEAR_BIT(cpu->flags, FL_CARRY);
            }
            break;
        case OP_CMPW : // Mode: 2
            memcpy(&temp, addr_ptr + offset, WORD);
            if(*reg1_ptr > temp){
                SET_BIT(cpu->flags, FL_CARRY);
                CLEAR_BIT(cpu->flags,FL_ZERO);
            }
            else if(*reg1_ptr == temp){
                SET_BIT(cpu->flags, FL_ZERO);
                CLEAR_BIT(cpu->flags, FL_CARRY);
            }
            else{
                CLEAR_BIT(cpu->flags, FL_ZERO);
                CLEAR_BIT(cpu->flags, FL_CARRY);
            }
            break;
        case OP_CMPH:  // Mode: 2
            memcpy(&temp, addr_ptr + offset, HWORD);
            if(*reg1_ptr > temp){
                SET_BIT(cpu->flags, FL_CARRY);
                CLEAR_BIT(cpu->flags,FL_ZERO);
            }
            else if(*reg1_ptr == temp){
                SET_BIT(cpu->flags, FL_ZERO);
                CLEAR_BIT(cpu->flags, FL_CARRY);
            }
            else{
                CLEAR_BIT(cpu->flags, FL_ZERO);
                CLEAR_BIT(cpu->flags, FL_CARRY);
            }
            break;
        case OP_SR: // Mode : 0
            /*Set FL_CARRY to whatever bit was trashed by shift operation*/
            ((*reg1_ptr >> (shift_reg - 1)) & 0x1)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags,FL_CARRY) ;
            *reg1_ptr = (uint16_t)(*reg1_ptr >> shift_reg); //Odd syntax in order to supress strict compiler warning
            break;
        case OP_SRI: // Mode : 1
            /*Set FL_CARRY to whatever bit was trashed by shift operation*/
            ((*reg1_ptr >> (shift_imm - 1)) & 0x1)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags,FL_CARRY) ; 
            *reg1_ptr = (uint16_t)(*reg1_ptr >> shift_imm);
            break;
        case OP_SL: // Mode : 0
            *reg1_ptr = (uint16_t)(*reg1_ptr << shift_reg);

            (*reg2_ptr && reg1_inital >> 15 & 0x01)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags,FL_CARRY) ; 
            break;
        case OP_SLI: // Mode : 1
            *reg1_ptr = (uint16_t)(*reg1_ptr << shift_imm);

            (imm && reg1_inital >> 15 & 0x01)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags,FL_CARRY) ; 
            break;
        /*---------------------------------------------*/
        case OP_JMP:    // Mode : 1
            cpu->pc = imm; // Jump to the specified byte address
            break;
        case OP_BRC:    // Mode : 1
            switch (br_condition) {
               case CON_NON_ZERO:
                   if(!MASK_BIT(cpu->flags,FL_ZERO)){
                       temp = 1;
                   }
                   break;
               case CON_ZERO:
                   if(MASK_BIT(cpu->flags,FL_ZERO)){
                       temp = 1;
                   }
                   break;
               case CON_CARRY_CLEAR:
                   if(!MASK_BIT(cpu->flags,FL_CARRY)){
                       temp = 1;
                   }
                   break;
               case CON_CARRY_SET:
                   if(MASK_BIT(cpu->flags,FL_CARRY)){
                       temp = 1;
                   }
                   break;
               case CON_POSITIVE:
                   if(!MASK_BIT(cpu->flags,FL_NEGATIVE)){
                       temp = 1;
                   }
                   break;
               case CON_NEGATIVE:
                   if(MASK_BIT(cpu->flags,FL_NEGATIVE)){
                       temp = 1;
                   }
                   break;
            }

            if(temp){ // Temp Used as a boolean val in order to determine jump
                cpu->pc = imm;
            }
            break;
        case OP_CALL: // Mode : 1
            if((cpu->sp - WORD) < STACK_MIN){ // Prevent Overflow
                break;
            }
            cpu->sp -= WORD;
            sp_addr = locate_add(mem, cpu->sp); //Needs to be updated 
            memcpy(sp_addr,&cpu->pc, WORD);
            cpu->pc = imm;
            break;
        case OP_RET:  // Mode : 0
            if((cpu->sp + WORD) > STACK_MAX){
                break;
            }
            sp_addr = locate_add(mem, cpu->sp); //Needs to be updated 
            memcpy(&cpu->pc,sp_addr,WORD);
            memset(sp_addr,0,WORD);
            cpu->sp += WORD;
            break;
        case OP_NOP:  // Mode : 0
           // No Operation
            break;
        case OP_KILL:  // Mode : 0
            RUN = 0;
            break;
        case OP_SYSCALL:
            switch (reg1_index) {
                case SYS_PUTCHAR:
                    putchar((char)(*reg2_ptr));
                    break;
                case SYS_PUTS:
                    puts((const char*)(addr_ptr + offset));
                    break;
                case SYS_CLEAR:
#ifdef _WIN32
                    system("cls");
#else
                    system("clear");
#endif
                    break;
                case SYS_SLEEP:
#ifdef _WIN32
                    Sleep(offset);
#else
                    usleep(offset * 1000); // in milliseconds
#endif
                    
            }
        default:
            break;
    }

    // Generic-Flag Checks Exc. CMP*
    if(cpu->ir.opcode >= OP_ADD && (cpu->ir.opcode < OP_CMP || cpu->ir.opcode > OP_CMPH) ){ 
        (*reg1_ptr == 0)? SET_BIT(cpu->flags,FL_ZERO) : CLEAR_BIT(cpu->flags,FL_ZERO) ;
        (*reg1_ptr & SIGN_BIT)? SET_BIT(cpu->flags, FL_NEGATIVE) : CLEAR_BIT(cpu->flags, FL_NEGATIVE) ;
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
uint32_t encoder_modO(uint8_t op,uint8_t reg1,uint8_t reg2,uint16_t ofs){
    uint32_t inst = ((uint32_t)(op) << 26)      | 
                    ((uint32_t)(MOD_OFS) << 24) |
                    ((uint32_t)(reg1) << 20)    |
                    ((uint32_t)(reg2) << 16)    |
                    ((uint32_t)(ofs) << 0) ;
    return inst;
}
uint32_t encoder_modT(uint8_t op,uint8_t reg1,uint8_t reg2,uint8_t reg3){
    uint32_t inst = ((uint32_t)(op) << 26)      | 
                    ((uint32_t)(MOD_TRI) << 24) |
                    ((uint32_t)(reg1) << 20)    |
                    ((uint32_t)(reg2) << 16)    |
                    ((uint32_t)(reg3) << 12) ;
    return inst;
}

void clear_decoded_fields(CPU* cpu){
    if(!cpu){
        return;
    }
    cpu->ir.imm = 0;
    cpu->ir.offset = 0;
    cpu->ir.opcode = 0;
    cpu->ir.mode = 0;
    cpu->ir.reg1 = 0;
    cpu->ir.reg2 = 0;
    cpu->ir.reg3 = 0;
    return;
}
