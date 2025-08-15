#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAXLINE 512 

int
main(int argc, char *argv[])
{
    char *command;
    char line[MAXLINE];
    int lineidx = 0;
    char *args[MAXARG];
    int c;
    if (argc < 2) {
        fprintf(2, "usage: xargs <command> [arg ...]\n");
        exit(1);
    }
    command = argv[1];
    int max_fixed = MAXARG - 2;
	int fixed = argc - 1;
	if (fixed > max_fixed) {
    	fprintf(2, "xargs: too many initial args\n");
    	exit(1);
	}
	for (int i = 1; i <= fixed; i++)
    	args[i - 1] = argv[i];         
    while (read(0,&c,1) == 1) {
        if (c == '\n') {
            line[lineidx] = '\0';
            if (lineidx == 0)      
                continue;
            int idx = fixed;
            args[idx++] = line;    
            if (idx >= MAXARG) {
                fprintf(2, "xargs: too many args\n");
                exit(1);
            }
            args[idx] = 0;
            int pid = fork();
            if (pid == 0) {
            	char *buf = malloc(lineidx + 1);
         		if (!buf) {
             		fprintf(2, "xargs: malloc failed\n");
             		exit(1);
 		        }
            	memmove(buf, line, lineidx + 1);
                args[idx - 1] = buf;
                exec(command, args);
                fprintf(2, "xargs: exec failed\n");
                exit(1);
            } 
            else {
                wait(0);
            }
            lineidx = 0;
        } 
        else {
            if (lineidx >= MAXLINE - 1) {
                fprintf(2, "xargs: line too long\n");
                exit(1);
            }
            line[lineidx++] = c;
        }
    }
    exit(0);
}
