#include "cpu.h"

/* Quick Encoder Macros Header File: 
 * Macros provided by this header file meant to be used when quick instruction generation is needed.
 * Ready to use when a method faster than ROM Generator needed (assembler.sh)
 * Mostly used for debugging purposes
*/

#ifndef QUICK
#define QUICK
#define LD(reg1,reg2,offset)     (encoder_modO(OP_LD,reg1,reg2,offset)) 
#define LDB(reg1,reg2,offset)    (encoder_modO(OP_LDB,reg1,reg2,offset)) 
#define STR(reg1,reg2,offset)    (encoder_modO(OP_STR,reg1,reg2,offset))
#define STRB(reg1,reg2,offset)   (encoder_modO(OP_STRB,reg1,reg2,offset))
#define MOV(reg1,reg2)           (encoder_modR(OP_MOV,reg1,reg2))
#define MOVI(reg1,imm)           (encoder_modI(OP_MOVI,reg1,imm))

#define ADD(reg1,reg2)           (encoder_modR(OP_ADD,reg1,reg2))
#define ADDI(reg1,imm)           (encoder_modI(OP_ADDI,reg1,imm))
#define ADDS(reg1,reg2,reg3)     (encoder_modT(OP_ADDS,reg1,reg2,reg3)) 
#define SUB(reg1,reg2)           (encoder_modR(OP_SUB,reg1,reg2))
#define SUBI(reg1,imm)           (encoder_modI(OP_SUBI,reg1,imm))
#define SUBS(reg1,reg2,reg3)     (encoder_modT(OP_SUBS,reg1,reg2,reg3)) 

#define AND(reg1,reg2,reg3)      (encoder_modT(OP_AND,reg1,reg2,reg3))
#define ANDI(reg1,reg2,imm)      (encoder_modO(OP_ANDI,reg1,reg2,imm))                                     
#define OR(reg1,reg2,reg3)       (encoder_modT(OP_OR,reg1,reg2,reg3))
#define ORI(reg1,reg2,imm)       (encoder_modO(OP_ORI,reg1,reg2,imm))
#define ZOR(reg1,reg2,reg3)      (encoder_modT(OP_ZOR,reg1,reg2,reg3))
#define ZORI(reg1,reg2,imm)      (encoder_modO(OP_ZORI,reg1,reg2,imm))                                     
#define CMP(reg1,reg2)           (encoder_modR(OP_CMP,reg1,reg2))
#define CMPI(reg1,imm)           (encoder_modI(OP_CMPI,reg1,imm))
#define CMPW(reg1,reg2,offset)   (encoder_modO(OP_CMPW,reg1,reg2,offset))
#define CMPH(reg1,reg2,offset)   (encoder_modO(OP_CMPH,reg1,reg2,offset))
#define SR(reg1,reg2)            (encoder_modR(OP_SR,reg1,reg2))
#define SRI(reg1,imm)            (encoder_modI(OP_SRI,reg1,imm))
#define SL(reg1,reg2)            (encoder_modR(OP_SL,reg1,reg2))
#define SLI(reg1,imm)            (encoder_modI(OP_SLI,reg1,imm))

#define JMP(imm)                 (encoder_modI(OP_JMP,IGN,imm))
#define BRC(con,imm)             (encoder_modI(OP_BRC,con,imm))
#define CALL(imm)                (encoder_modI(OP_CALL,IGN,imm)) 
#define RET()                    (encoder_modR(OP_RET,IGN,IGN))
#define NOP()                    (encoder_modR(OP_NOP,IGN,IGN))

#define KILL()                   (encoder_modR(OP_KILL,IGN,IGN))
#define __SYSCALL(call,reg2,imm)   (encoder_modO(OP_SYSCALL,call,reg2,imm))

/* Wrapper SYSCALL Methods */
#define CALL_PUTCHAR(reg) (__SYSCALL(SYS_PUTCHAR,reg,IGN))
#define CALL_PUTS(reg) (__SYSCALL(SYS_PUTS,reg,IGN))
#define CALL_CLEAR() (__SYSCALL(SYS_CLEAR,IGN,IGN))
#define CALL_SLEEP(ms) (__SYSCALL(SYS_SLEEP,IGN,ms))
#endif
