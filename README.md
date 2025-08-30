
# Forge-Compiler 🔨

A next-level project to build a compiler from scratch in **C** and **Assembly**, designed to compile **C/C++ code** into efficient machine code.  
This project is not just about writing a toy lexer or parser — it is a full journey into the internals of how compilers work, from raw source code down to assembly instructions.  

---

## 🌟 Project Vision
The goal of this project is to create a modern compiler that demonstrates the **complete pipeline of compilation**, while being built from the ground up without relying on heavy external libraries.  
Along the way, this project will serve as a deep learning exercise in language design, parsing, optimization, and low-level system programming.  

---

## 🛤️ Roadmap

The development will follow the classical **structure of a modern compiler**:

```text
                       ┌───────────────┐
                       │   Source Code │
                       └───────┬───────┘
                               │
                               ▼
                       ┌───────┴────────┐
                       │    Frontend    │  (C)
                       └───────┬────────┘
                               │
                  ┌────────────┼──────────────┐
                  ▼            ▼              ▼
              Lexical      Syntax          Semantic
              Analysis     Analysis        Analysis
              (C)          (C)             (C)
                               │
                               ▼
                     ┌─────────┴──────────┐
                     │  IR Generation (C) │
                     └─────────┬──────────┘
                               │
                               ▼
                       ┌───────┴────────┐
                       │    Backend     │
                       └───────┬────────┘
                               │
                  ┌────────────┼──────────────┐
                  ▼            ▼              ▼
              IR Opt.      Code Gen.     Final Opt.
              (C)          (C→ASM)       (ASM)
                               │
                               ▼
                        ┌──────┴───────┐
                        │ Machine Code │
                        └──────────────┘
````

---

### 🔎 Phase Breakdown with Languages

* **Frontend (C)**

  * Lexical Analysis → written in **C**
  * Syntax Analysis → written in **C**
  * Semantic Analysis → written in **C**
  * IR Generation → written in **C**

* **Backend (C + Assembly)**

  * IR Optimization → **C**
  * Code Generation → **C outputs x86-64 Assembly**
  * Final Optimization → **hand-tuned Assembly**

* **Machine Code**

  * Assembly is assembled and linked into raw **machine code** (binary executable).

---

## 🚀 Future Goals

* Extend support for additional architectures (ARM, RISC-V).
* Experiment with modern compiler design techniques.
* Build a lightweight standard library for compiled programs.

---

## 📌 Repository Status

This is the **initial stage** of the project.
The first milestone is to implement the **Lexer (Lexical Analyzer)** using two different approaches:

1. Regex-based
2. State-machine based (without regex)

---

## 🤝 Contributors

This project is developed as part of a compiler design learning journey.
- [Ayesha Siddiqa](https://github.com/AS-0167)
- [Talha Sikandar](https://github.com/TalhaSikandar)
- [Izza Arooj](https://github.com/IzzaArooj01)
- [Chauhdry Ahmad](https://github.com/ChauhdryAhmad)

---

