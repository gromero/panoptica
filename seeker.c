#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
 int pid;

 pid = fork();

 if (pid) {
  printf("I'm the parent\n");
  printf("PPID %d\n", getpid());
 } else {
  printf("I'm the child\n");
  printf("CPID: %d\n", getpid());
 }
}
