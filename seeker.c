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

 if (pid) {
//printf("I'm the parent\n");
//printf("PPID %d\n", getpid());
  waitpid(pid, &cstatus, 0);

  // Skip halting instruction below:
  if (instruction == 0x4f ||
      instruction == 0x4e   ) {
   instruction++;
   goto spawn;
  }

  if (WIFEXITED(cstatus)) {
    printf("Instruction: 0x%x is valid.\n", instruction);
  //printf("Child exited fine\n");
  } else {
    printf("Instruction: 0x%x is invalid.\n", instruction);
  //printf("Something went wrong to the child %d. Exited not normally.\n", pid);
  }

  instruction++;
  goto spawn;

 } else {
  codeCache(instruction);
//printf("I'm the child\n");
//printf("CPID: %d\n", getpid());
 }
}
