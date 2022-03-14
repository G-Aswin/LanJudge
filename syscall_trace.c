#define _POSIX_SOURCE

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/ptrace.h>
#include <linux/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>

#include <stdio.h>

#define MEMORY_LIMIT 1073741824
#define SOFT_TIME_LIMIT 3
#define HARD_TIME_LIMIT 4

/*
  Error Codes, will be used when writing server
  1: System Error
  2: Unprevilidged Code Error
  3: TLE
  4: MLE
*/

int main(int argc, char *argv[])
{
  freopen("error.log", "w", stderr);

  pid_t pid = fork();
  if (pid == -1)
  {
    fprintf(stderr, "[System Error]: Coudln't fork!\n");
    return 1;
  }
  else if (pid == 0)
  {
    /* Set time limit */
    struct rlimit rsc;
    rsc.rlim_cur = SOFT_TIME_LIMIT;
    rsc.rlim_max = HARD_TIME_LIMIT;
    if (setrlimit(RLIMIT_CPU, &rsc) == -1)
    {
      fprintf(stderr, "[System Error]: Couldn't set Time Limit!");
      return 1;
    }

    /* Set memory limit */
    rsc.rlim_cur = MEMORY_LIMIT;
    rsc.rlim_max = MEMORY_LIMIT;
    if (setrlimit(RLIMIT_AS, &rsc) == -1)
    {
      fprintf(stderr, "[System Error]: Couldn't set Memory Limit!");
      return 1;
    }

    /* Start traicing */
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);

    /* Run */
    if (execl(argv[1], "", NULL))
    {
      fprintf(stderr, "[System Error]: Couldn't run\n");
      return 1;
    }
  }
  else
  {
    int status;
    ptrace(PTRACE_SYSCALL, pid, NULL, NULL);

    while (1)
    {
      waitpid(pid, &status, 0);
      if (WIFEXITED(status))
      {
        int exit_val = WEXITSTATUS(status);
        if (exit_val)
        {
          fprintf(stderr, "[Runtime Error]: %d\n", exit_val);
          return 5;
        }
        return 0;
      }
      else if (WIFSIGNALED(status))
      {
        int sign_val = WTERMSIG(status);
        if (sign_val == SIGSEGV)
        {
          fprintf(stderr, "[MLE]: \n");
          return 3;
        }
        else if (sign_val == SIGXCPU)
        {
          fprintf(stderr, "[TLE]: \n");
          return 4;
        }
      }
      else if (WIFSTOPPED(status))
      {
        int sign_val = WSTOPSIG(status);
        if (sign_val == SIGTRAP)
        {
          struct user_regs_struct regs;
          ptrace(PTRACE_GETREGS, pid, 0, &regs);
          long syscall = regs.orig_rax;

          printf("System Call %ld\n", syscall);
          if (syscall == 56)
          {
            fprintf(stderr, "[Unprevileged Code]: \n");
            kill(pid, SIGTERM);
            return 2;
          }
        }

        ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
      }
    }
  }

  return 0;
}
