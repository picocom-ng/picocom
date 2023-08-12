#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pty.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int porta, portb;
    char ptyname[PATH_MAX];
    pid_t pid;

    if (openpty(&porta, &portb, ptyname, NULL, NULL) != 0) {
        fprintf(stderr, "ERROR: failed to open pty (%d): %s\n", errno, strerror(errno));
        exit(1);
    }

    switch(pid = fork()) {
        case -1:
            fprintf(stderr, "ERROR: failed to start background process (%d): %s", errno, strerror(errno));
            exit(1);
            break;
        case 0:
            pause();
            exit(0);
            break;

        default:
            close(porta);
            close(portb);
            break;
    }

    printf("%s %d\n", ptyname, pid);
    return 0;
}
