#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p2c[2], c2p[2];   // parent to child and child to parent
  char buf[1];          // 1 byte message
  int pid;

  // create two pipes
  if (pipe(p2c) < 0 || pipe(c2p) < 0) {
    fprintf(2, "pipe() failed\n");
    exit(1);
  }
  // fork
  pid = fork();
  if (pid < 0) {
    fprintf(2, "fork() failed\n");
    exit(1);
  }
  if (pid == 0) {               // now child is executing!
    close(p2c[1]);              // close write end
    close(c2p[0]);              // close read end
    // child wait for 1 byte message from parent
    if (read(p2c[0], buf, 1) != 1) {
      fprintf(2, "child: read failed\n");
      exit(1);
    }
    printf("%d: received ping\n", getpid());
    // child to parent
    if (write(c2p[1], buf, 1) != 1) {
      fprintf(2, "child: write failed\n");
      exit(1);
    }

    close(p2c[0]);
    close(c2p[1]);
    exit(0);

  } 
  else {                      // now parent is executing!
    close(p2c[0]);              // close read end
    close(c2p[1]);              // close write end

    // parent sends 1 byte to child 
    buf[0] = 'x';
    if (write(p2c[1], buf, 1) != 1) {
      fprintf(2, "parent: write failed\n");
      exit(1);
    }
    // parent wait for 1 byte message from child
    if (read(c2p[0], buf, 1) != 1) {
      fprintf(2, "parent: read failed\n");
      exit(1);
    }
    printf("%d: received pong\n", getpid());

    // parent wait for child exit 
    wait(0);

    close(p2c[1]);
    close(c2p[0]);
    exit(0);
  }
}
