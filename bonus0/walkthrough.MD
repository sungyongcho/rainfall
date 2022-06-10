# Bonus

- Once logged in, you will see a binary `bonus0`

```
[???]$ ssh localhost -p 4242 -l bonus0
	  _____       _       ______    _ _
	 |  __ \     (_)     |  ____|  | | |
	 | |__) |__ _ _ _ __ | |__ __ _| | |
	 |  _  /  _` | | '_ \|  __/ _` | | |
	 | | \ \ (_| | | | | | | | (_| | | |
	 |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                 Good luck & Have fun

  To start, ssh with level0/level0 on localhost:4242
bonus0@localhost's password:
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/bonus0/bonus0
bonus0@RainFall:~$ ls
bonus0
bonus0@RainFall:~$
```

let's execute and see what this does

```
bonus0@RainFall:~$ ./bonus0
 -
a
 -
b
a b
bonus0@RainFall:~$ ./bonus0
 -
aa
 -
bbbbbbbbbbbbb
aa bbbbbbbbbbbbb
bonus0@RainFall:~$ ./bonus0
 -
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
 -
bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
aaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbb�� bbbbbbbbbbbbbbbbbbbb��
Segmentation fault (core dumped)
```

this seems the program takes input, and prints out each input. but from the prompt of the last one, the output gives segmentation falult with lengthy input.

Let's take a deeper look into the program by using gdb.

From taking at the list of functions, there is a function p, pp, and main.
```
gdb-peda$ info functions
All defined functions:

Non-debugging symbols:
[...]
0x080484b4  p
0x0804851e  pp
0x080485a4  main
[...]
```

by analyzing the source code, we can see the the program ins using `strcpy(), strcat()`, which both are vulnerable for buffer overflow attack.

the program first calls function `pp`, and from function `pp` it calles `p` function.

From function `p`, it frist prints prints `" - "` on screen, and gets input with buffer size of 0x1000 from stdin. Once it copies into the buffer, if the buffer contains `newline; nl; '\n'`, it will set value where `\n` was placed into 0(=NULL; \0) and copies back 20 bytes back into first parameter of function.

by checking the output, the two buffers are stored next to each other. As the strncpy() is call to copy the first 20 bytes, but on the man page,

```
 If stpncpy() does not terminate dst with a
     NUL character, it instead returns a pointer to dst[n] (which
     does not necessarily refer to a valid memory location.)
```

And as the buffer is next to each other, `strcat()` function will consider the second string as the part of the first string.

We inject 4095 random caracters followed by a \n (because there is an unchecked strchr using it later). Then we will inject an overflow pattern to find where the RET address is replaced.


-------------xxxxxxxxxxxxxxxxxxx----------------------
