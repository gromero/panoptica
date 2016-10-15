#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void codeCache(unsigned int instr)
{
 //asm(".long 0xBABEBEEF");
 //asm("nop");
 //int data[] = "0xBABEBEEF"

 //unsigned char data[] = {0xB8, 0x00, 0x00, 0x00, 0x00, 0xC3};
 unsigned char data[] = {0x90, 0x90, 0x90, 0x90, 0xC3};
 data[0] = instr;
 int (*f)(void);
 void *mem;

 mem = mmap(NULL, 5 /* 6 bytes */, PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);
 memcpy(mem,data, 5 );

#if DEBUG
 printf("0x%x\n", data);
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
 if ( !(instruction <= 0xFFFFFF))
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
          printf("Instruction: 0x%x is valid. Probably a push or a jump/branch "
                 "instruction.\n", instruction);

        } else if (WTERMSIG(cstatus) == SIGILL) { // Illegal instruction
          printf("Instruction: 0x%x is invalid.\n", instruction);

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

  // Execute the parent's JITed code.
  codeCache(instruction);
 }
}
