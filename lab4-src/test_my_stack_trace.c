#include <arch/x86/include/asm/unistd_64.h>
#include <stdio.h>
#include <errno.h>

int main() { 
        printf("Calling ...\n"); 
	syscall(__NR_my_stack_trace);
	return 0; 
}
