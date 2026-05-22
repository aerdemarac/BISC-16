import struct
import sys
import os

BIN_FILE_PATH = "bin/counter.bin"

MOD_REG, MOD_IMM, MOD_OFS, MOD_TRI = 0, 1, 2, 3
GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7 = 0, 1, 2, 3, 4, 5, 6, 7
BP, SP, PC, UDP, FL, IGN = 8, 9, 10, 11, 12, 0
CON_NON_ZERO, CON_ZERO, CON_POSITIVE, CON_NEGATIVE, CON_CARRY_SET, CON_CARRY_CLEAR = 0, 1, 2, 3, 4, 5

OP_LD, OP_LDB, OP_STR, OP_STRB, OP_MOV, OP_MOVI = 0, 1, 2, 3, 4, 5
OP_ADD, OP_ADDI, OP_ADDS, OP_SUB, OP_SUBI, OP_SUBS = 6, 7, 8, 9, 10, 11
OP_AND, OP_ANDI, OP_OR, OP_ORI, OP_ZOR, OP_ZORI = 12, 13, 14, 15, 16, 17
OP_CMP, OP_CMPI, OP_CMPW, OP_CMPH = 18, 19, 20, 21
OP_SR, OP_SRI, OP_SL, OP_SLI = 22, 23, 24, 25
OP_JMP, OP_BRC, OP_CALL, OP_RET, OP_NOP = 26, 27, 28, 29, 30
OP_KILL, OP_SYSCALL = 31, 32
SYS_PUTCHAR, SYS_PUTS, SYS_CLEAR, SYS_SLEEP = 0, 1, 2, 3

def encoder_modR(op, r1, r2):       
    return ((op << 26) | (MOD_REG << 24) | (r1 << 20) | (r2 << 16)) & 0xFFFFFFFF

def encoder_modI(op, r1, imm):      
    return ((op << 26) | (MOD_IMM << 24) | (r1 << 20) | ((imm & 0xFFFF) << 4)) & 0xFFFFFFFF

def encoder_modO(op, r1, r2, ofs):  
    return ((op << 26) | (MOD_OFS << 24) | (r1 << 20) | (r2 << 16) | (ofs & 0xFFFF)) & 0xFFFFFFFF

def encoder_modT(op, r1, r2, r3):   
    return ((op << 26) | (MOD_TRI << 24) | (r1 << 20) | (r2 << 16) | (r3 << 12)) & 0xFFFFFFFF

def LD(r1, r2, ofs):     return encoder_modO(OP_LD, r1, r2, ofs)
def LDB(r1, r2, ofs):    return encoder_modO(OP_LDB, r1, r2, ofs)
def STR(r1, r2, ofs):    return encoder_modO(OP_STR, r1, r2, ofs)
def STRB(r1, r2, ofs):   return encoder_modO(OP_STRB, r1, r2, ofs)
def MOV(r1, r2):         return encoder_modR(OP_MOV, r1, r2)
def MOVI(r1, imm):       return encoder_modI(OP_MOVI, r1, imm)
def ADD(r1, r2):         return encoder_modR(OP_ADD, r1, r2)
def ADDI(r1, imm):       return encoder_modI(OP_ADDI, r1, imm)
def ADDS(r1, r2, r3):    return encoder_modT(OP_ADDS, r1, r2, r3)
def SUB(r1, r2):         return encoder_modR(OP_SUB, r1, r2)
def SUBI(r1, imm):       return encoder_modI(OP_SUBI, r1, imm)
def SUBS(r1, r2, r3):    return encoder_modT(OP_SUBS, r1, r2, r3)
def AND(r1, r2, r3):     return encoder_modT(OP_AND, r1, r2, r3)
def ANDI(r1, imm):       return encoder_modI(OP_ANDI, r1, imm)
def OR(r1, r2, r3):      return encoder_modT(OP_OR, r1, r2, r3)
def ORI(r1, imm):        return encoder_modI(OP_ORI, r1, imm)
def ZOR(r1, r2, r3):     return encoder_modT(OP_ZOR, r1, r2, r3)
def ZORI(r1, imm):       return encoder_modI(OP_ZORI, r1, imm)
def CMP(r1, r2):         return encoder_modR(OP_CMP, r1, r2)
def CMPI(r1, imm):       return encoder_modI(OP_CMPI, r1, imm)
def CMPW(r1, r2, ofs):   return encoder_modO(OP_CMPW, r1, r2,ofs)
def CMPH(r1, r2, ofs):   return encoder_modO(OP_CMPH, r1, r2, ofs)
def SR(r1, r2):          return encoder_modR(OP_SR, r1, r2)
def SRI(r1, imm):        return encoder_modI(OP_SRI, r1, imm)
def SL(r1, r2):          return encoder_modR(OP_SL, r1, r2)
def SLI(r1, imm):        return encoder_modI(OP_SLI, r1, imm)
def JMP(imm):            return encoder_modI(OP_JMP, IGN, imm)
def BRC(con, imm):       return encoder_modI(OP_BRC, con, imm)
def CALL(imm):           return encoder_modI(OP_CALL, IGN, imm)
def RET():               return encoder_modR(OP_RET, IGN, IGN)
def NOP():               return encoder_modR(OP_NOP, IGN, IGN)
def KILL():              return encoder_modR(OP_KILL, IGN, IGN)
def CALL_PUTCHAR(reg):   return encoder_modO(OP_SYSCALL, SYS_PUTCHAR, reg, IGN)
def CALL_PUTS(reg,ofs):  return encoder_modO(OP_SYSCALL, SYS_PUTS, reg, ofs)
def CALL_CLEAR():        return encoder_modO(OP_SYSCALL, SYS_CLEAR, IGN, IGN)
def CALL_SLEEP(ms):      return encoder_modO(OP_SYSCALL, SYS_SLEEP, IGN, ms)

#ROM Array to be written into BIN File
rom = [
    CALL_CLEAR(),
    MOVI(GP0, 0xA),
    MOVI(GP1, 0x30),
    MOVI(GP2, 0xA),
    MOVI(GP3, 0x30),
    MOVI(GP4, 0x31),
    ADD(GP1, GP0),
    CMPI(GP0, 10),
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
    BRC(CON_NON_ZERO, 28),
    KILL()
]
# Please run assembler.sh after inserting instruction macros int rom array

if __name__ == "__main__":
    dir_name = os.path.dirname(BIN_FILE_PATH)
    if dir_name and not os.path.exists(dir_name):
        os.makedirs(dir_name)

    binary_size_bytes = len(rom) * 4
    if binary_size_bytes > 5120:
        print("Error,ROM File Size Exceeded 5KB Limit!")
        sys.exit(1)

    with open(BIN_FILE_PATH, "wb") as f:
        for inst in rom:
            f.write(struct.pack("<I", inst))
            
    print(f"Generated counter.bin - Size: {binary_size_bytes} bytes.")
