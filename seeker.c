#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void jiter(void *p, size_t n, int dbgl)
{
 int (*f)(void);
 void *mem;
 unsigned char terminator = 0xC3; // retq, just on x64 for sure ;-)

 mem = mmap(NULL, n + 1, PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);
 memcpy(mem, p, n);
 memcpy(mem+n, &terminator, 1);

#if DEBUG
 unsigned char *bytecode;
 bytecode = (unsigned char *) mem;

 printf("CODECACHE: \n");
 for (int i = 0; i < n + 1; ++i)
   printf("0x%.2X\n",  *(bytecode+i));
 printf("\n");
#endif

 f = mem ;
 f();
}

int main(void)
{
 int pid;
 int cstatus;

 unsigned int instruction = 0x00000000;

spawn:
 if ( !(instruction <= 0xFFFFFF) )
  exit(0);

 pid = fork();

 if (pid) { // Parent
  waitpid(pid, &cstatus, 0);

/*** Kept as comment as an example on how to skip an
 *** instruction.
  // Skip halting instruction below:
  if (instruction == 0x4f ||
      instruction == 0x4e   ) {
   instruction++;
   goto spawn;
  }
***/

  if (WIFEXITED(cstatus)) {
    printf("Instruction: 0x%x is valid.\n", instruction);

  } else { // Abnormal exit, cause must be investigated further...

      if (WIFSIGNALED(cstatus)) {
        if (WTERMSIG(cstatus) == SIGALRM) {       // Timeouted
          printf("Instruction: 0x%x is valid. It halts, so probably a self push "
                 "or jump/branch instruction.\n", instruction);

        } else if (WTERMSIG(cstatus) == SIGILL) { // Illegal instruction
          printf("Instruction: 0x%x is invalid.\n", instruction);

        } else if (WTERMSIG(cstatus) == SIGSEGV) { // Load/Store instruction
          printf("Instruction: 0x%x is probably valid. Load/store, "
                 "fusion with the ending return byte, "
                 "or a self modified jit.\n",
                  instruction);

        } else {                                  // Cause not understood yet
          printf("Instruction: 0x%x is invalid. (unknown cause)\n", instruction);

        }
     }
   }

  instruction++;
  goto spawn;

 } else { // Child

  // Child must execute the jited code in a certain amount of time. This is most
  // necessary to avoid special kinds of lock like those as self pushed address
  // + return case on x64 (PPC64 has no dynamic stack thus push/pop instruction).
  alarm(1);

  // Execute generated instruction code.
  jiter((void *)&instruction,4,0);
 }
}
