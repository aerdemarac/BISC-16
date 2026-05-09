![Banner](https://i.imgur.com/xBsJH2F.png)
![C](https://img.shields.io/badge/language-C-blue)
![Standard](https://img.shields.io/badge/ISO-C99-green)
![MIT](https://img.shields.io/badge/license-MIT-yellow)



# BISC-16

## Overview

**BISC-16** is an under-development 16-bit CPU architecture project designed for educational and systems-level experimentation. The goal of this project is to build a minimal but expressive processor model that helps explore how real hardware concepts such as instruction execution, memory organization, and register design work at a fundamental level.

This project intentionally avoids high-level abstractions like toolchains or compiler assumptions, focusing instead on raw instruction semantics and memory behavior.

---

## Design Philosophy

BISC-16 is built around a few core principles:

- **Simplicity over complexity**
  The architecture is intentionally minimal to make every hardware decision visible and understandable.

- **Deterministic execution**
  Every instruction has a clearly defined behavior without hidden side effects.

- **Low-level transparency**
  The CPU does not interpret memory as “code” or “data”; it only sees addresses and instructions.

- **Educational focus**
  The design is meant to demonstrate how real CPU concepts emerge from basic building blocks.

---

## Execution Model

The CPU operates on a classic cycle:

Fetch → Decode → Execute → Repeat

This cycle forms the foundation of all instruction processing in the architecture.

---

## Memory Model

The memory space is logically divided into:

- Program section (instruction memory)
- General-purpose memory region
- Stack region (manually managed, grows downward)

The CPU does not enforce semantic meaning on memory regions; instead, structure is defined by convention and enforced by software-level discipline.

---

## Architectural Intent

BISC-16 is not designed to compete with modern ISAs or production systems. Instead, it serves as a controlled environment to explore:

- Instruction set design trade-offs
- Register allocation strategies
- Memory access patterns
- Stack behavior and calling conventions (at a minimal level)
- Endianness and data representation at hardware level

---


## TODO

- Ability to fetch code from ROM files
- Full Assembler
- Couple of demo ROMs
- Extensive SysCall capabilites
- Comperhensive CPU documentation

---

## Documents Consumed During Development
- https://www.zilog.com/docs/z80/um0080.pdf
- http://retro.hansotten.nl/uploads/books/6502UsersManual.pdf
- https://icourse.club/uploads/files/a9710bf2454961912f79d89b25ba33c4841f6c24.pdf
- https://deramp.com/downloads/intel/8080%20Data%20Sheet.pdf
- https://rohan-sharma.de/blog/32-bit-cpu/

