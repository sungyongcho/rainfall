# General introduction

Welcome to our Rainfall walkthrough!

## The team

While I am correcting this project on my own at 42 Paris, I did this project's exercises with [Sungyong (@sucho)](https://profile.intra.42.fr/users/sucho), and this GitHub repo has been a collaboration between both of us.

## Setup

### The .iso

We set up a VirtualBox with the `.iso` and created a port forwarding rule to forward port `4444` to `4242` to make it easier to work with the virtual machine.

### PEDA

In this project, we used PEDA to help understand and exploit binary files. PEDA stands for Python Exploit Development Assistance for GDB, and more information can be found [here](https://github.com/longld/peda).

Setup instructions:
```
git clone https://github.com/longld/peda.git

# password: level0
scp -P 4444 -r peda/ level0@localhost:/tmp/

chmod +rwx /tmp/peda/; chmod +rwx ~; echo "source /tmp/peda/peda.py" > ~/.gdbinit
```

## General process

In solving Rainfall's puzzles (including level 0), we generally relied on the following process to make progress:

1. Try running the binary with various arguments and note the results
2. Use GDB to disassemble the functions we found with `info functions` in GDB.
3. Manually reverse-engineer the assembly into something similar in C
4. Research any vulnerable functions found in the disassembly/reverse-engineering (`printf`, `strcpy`, etc.)
5. Use GDB and PEDA while the program is running to trace functionality view stack/memory addresses.
6. Exploit any vulnerabilities found
7. Document our findings and successes (not so much our failures, of which there were many)

## Caveats

Any reverse-engineered C code in this project is not necessarily complete and has not been tested. It serves only to better understand the respective binary's functionality.

# Level 0

## The exploit

Once logged in, you will see a binary called `level0`.

```
ssh level0@localhost -p 4444
	  _____       _       ______    _ _
	 |  __ \     (_)     |  ____|  | | |
	 | |__) |__ _ _ _ __ | |__ __ _| | |
	 |  _  /  _` | | '_ \|  __/ _` | | |
	 | | \ \ (_| | | | | | | | (_| | | |
	 |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                 Good luck & Have fun

  To start, ssh with level0/level0 on localhost:4242
level0@localhost's password:
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX enabled    No PIE          No RPATH   No RUNPATH   /home/user/level0/level0
level0@RainFall:~$ ls
level0
level0@RainFall:~$ ls -l
total 732
-rwsr-x---+ 1 level1 users 747441 Mar  6  2016 level0
```

XXX: add something about the s flag

Let's check the binary with the GDB...

```
level0@RainFall:~$ gdb level0
GNU gdb (Ubuntu/Linaro 7.4-2012.04-0ubuntu2.1) 7.4-2012.04
Copyright (C) 2012 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "i686-linux-gnu".
For bug reporting instructions, please see:
<http://bugs.launchpad.net/gdb-linaro/>...
Reading symbols from /home/user/level0/level0...(no debugging symbols found)...done.
gdb-peda$
gdb-peda$ set disassembly-flavor intel  # NOTE: this is before we found PEDA
gdb-peda$ disassemble main
Dump of assembler code for function main:
   0x08048ec0 <+0>:	push   ebp
   0x08048ec1 <+1>:	mov    ebp,esp
   0x08048ec3 <+3>:	and    esp,0xfffffff0
   0x08048ec6 <+6>:	sub    esp,0x20
   0x08048ec9 <+9>:	mov    eax,DWORD PTR [ebp+0xc]
   0x08048ecc <+12>:	add    eax,0x4
   0x08048ecf <+15>:	mov    eax,DWORD PTR [eax]
   0x08048ed1 <+17>:	mov    DWORD PTR [esp],eax
   0x08048ed4 <+20>:	call   0x8049710 <atoi>
   0x08048ed9 <+25>:	cmp    eax,0x1a7
   0x08048ede <+30>:	jne    0x8048f58 <main+152>
   0x08048ee0 <+32>:	mov    DWORD PTR [esp],0x80c5348
   0x08048ee7 <+39>:	call   0x8050bf0 <strdup>
   0x08048eec <+44>:	mov    DWORD PTR [esp+0x10],eax
   0x08048ef0 <+48>:	mov    DWORD PTR [esp+0x14],0x0
   0x08048ef8 <+56>:	call   0x8054680 <getegid>
   0x08048efd <+61>:	mov    DWORD PTR [esp+0x1c],eax
   0x08048f01 <+65>:	call   0x8054670 <geteuid>
   0x08048f06 <+70>:	mov    DWORD PTR [esp+0x18],eax
   0x08048f0a <+74>:	mov    eax,DWORD PTR [esp+0x1c]
   0x08048f0e <+78>:	mov    DWORD PTR [esp+0x8],eax
   0x08048f12 <+82>:	mov    eax,DWORD PTR [esp+0x1c]
   0x08048f16 <+86>:	mov    DWORD PTR [esp+0x4],eax
   0x08048f1a <+90>:	mov    eax,DWORD PTR [esp+0x1c]
   0x08048f1e <+94>:	mov    DWORD PTR [esp],eax
   0x08048f21 <+97>:	call   0x8054700 <setresgid>
   0x08048f26 <+102>:	mov    eax,DWORD PTR [esp+0x18]
   0x08048f2a <+106>:	mov    DWORD PTR [esp+0x8],eax
   0x08048f2e <+110>:	mov    eax,DWORD PTR [esp+0x18]
   0x08048f32 <+114>:	mov    DWORD PTR [esp+0x4],eax
   0x08048f36 <+118>:	mov    eax,DWORD PTR [esp+0x18]
   0x08048f3a <+122>:	mov    DWORD PTR [esp],eax
   0x08048f3d <+125>:	call   0x8054690 <setresuid>
   0x08048f42 <+130>:	lea    eax,[esp+0x10]
   0x08048f46 <+134>:	mov    DWORD PTR [esp+0x4],eax
   0x08048f4a <+138>:	mov    DWORD PTR [esp],0x80c5348
   0x08048f51 <+145>:	call   0x8054640 <execv>
   0x08048f56 <+150>:	jmp    0x8048f80 <main+192>
   0x08048f58 <+152>:	mov    eax,ds:0x80ee170
   0x08048f5d <+157>:	mov    edx,eax
   0x08048f5f <+159>:	mov    eax,0x80c5350
   0x08048f64 <+164>:	mov    DWORD PTR [esp+0xc],edx
   0x08048f68 <+168>:	mov    DWORD PTR [esp+0x8],0x5
   0x08048f70 <+176>:	mov    DWORD PTR [esp+0x4],0x1
   0x08048f78 <+184>:	mov    DWORD PTR [esp],eax
   0x08048f7b <+187>:	call   0x804a230 <fwrite>
   0x08048f80 <+192>:	mov    eax,0x0
   0x08048f85 <+197>:	leave
   0x08048f86 <+198>:	ret
End of assembler dump.
gdb-peda$
```

While looking through the assembly code, the following functions are called that are labeled with bracket notation (`<>`) on the right side of the assembly code:
- atoi
- strdup
- strdup
- getegid
- geteuid
- setresgid
- setresuid
- execve
- fwrite

Below is the first segment of code that creates the stack frame:
```
   0x08048ec0 <+0>:	push   ebp
   0x08048ec1 <+1>:	mov    ebp,esp
   0x08048ec3 <+3>:	and    esp,0xfffffff0
   0x08048ec6 <+6>:	sub    esp,0x20
```

On line `+9`, the address for `argv[1]` is accessed, and it stores the value into `eax`.
```
   0x08048ec9 <+9>:	mov    eax,DWORD PTR [ebp+0xc]
```

Then the function `atoi` is called, and it compares the return value of `atoi` with `0x1a7` (`423` in decimal). If the value is not the same (`jne`: jump if not equal), the code will jump to `<main+152>`.
```
   0x08048ed4 <+20>:	call   0x8049710 <atoi>
   0x08048ed9 <+25>:	cmp    eax,0x1a7
   0x08048ede <+30>:	jne    0x8048f58 <main+152>
```

Let's run the program with the argument 423 (the value from line `+25` that was `0x1a7` in the code):

```
level0@RainFall:~$ ./level0 423
$
```

It prints a dollar sign, so let's try typing something.

```
$ ls
ls: cannot open directory .: Permission denied
$ pwd
/home/user/level0
$
```

This looks like a shell, and let's try finding the password (as shown on the subject).

##### The subject
```
level0@RainFall:~$ ./level0 $(exploit)
$ cat /home/user/level1/.pass
?????????????????????
$ exit
level0@RainFall:~$ su level1
Password:
level1@RainFall:~$ _
```

##### The prompt we encountered
```
$ cat /home/user/level1/.pass
1fe8a524fa4bec01ca4ea2a869af2a02260d4a7d5fe7e7c24d8617e6dca12d3a
$ su level1
Password:
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level1/level1
level1@RainFall:~$
```

We did it!
