![Banner](https://i.imgur.com/8d3Yo3X.png)
![C](https://img.shields.io/badge/language-C-blue)
![Standard](https://img.shields.io/badge/ISO-C99-green)
![MIT](https://img.shields.io/badge/license-MIT-yellow)



# BISC-16 | RISC-Style ISA & Deterministic CPU Design

## Overview

**BISC-16**, an 16-bit CPU architecture project designed for educational and systems-level experimentation. The goal of this project is to build a minimal but expressive processor model that helps explore how real hardware concepts such as instruction execution, memory organization, and register design work at a fundamental level.

This project intentionally avoids high-level abstractions like toolchains or compiler assumptions, focusing instead on raw instruction semantics and memory behavior.

---

## Design Philosophy

BISC-16 is built around a few core principles:

- **Simplicity over complexity**
  With the help of RISC-style instruction-set and clean CPU design, every aspect of BISC-16 pretty simple and predictable  

- **Deterministic execution**
  Every instruction has a clearly defined behavior without hidden side effects.

- **Nothing more than a CPU**
  BISC-16 is never meant to produce fully graphical outputs therefore not equipped with Graphical Libs, ABIs or Wrapper Units, in order to comply plain CPU and ISA design philosophy

---

## Execution Model

The CPU operates on a classic cycle:

Fetch → Decode → Execute → Repeat

---

## Registers Model

BISC-16 equipped with 8 general, 5 special purpose registers:

- **8 General Purpose Registers** =>                (uint16_t[]) 
- **Program Counter(PC)**  =>                       (uint16_t)
- **Stack Base Pointer(BP) , Stack Pointer(SP)** => (uint16_t)
- **User Data Pointer (UDP)** =>                    (uint16_t)
- **Flags Register (FL)** =>                        (uint16_t)

UDP is a special register that retains the memory address to manually managed section where program data stored such as variables,strings etc.

---

## Memory Model

The 64KB memory space is logically divided into:

- **Program section** (.text Equvilant) =>                                    [0x0000  - 0x13FF]
- **General-purpose memory region** (Manually Managed Program Data Section => [0x1400  - 0xFBFF]
- **Stack region** (Grows downward) =>                                        [0xFC00  - 0xFFFF]

Stack Design is kept simple therefore its only capable of storing return addresses of function calls

---

## Decoding Modes

BISC-16 is equipped with 4 decoding modes:

- **Mode-R** (Double-Registers Mode) = Used for instructions requires 2 register identifier arguments. e.g. ADD,CMP,MOV 
- **Mode-I** (Immediate Mode) = Used when instruction operates with 1 register identifier and a 16-Bit immediate value e.g. MOVI,CMPI,SUBI
- **Mode-O** (Offset Mode) = Mostly used in memory-related operations. Contains a destination register, source address register and an 16-Bit offset value e.g. LD,STR,CMPW
- **Mode-T** (Tri-Register Mode) = Dedicated to instructions requires 1 destination and 2 operand registers e.g. SUBS, OR, ZOR

---

## Instruction Classes

- **Memory Load-Store Operations** e.g. LDB,STRB
- **Data Movement on Registers** e.g. MOV,MOVI 
- **Arithmetic Operations** e.g. SUBI,ADD
- **Logical Operations** e.g. SR,CMP,OR
- **Flow Control** e.g. JMP,BRC,RET
- **System Call** e.g. SYSCALL

---
## Inline Assembler
**BISC-16** is equipped a inline assembler:
- **Assembler** is basically a python program contains instruction-encoder methods
- **An array of bytes** called 'rom' is initalized via mere encoder functions
- **Then** written into a binary file.
- **Please** insert the instructions into bytearray located in the **'auxiliary/assembler.py'**
- **Then run './assembler.sh'** in order to generate binary file 
- **ROM file** outputs will be located at bin/

---

## TO-DO

- Couple of demo ROMs
- Comperhensive CPU documentation

---

## Documents Consumed During Development
- Established decent understanding of CPU architectures via succesors such as **MOS6502, Motorolla 68K, Zilog Z80**
- https://www.zilog.com/docs/z80/um0080.pdf
- http://retro.hansotten.nl/uploads/books/6502UsersManual.pdf
- https://icourse.club/uploads/files/a9710bf2454961912f79d89b25ba33c4841f6c24.pdf
- https://deramp.com/downloads/intel/8080%20Data%20Sheet.pdf
- https://rohan-sharma.de/blog/32-bit-cpu/

