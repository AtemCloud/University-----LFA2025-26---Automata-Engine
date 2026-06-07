# Polymorphic Automata & Game Engine

A object-oriented C++ simulation engine that dynamically executes computational models (DFA, NFA, PDA) and interactive text adventure games using runtime configuration files.

## Features
- **DFA & NFA:** Standard state evaluation and non-deterministic branching ($\epsilon$-closures).
- **PDA:** Stack-based memory for validating context-free languages (e.g., $L = \{0^n1^n\}$).
- **Game Engine:** Inherits from the PDA to map states to rooms and the stack to an inventory system for interactive text adventures.

---

## Configuration Formats

The engine reads the first line (`DFA`, `NFA`, `PDA`, or `GAME`) to determine the module.

### 1. PDA Mode 

```text
PDA
STATES: 0, 1, 2
ALPHABET: 0, 1
STACK_START: $
START: 0
ACCEPT: 2
TRANSITIONS:
0, ~, $, 1, S$
1, ~, S, 1, 0S1
1, ~, S, 1, ~
1, 0, 0, 1, ~
1, 1, 1, 1, ~
1, ~, $, 2, ~
```

### 2. GAME Mode 

```text
GAME
STATES: 0, 1, 2, 3, 4, 5, 6, 7
ALPHABET: N, S, E, W
STACK_START: $
START: 0
ACCEPT: 6, 7
TRANSITIONS:
0, N, ~, 1, K
1, S, ~, 0, ~
0, W, K, 2, PK
0, W, $, 2, $
2, N, ~, 3, ~
3, N, ~, 4, ~
4, W, ~, 5, ~
5, ~, P, 6, ~
5, ~, K, 7, ~
5, ~, $, 7, ~
```

## Usage

### 1. Compile

```bash
g++ -std=c++17 main.cpp -o automata
```

### 2. Run

```bash
./automata file
```
