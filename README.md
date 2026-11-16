# StackLab — Applications of the Stack

A menu-driven C program that shows the **Stack (LIFO)** data structure solving real problems, with step-by-step traces and a colourful console UI.

Built for the **Data Structures & Algorithms Lab**.

```
   ____  _             _      _           _
  / ___|| |_ __ _  ___| | __ | |    __ _ | |__
  \___ \| __/ _` |/ __| |/ / | |   / _` || '_ \
   ___) | || (_| | (__|   <  | |__| (_| || |_) |
  |____/ \__\__,_|\___|_|\_\ |_____\__,_||_.__/
        Applications of the Stack  |  LIFO in action
```

## Features

| # | Module | What it does | Example |
|---|--------|--------------|---------|
| 1 | **Stack Playground** | Push, pop and peek with a live visual of the stack, plus overflow / underflow handling | — |
| 2 | **Balanced Brackets** | Checks `()`, `{}`, `[]` and reports the exact position of a mismatch | `{a+(b*c)}` → Balanced |
| 3 | **Infix → Postfix** | Converts using operator precedence and associativity, showing the stack at every step | `A+B*(C-D)/E` → `A B C D - * E / +` |
| 4 | **Postfix Evaluation** | Evaluates multi-digit postfix expressions (`+ - * / % ^`) | `12 3 4 * + 6 -` → `18` |
| 5 | **String Reversal** | Pushes every character, then pops them in reverse | `hello` → `olleh` |
| 6 | **Undo / Redo Editor** | A mini text editor that uses **two stacks** for history, just like real editors | — |

## Sample run

```
  +------------------------------------------+
  |  3. INFIX  ->  POSTFIX                   |
  +------------------------------------------+

  Enter infix expression > A + B * (C - D) / E

  tok | stack        | output
  ----+--------------+----------------------
  A   |              | A
  +   | +            | A
  B   | +            | A B
  *   | +*           | A B
  (   | +*(          | A B
  C   | +*(          | A B C
  -   | +*(-         | A B C
  D   | +*(-         | A B C D
  )   | +*           | A B C D -
  /   | +/           | A B C D - *
  E   | +/           | A B C D - * E

  Postfix: A B C D - * E / +
```

```
  Stack Playground

        |     30 |  <- TOP
        |     20 |
        |     10 |
        +--------+
        size: 3 / 100
```

## How to run

**Dev-C++**
1. Open `stack_app.c` (File → Open).
2. Press **F11** (Compile & Run).

**GCC (any OS)**
```bash
gcc stack_app.c -o stack_app
./stack_app          # on Windows: stack_app.exe
```

Colours work on Windows 10/11, Linux and macOS terminals.

## How it works

The core is a simple **array-based stack**:

```c
typedef struct {
    int top;
    long items[MAX];
} Stack;
```

| Operation | Description | Time |
|-----------|-------------|------|
| `push`    | Add an element to the top | O(1) |
| `pop`     | Remove and return the top element | O(1) |
| `peek`    | Read the top element without removing it | O(1) |
| `isEmpty` / `isFull` | Underflow / overflow checks | O(1) |

Every application runs in **O(n)** time, where *n* is the length of the input.

**Operator precedence used in Infix → Postfix**

| Operator | Precedence | Associativity |
|----------|-----------|---------------|
| `^`      | 3 (highest) | Right |
| `* / %`  | 2 | Left |
| `+ -`    | 1 | Left |

## Project structure

```
stack-app/
├── stack_app.c   # all source code (stack + 6 applications + menu)
└── README.md
```

## Author

**Dinesh Aryan M**
