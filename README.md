# rainfall

> Binary exploitation track — fourteen pwn challenges (10 mandatory + 4 bonus) with per-level walkthroughs.

## Overview

A series of progressively harder pwn challenges run as setuid binaries on a shared Linux box: each level has a single binary you must exploit to obtain the next user's credentials. The rainfall track covers stack and heap overflows, format strings, GOT overwrites and a C++ vtable hijack — the foundation for the 64-bit levels in [override](https://github.com/sungyongcho/override). The lab VMs ran with ASLR disabled, so none of this is a mitigation bypass.

This repo contains my work on the ten mandatory levels (`level0`–`level9`) and four bonus levels (`bonus0`–`bonus3`), with the original challenge source and per-level exploit notes preserved.

This project was built as part of the 42 school cybersecurity track with a partner ([Teo Fleming](https://github.com/mokolodi1)) · Score: 125/100.

## Tech Stack

| Layer | Technologies |
|-------|-------------|
| Architecture | x86 (32-bit ELF) |
| OS | Linux |
| Tools | GDB + PEDA, Ghidra, `objdump`, `strings` |
| Bug classes | Stack overflow, heap overflow, format string, GOT overwrite, vtable hijack |

## Key Features

- Stack buffer overflows — return-address and function-pointer overwrites
- Heap overflows — writing past one allocation into the next to control what the program does with it
- Format-string vulnerabilities — stack reads and `%n` writes
- GOT overwrites — redirecting a resolved libc call to the function you want
- A C++ vtable hijack — overwriting an object's vtable pointer to redirect a virtual call
- Reverse-engineering by hand using GDB + PEDA and Ghidra to recover each binary's logic

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

- **Bug-class fluency**: identifying stack and heap overflows, format-string bugs, GOT-overwrite and vtable primitives from the binary, and turning each into the next user's credentials.
- **Tool fluency**: using GDB + PEDA for live exploit development and Ghidra for static analysis of the binaries.
- **Mental model of memory**: how stacks and heap chunks are laid out, how the dynamic linker resolves symbols, and what each checksec column means for a given binary (ASLR was off on the lab VMs).

## Note

Solutions are published here as a record of completed coursework, post-graduation. They are not intended as walkthroughs for current students of the program.

## License

This project was built as part of the 42 school curriculum.

---

*Part of [sungyongcho](https://github.com/sungyongcho)'s project portfolio.*
