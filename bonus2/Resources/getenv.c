#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	printf("env address: %p\n", getenv("SHELLCODE"));
	return (0);
}
