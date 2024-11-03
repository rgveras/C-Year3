#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>




int main(int argc, char * argv[]) {


    int num1 = atoi(argv[1]);
    int a1 = num1 / 100;
    int a2 = num1 % 100;
    int num2 = atoi(argv[2]);
    int b1 = num2 / 100;
    int b2 = num2 % 100;

    int c;

    int port[2];
    pid_t pid;

    if (pipe(port) < 0) {
        perror("pipe error"); exit(0);
    }
    pid = fork();
    if (pid <0) {
        perror("fork error"); exit(0);
    }

    if (pid > 0) {
        //close(port[0]);
        //printf("HELLO");
        write(port[1], &a1, sizeof(a1));
        write(port[1], &a2, sizeof(a2));
        write(port[1], &b1, sizeof(b1));
        write(port[1], &b2, sizeof(b2));
        //close(port[1]);
        wait(NULL);
    } else {
        //close(port[1]);
        read(port[0], &a1, sizeof(a1));
        read(port[0], &a2, sizeof(a2));
        read(port[0], &b1, sizeof(b1));
        read(port[0], &b2, sizeof(b2));
        //close(port[0]);
        printf("\n from child: I read: %d, %d, %d, and %d\n", a1, a2, b1, b2);
        exit(0);
    }

    return 0;
}
