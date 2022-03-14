#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>

#define MEMORY_LIMIT 1073741824
#define SOFT_TIME_LIMIT 3
#define HARD_TIME_LIMIT 4

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./engine filename(w/o extension)\n");
        return -1;
    }

    char c_file[50];
    char out_file[50];
    char exec_file[50] = "./";

    strcpy(c_file, argv[1]);
    strcpy(out_file, argv[1]);

    strcat(c_file, ".c");
    strcat(out_file, ".out");
    strcat(exec_file, out_file);

    printf("C file : %s\nOut file : %s\nExec file : %s\n", c_file, out_file, exec_file);

    int status;
    pid_t run = fork();
    pid_t compile = fork();
    if (compile < 0 || run < 0)
    {
        printf("fork error");
    }
    else if (compile == 0)
    {
        execl("/usr/bin/gcc", "gcc", c_file, "-o", out_file, NULL);
        return 0;
    }
    else if (run == 0)
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

        //Sleep till the compilation is complete
        if (waitpid(compile, &status, 0) == -1)
        {
            printf("Waitpid error on compilation\n");
            return 2;
        }

        //Run the file (currently doesnt support stdin and stdout printed to terminal)
        if (execl(exec_file, ">", "output", NULL))
        {
            printf("Execl error: cannot run the executable");
            return 1;
        }
    }
    else
    {
        int status;
        if (waitpid(compile, &status, 0) == -1)
        {
            printf("Waitpid error on compilation\n");
            return 2;
        }
        printf("Compilation success, proceeding to run\n");
        if (waitpid(run, &status, 0) == -1)
        {
            printf("Waitpid error on execution\n");
            return 3;
        }
        printf("Execution finished, output stored at outputfile\n");
        printf("parent process finished\n");
    }
}