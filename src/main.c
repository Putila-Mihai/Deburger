#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program> [args...]\n", argv[0]);
        return EXIT_FAILURE;
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        if (ptrace(PT_TRACE_ME, 0, NULL, NULL) == -1) {
            perror("ptrace(PT_TRACE_ME");
            EXIT_FAILURE;
        }
        execv(argv[1], &argv[1]);
        perror("execv");
        exit(EXIT_FAILURE);
    }

    int status;
    for (;;) {
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return EXIT_FAILURE;
        }
        if (WIFEXITED(status)) {
            printf("Child exited with code %d\n", WEXITSTATUS(status));
            break;
        }
        if (WIFSIGNALED(status)) {
            printf("child killed by signal %d\n", WTERMSIG(status));
            break;
        }
        if (WIFSTOPPED(status)) {
            printf("child stopped by signal %d\n", WSTOPSIG(status));
            if (ptrace(PTRACE_CONT, pid, NULL, NULL) == -1) {
                perror("ptrace(PTRACE_CONT)");
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_SUCCESS;
}
