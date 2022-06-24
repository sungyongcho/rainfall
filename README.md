# rainfall

> Binary exploitation track — fourteen pwn challenges (10 mandatory + 4 bonus) with per-level walkthroughs.

## Overview

A series of progressively harder pwn challenges run as setuid binaries on a shared Linux box: each level has a single binary you must exploit to obtain the next user's credentials. The rainfall track focuses on stack-based vulnerabilities, format strings, GOT manipulation, and return-to-libc — a foundation for the more advanced heap-exploitation work in [override](https://github.com/sungyongcho/override).

This repo contains my work on the ten mandatory levels (`level0`–`level9`) and four bonus levels (`bonus0`–`bonus3`), with the original challenge source and per-level exploit notes preserved.

This project was built as part of the 42 school cybersecurity track with a partner ([Teo Fleming](https://github.com/mokolodi1)) · Score: 125/100.

## Tech Stack

| Layer | Technologies |
|-------|-------------|
| Architecture | x86 (32-bit ELF) |
| OS | Linux |
| Tools | GDB + PEDA, Ghidra, `objdump`, `strings` |
| Bug classes | Stack overflow, format string, GOT, ret2libc |

## Key Features

- Stack buffer overflows — return-address and function-pointer overwrites
- Format-string vulnerabilities — arbitrary read/write via `%n` primitives
- GOT overwrites — hijacking dynamic-linker resolution to redirect execution
- Return-to-libc — chaining libc calls to bypass non-executable stacks
- Reverse-engineering by hand using GDB + PEDA and Ghidra to recover decompiled C from stripped binaries

## Architecture

```
rainfall/
├── level0/
│   ├── source            # original challenge source
│   ├── flag              # captured credentials
│   └── walkthrough.MD    # my exploit notes
├── level1..9/            # mandatory levels
├── bonus0..3/            # bonus levels
├── ghidra_url.MD         # link to Ghidra (NSA's reverse-engineering toolkit)
├── peda_setup.sh         # GDB-PEDA installation helper
└── README.md
```

## What This Demonstrates

- **Bug-class fluency**: identifying stack overflows, format-string bugs, and GOT-overwrite primitives from a binary alone — without source — and chaining them into reliable exploits.
- **Tool fluency**: using GDB + PEDA for live exploit development and Ghidra for static analysis of stripped binaries.
- **Mental model of memory**: how stacks are laid out, how the dynamic linker resolves symbols, where defenses are and aren't, and which bypasses each defense allows.

## Note

Solutions are published here as a record of completed coursework, post-graduation. They are not intended as walkthroughs for current students of the program.

## License

This project was built as part of the 42 school curriculum.

---

*Part of [sungyongcho](https://github.com/sungyongcho)'s project portfolio.*
