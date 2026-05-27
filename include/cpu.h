#include <stddef.h>
#include <stdint.h>

#ifndef CPU_H
#define CPU_H

/*4b-opcode + 2b-mode determines the instruction format
 *Mod Values: R=> 0 , I=> 1 , D=>2 , O=>3;
 * */

/*Helper Parametized Macros*/
#define SET_BIT(field,bit)  ((field) |= (1U << (bit)))
#define CLEAR_BIT(field,bit) ((field) &= ~(1U << (bit)))
#define MASK_BIT(field,bit) (((field) >> (bit) & 0x1U))
#define CLEAR_FL(flags) (flags &= 0x0U)

/*      Size Identifiers in Bytes   */
#define QWORD 8U
#define DWORD 4U
#define WORD  2U
#define HWORD 1U

/*         Register-Macros           */
#define GP0 0U
#define GP1 1U
#define GP2 2U
#define GP3 3U
#define GP4 4U
#define GP5 5U
#define GP6 6U
#define GP7 7U
#define BP  8U
#define SP  9U
#define PC  10U
#define UDP 11U
#define FL  12U

/*         Flag-Macros           */
//Unsigned 
#define FL_ZERO     0 //0th Bit
#define FL_CARRY    1 //1th Bit
//Signed
#define FL_NEGATIVE 2 //2nd Bit

/*         Condition Specifier-Macros           */
#define CON_NON_ZERO    0
#define CON_ZERO        1
#define CON_POSITIVE    2
#define CON_NEGATIVE    3
#define CON_CARRY_SET   4
#define CON_CARRY_CLEAR 5

/*         2's Complement Sign Bit */
#define SIGN_BIT 0x8000

/*         Modes           */
#define MOD_REG 0U
#define MOD_IMM 1U
#define MOD_OFS 2U
#define MOD_TRI 3U

/*      Field-Masking Macros*/
#define OP_FIELD(raw)   ((raw >> 26) & 0xFFu)
#define MOD_FIELD(raw)  ((raw >> 24) & 0x3U) 
#define REG1_FIELD(raw) ((raw >> 20) & 0x0Fu) 
#define REG2_FIELD(raw) ((raw >> 16) & 0x0Fu)
#define REG3_FIELD(raw) ((raw >> 12) & 0x0Fu)
#define IMM_FIELD(raw)  ((raw >> 4) & 0xFFFF)
#define OFS_FIELD(raw)  ((raw >> 0) & 0xFFFF)

#define IGN 0 // Meaning: Ignore this instruction Filed


/*   Unique Instruction Identifier opcode + mode */
#define OP_LD   0U      // Mode: 2    
#define OP_LDB  1U      // Mode: 2    
#define OP_STR  2U      // Mode: 2    
#define OP_STRB 3U      // Mode: 2    
#define OP_MOV  4U      // Mode: 0    
#define OP_MOVI 5U      // Mode: 1    

#define OP_ADD  6U      // Mode: 0    
#define OP_ADDI 7U      // Mode: 1    
#define OP_ADDS 8U      // Mode: 3    
#define OP_SUB  9U      // Mode: 0    
#define OP_SUBI 10U     // Mode: 1    
#define OP_SUBS 11U     // Mode: 3    

#define OP_AND  12U     // Mode: 3    
#define OP_ANDI 13U     // Mode: 2    
#define OP_OR   14U     // Mode: 3    
#define OP_ORI  15U     // Mode: 2    
#define OP_ZOR  16U     // Mode: 3    
#define OP_ZORI 17U     // Mode: 2    
#define OP_CMP  18U     // Mode: 0    
#define OP_CMPI 19U     // Mode: 1    
#define OP_CMPW 20U     // Mode: 2    
#define OP_CMPH 21U     // Mode: 2    
#define OP_SR   22U     // Mode: 0    
#define OP_SRI  23U     // Mode: 1    
#define OP_SL   24U     // Mode: 0    
#define OP_SLI  25U     // Mode: 1    

#define OP_JMP  26U     // Mode: 1      
#define OP_BRC  27U     // Mode: 1      
#define OP_CALL 28U     // Mode: 1      
#define OP_RET  29U     // Mode: 0      
#define OP_NOP  30U     // Mode: 0      

#define OP_KILL 31U     // Mode: 0
#define OP_SYSCALL 32U     // Mode: 2

/*       Memory Mapping - Boundaries      */
#define MEM_SIZE    0x10000U // 64KB
#define PROG_MIN    0x0U
#define PROG_MAX    0x13FFU
#define USRDATA_MIN 0x1400U // [PROGRAM-Region 5 KB MAX][USER-DATA Region - 58 KB MAX][STACK - 1 KB (- Grow)]
#define USRDATA_MAX 0xFBFFU
#define STACK_MIN   0xFC00U
#define STACK_MAX   0xFFFFU

/* System-Call Functionalities*/
#define SYS_PUTCHAR 0U
#define SYS_PUTS 1U
#define SYS_CLEAR 2U
#define SYS_SLEEP 3U


typedef struct CPU CPU;
typedef struct Instruction Instruction;
typedef uint8_t MemByte;

//Methods
CPU* cpu_init(void);
MemByte* mem_init(void);

void cpu_free(CPU** cpu_ptr);
void mem_free(MemByte** mem_ptr);

void fetch(CPU* cpu, MemByte* mem);
void decode(CPU* cpu);
void execute(CPU* cpu,MemByte* mem);

uint16_t* locate_reg(CPU* cpu,uint8_t reg);
uint8_t* locate_add(MemByte* mem,uint16_t add_val);

// Debug-Helper Quick Instruction Generation
uint32_t encoder_modR(uint8_t op,uint8_t reg1,uint8_t reg2);
uint32_t encoder_modI(uint8_t op,uint8_t reg1,uint16_t imm);
uint32_t encoder_modT(uint8_t op,uint8_t reg1,uint8_t reg2,uint8_t reg3);
uint32_t encoder_modO(uint8_t op,uint8_t reg1,uint8_t reg2,uint16_t ofs);

void clear_decoded_fields(CPU* cpu);

/*---------------------------------------------------------------*/
uint8_t* read_mem_hword(MemByte* mem,uint16_t mem_addr); // LEGACY
uint16_t* read_mem_word(MemByte* mem,uint16_t mem_addr); // LEGACY
/*---------------------------------------------------------------*/
#endif
