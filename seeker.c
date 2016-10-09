#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
 int pid;
 int cstatus;

 pid = fork();

 if (pid) {
  printf("I'm the parent\n");
  printf("PPID %d\n", getpid());
  waitpid(pid, &cstatus, 0);
  if (WIFEXITED(cstatus)) {
    printf("Child exited fine\n");
  } else {
    printf("Something went wrong to the child. Exited not normally.\n");
  }
 } else {
  asm(".long  0xBABEBEEF");
  printf("I'm the child\n");
  printf("CPID: %d\n", getpid());
 }
}
