# Forge-Compiler 🔨

A next-level project to build a compiler from scratch in **C** and **Assembly**, designed to compile **C/C++ code** into efficient machine code.  
This project is not just about writing a toy lexer or parser — it is a full journey into the internals of how compilers work, from raw source code down to assembly instructions.  

---

## 🌟 Project Vision
The goal of this project is to create a modern compiler that demonstrates the **complete pipeline of compilation**, while being built from the ground up without relying on heavy external libraries.  
Along the way, this project will serve as a deep learning exercise in language design, parsing, optimization, and low-level system programming.  

---

## Roadmap

The development will follow the classical **structure of a modern compiler**:

### 1. Source Code
The input will be C or C++ source code provided by the user.

---

### 2. Frontend

#### **Lexical Analysis**
- Transform the raw source code into a sequence of tokens.
- Tokens represent keywords, identifiers, literals, operators, and more.

#### **Syntax Analysis**
- Parse the token stream using grammar rules.
- Build an **Abstract Syntax Tree (AST)** to represent the program structure.

#### **Semantic Analysis**
- Perform type checking, scope resolution, and enforce language rules.
- Construct a **symbol table** for identifiers, functions, and types.

#### **Intermediate Representation (IR) Generation**
- Convert the AST into an **Intermediate Representation (IR)** that abstracts away high-level syntax and prepares for optimization.

---

### 3. Backend

#### **IR Optimization**
- Apply optimizations on the IR (e.g., constant folding, dead code elimination).
- Prepare the IR for target machine instructions.

#### **Code Generation**
- Translate IR into **x86-64 Assembly** (and potentially other architectures in the future).
- Handle registers, memory layout, and calling conventions.

#### **Final Optimization**
- Optimize the assembly/machine code for efficiency and performance.

---

### 4. Machine Code
- The final stage produces executable **machine code** that can run directly on the target system.

---

## Future Goals
- Extend support for additional architectures (ARM, RISC-V).
- Experiment with modern compiler design techniques.
- Build a lightweight standard library for compiled programs.

---

## Repository Status
This is the **initial stage** of the project.  
The first milestone is to implement the **Lexer (Lexical Analyzer)** (brach: `lexer`) using two different approaches:  
1. Regex-based  
2. State-machine based (without regex)  

---

## Contributors
This project is developed as part of a compiler design learning journey.  

