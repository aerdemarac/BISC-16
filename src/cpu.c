#include "cpu.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
        case MOD_TRI:
            cpu->ir.mode = MOD_TRI;
            cpu->ir.opcode = OP_FIELD(raw);
            cpu->ir.reg1 = REG1_FIELD(raw);
            cpu->ir.reg2 = REG2_FIELD(raw);
            cpu->ir.reg3 = REG3_FIELD(raw);
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
    uint8_t reg3_index = cpu->ir.reg3;
    uint16_t* reg1_ptr = locate_reg(cpu,reg1_index);
    uint16_t* reg2_ptr = locate_reg(cpu,reg2_index);
    uint16_t* reg3_ptr = locate_reg(cpu,reg3_index);
    uint8_t* addr_ptr = locate_add(mem, *reg2_ptr);
    uint16_t imm = cpu->ir.imm;
    uint16_t offset = cpu->ir.offset;
    uint16_t shift_reg = *reg2_ptr & 0x0F; // Masking to keep shift value the maximum 15 
    uint16_t shift_imm = imm & 0x0F; // shift by immediate value max 15
    uint16_t reg1_inital = *reg1_ptr;
    uint16_t br_condition = reg1_index;
    uint16_t temp = 0;

    switch(cpu->ir.opcode){
        case LD: // Mode : 2
            memcpy(reg1_ptr,addr_ptr + offset,WORD);
            break;
        case LDB: // Mode : 2
            memcpy(reg1_ptr,addr_ptr + offset,HWORD);
            break;
        case STR: // Mode : 2
            memcpy(addr_ptr+ offset,reg1_ptr,WORD);
            break;
        case STRB: // Mode : 2
            memcpy(addr_ptr + offset,reg1_ptr,HWORD);
            break;
        case MOV: // Mode : 0
            memcpy(reg1_ptr, reg2_ptr, WORD);
            break;
        case MOVI: // Mode : 1
            memcpy(reg1_ptr, &imm,WORD);
            break;
        /*---------------------------------------------*/
        case ADD: // Mode : 0
            *reg1_ptr += *reg2_ptr;

            (*reg2_ptr && *reg1_ptr <= reg1_inital)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags, FL_CARRY) ;
            break;
        case ADDI: // Mode : 1
            *reg1_ptr += imm;

            (imm && *reg1_ptr <= reg1_inital)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags, FL_CARRY) ;
            break;
        case ADDS: // Mode : 3
            *reg1_ptr = *reg2_ptr + *reg3_ptr;

            (*reg1_ptr <= *reg2_ptr)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags, FL_CARRY) ;
            break;
        case SUB: // Mode : 0
            *reg1_ptr -= *reg2_ptr;

            (*reg2_ptr && *reg1_ptr >= reg1_inital)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags,FL_CARRY) ;
            break;
        case SUBI: // Mode : 1
            *reg1_ptr -= imm;

            (imm && *reg1_ptr >= reg1_inital)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags,FL_CARRY) ;
            break;
        case SUBS: // Mode : 3
            *reg1_ptr = *reg2_ptr - *reg3_ptr;

            (*reg1_ptr >= *reg2_ptr)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags, FL_CARRY) ;
            break;
        /*---------------------------------------------*/
        case AND: // Mode : 2
            *reg1_ptr = *reg2_ptr & *reg3_ptr;
            break;
        case ANDI: // Mode : 2
            *reg1_ptr = *reg2_ptr & offset;

            break;
        case OR: // Mode : 3
            *reg1_ptr = *reg2_ptr | *reg3_ptr;

            break;
        case ORI: // Mode : 2
            *reg1_ptr = *reg2_ptr | offset;

            break;
        case ZOR: // Mode : 3
            *reg1_ptr = *reg2_ptr ^ *reg3_ptr;
            break;
        case ZORI: // Mode : 2
            *reg1_ptr = *reg2_ptr ^ offset;

            break;
        case CMP: // Mode : 0
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
        case CMPI: // Mode : 1
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
        case CMPW : // Mode: 2
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
        case CMPH:  // Mode: 2
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
        case SR: // Mode : 0
            *reg1_ptr = (uint16_t)(*reg1_ptr >> shift_reg); //Odd syntax in order to supress strict compiler warning

            (*reg2_ptr && reg1_inital & 0x01)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags,FL_CARRY) ; 
            break;
        case SRI: // Mode : 1
            *reg1_ptr = (uint16_t)(*reg1_ptr >> shift_imm);

            (imm && reg1_inital & 0x01)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags,FL_CARRY) ; 
            break;
        case SL: // Mode : 0
            *reg1_ptr = (uint16_t)(*reg1_ptr << shift_reg);

            (*reg2_ptr && reg1_inital >> 15 & 0x01)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags,FL_CARRY) ; 
            break;
        case SLI: // Mode : 1
            *reg1_ptr = (uint16_t)(*reg1_ptr << shift_imm);

            (imm && reg1_inital >> 15 & 0x01)? SET_BIT(cpu->flags, FL_CARRY) : CLEAR_BIT(cpu->flags,FL_CARRY) ; 
            break;
        /*---------------------------------------------*/
        case JMP:    // Mode : 1
            cpu->pc = imm;
            break;
        case BRC:    // Mode : 1
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
                   if(!MASK_BIT(cpu->flags,FL_NEGATIVE)){
                       temp = 1;
                   }
                   break;
               case CON_CARRY_SET:
                   if(MASK_BIT(cpu->flags,FL_NEGATIVE)){
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
        case CALL: // Mode : 0
            break;
        case RET:  // Mode : 0
            break;
        case PUSH:  // Mode : 0
            break;
        case POP:  // Mode : 0
            break;
        case NOP:  // Mode : 0
           // No Operation
            break;
        case KILL:  // Mode : 0
            RUN = 0;
            break;
        default:
            break;
    }
    // Generic-Flag Checks Exc. CMP*
    if(cpu->ir.opcode > 6 && (cpu->ir.opcode < 19 || cpu->ir.opcode > 21) ){
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
