#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static void
sieve(int left_pipe)
{
    int p;
    // The first number read from left_pipe is a prime!!!
    if (read(left_pipe, &p, sizeof(p)) != sizeof(p)) {
    	// The left_pipe has nothing to read. Close normally.
        close(left_pipe);
        exit(0);
    }
    printf("prime %d\n", p);
    int right_pipe[2];
    if (pipe(right_pipe) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }
    if (pid == 0) {                // child
        close(left_pipe);           
        close(right_pipe[1]);       
        sieve(right_pipe[0]);       
        exit(0);
    } 
    else {                         // parent
        close(right_pipe[0]);      
        int n;
        while (read(left_pipe, &n, sizeof(n)) == sizeof(n)) {
            if (n % p != 0) {
                write(right_pipe[1], &n, sizeof(n));
            }
        }
        close(left_pipe);
        close(right_pipe[1]);
        wait(0);                   
        exit(0);
    }
}

int
main(int argc, char *argv[])
{
    int left_pipe[2];
    if (pipe(left_pipe) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }
    if (pid == 0) {                 // child
        close(left_pipe[1]);        
        sieve(left_pipe[0]);
        exit(0);
    } else {                        // parent
        close(left_pipe[0]);        
        for (int i = 2; i <= 35; i++) {
            write(left_pipe[1], &i, sizeof(i));
        }
        close(left_pipe[1]);        
        wait(0);                    
        exit(0);
    }
}
