#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "func.h"

int main(int argc, char *argw[]) {
    char *prog_name;
    if (argc >= 2)
        prog_name = argw[1];
    else
        prog_name = "./child.out";

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        print(STDERR_FILENO, "ERROR: failed to create pipe\n");
        exit(-1);
    }

    int buf_size = 64;
    char *fname = malloc(buf_size);
    *fname = 0;
    if (read_line(STDIN_FILENO, &fname, &buf_size) <= 0) {
        print(STDERR_FILENO,
              "ERROR: failed read filename from standart input\n");
        exit(-1);
    }
    int fd = open(fname, O_RDONLY);
    if (fd == -1) {
        print(STDERR_FILENO, "ERROR: failed to open file: \"");
        print(STDERR_FILENO, fname);
        print(STDERR_FILENO, "\"\n");
        print(STDERR_FILENO, strerror(errno));
        print(STDERR_FILENO, "\n");
        exit(-1);
    }
    free(fname);

    pid_t pid = fork();
    if (pid == 0) {
        dup2(fd, STDIN_FILENO);
        close(fd); // Закрываем исходный дескриптор, он уже продублирован

        close(pipefd[0]); // Закрываем чтение, процесс будет только писать в pipe
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);

        // Запускаем дочерний процесс
        child_process();

        exit(0);
    } else if (pid == -1) {
        print(STDERR_FILENO, "ERROR: failed to fork process\n");
        print(STDERR_FILENO, strerror(errno));
        print(STDERR_FILENO, "\n");
        exit(-1);
    } else {
        char buffer[128];
        close(pipefd[1]);
        while (1) {
            int n = read(pipefd[0], buffer, sizeof(buffer));
            if (n == 0)
                break;
            write(STDOUT_FILENO, buffer, n);
        }
        waitpid(pid, NULL, 0);
    }
    return 0;
}
