#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <errno.h>

#include <stdio.h>

int main(int argc, char *argv[])
{
    int status;
    pid_t run = fork();
    pid_t compile = fork();
    if (compile < 0 || run < 0)
    {
        printf("fork error");
    }
    else if (compile == 0)
    {
        execl("/usr/bin/gcc","gcc", "samplecode.c", "-o", "sample.out", NULL);
        return 0;
    }
    else if (run == 0)
    {
        waitpid(compile, &status, 0);
        execl("./sample.out", "<", "input", ">", "output", NULL);
    }
    else
    {
        int status;
        waitpid(compile, &status, 0);
        printf("Compilation success, proceeding to run\n");
        waitpid(run, &status, 0);
        printf("Execution finished, output stored at outputfile\n");
        printf("parent process finished\n");
    }
}