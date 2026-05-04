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

## Status

This project is currently in active development. The instruction set, memory model, and execution rules are still subject to change as the architecture evolves.

---

## Notes

BISC-16 deliberately avoids relying on external toolchains or compiler assumptions. The system is being built from the ground up to reflect a clean and explicit hardware model where every operation is defined at the ISA level.

---
