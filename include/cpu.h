#include <stddef.h>
#include <stdint.h>

/*4b-opcode + 2b-mode determines the instruction format
 *Mod Values: R=> 0 , I=> 1 , D=>2 , O=>3;
 * */

/*      Size Identifiers in Bytes   */
#define QWORD 8
#define DWORD 4
#define WORD 2
#define HWORD 1
/*         Modes           */
#define MOD_REG 0U
#define MOD_IMM 1U
#define MOD_OFS 2U
#define MOD_TRI 3U

/*      Field-Masking Macros*/
#define OP_FIELD(raw)   ((raw >> 26) & 0x1Fu)
#define MOD_FIELD(raw)  ((raw >> 24) & 0x3U) 
#define REG1_FIELD(raw) ((raw >> 20) & 0x0Fu) 
#define REG2_FIELD(raw) ((raw >> 16) & 0x0Fu)
#define REG3_FIELD(raw) ((raw >> 12) & 0x0Fu)
#define IMM_FIELD(raw)  ((raw >> 4) & 0xFFFF)
#define OFS_FIELD(raw)  ((raw >> 0) & 0xFFFF)

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
#define UDP  11U
#define FL  12U

/*         Flag-Macros           */
//Unsigned 
#define FL_ZERO 1 << 0 
#define FL_CARRY 1 << 1 
//Signed
#define FL_NEGATIVE 1 << 2
#define FL_OVERFLOW 1 << 3

/*   Unique Instruction Identifier opcode + mode */
#define LD   0U       // Mode: 3    // Tested      
#define LDB  1U       // Mode: 3    // Tested      
#define STR  2U       // Mode: 3    // Tested      
#define STRB 3U       // Mode: 3    // Tested      
#define MOV  4U       // Mode: 0    // Tested      
#define MOVI 5U       // Mode: 1    // Tested      
                      // Mode:         
#define ADD  6U       // Mode: 0     
#define ADDI 7U       // Mode: 1          
#define SUB  8U       // Mode: 0          
#define SUBI 9U       // Mode: 1          
                                    
#define AND  10U     //  Mode: 0         
#define ANDI 11U     //  Mode: 1         
#define OR   12U     //  Mode: 0         
#define ORI  13U     //  Mode: 1         
#define ZOR  14U     //  Mode: 0         
#define ZORI 15U     //  Mode: 1         
#define CMP  16U     //  Mode: 0         
#define CMPI 17U     //  Mode: 1         
#define SR   18U     //  Mode: 0         
#define SRI  19U     //  Mode: 1         
#define SL   20U     //  Mode: 0         
#define SLI  21U     //  Mode: 1         
                     //  Mode:         
#define BR   22U     //  Mode: 0         
#define CALL 23U     //  Mode: 0         
#define RET  24U     //  Mode: 0         
#define PUSH 25U     //  Mode: 0         
#define POP  26U     //  Mode: 0         
#define NOP  27U     //  Mode: 0         
                        //          
#define KILL ((28U << 2)//  | 0U)     //0x70

/*       Memory Mapping - Boundaries      */
#define MEM_SIZE 65536U
#define PROG_MIN 0U
#define PROG_MAX 5119U
#define USRDATA_MIN 5120U            // [PROGRAM-Region 5 KB MAX][USER-DATA Region - 58 KB MAX][STACK - 1 KB (- Grow)]
#define USRDATA_MAX 64511U
#define STACK_MIN 64512U
#define STACK_MAX 65535U


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

void write_reg(uint16_t* dest_ptr,void* src_ptr,size_t N);
void write_mem(uint8_t* dest_addr,void* src_add,size_t N);

void addition_reg(uint16_t* dest_reg,uint16_t* src_reg);
void subtraction_reg(uint16_t* dest_reg,uint16_t* src_reg);

uint32_t encoder_modR(uint8_t op,uint8_t reg1,uint8_t reg2);
uint32_t encoder_modI(uint8_t op,uint8_t reg1,uint16_t imm);
uint32_t encoder_modT(uint8_t op,uint8_t reg1,uint8_t reg2,uint8_t reg3);
uint32_t encoder_modO(uint8_t op,uint8_t reg1,uint8_t reg2,uint16_t ofs);

/*---------------------------------------------------------------*/
uint8_t* read_mem_hword(MemByte* mem,uint16_t mem_addr); // LEGACY
uint16_t* read_mem_word(MemByte* mem,uint16_t mem_addr); // LEGACY
/*---------------------------------------------------------------*/
