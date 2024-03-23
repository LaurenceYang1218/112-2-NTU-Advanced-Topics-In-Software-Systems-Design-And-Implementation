#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shell.h"

const int maxArgs = _POSIX_ARG_MAX;

unsigned int parseArgs(char* cmd, char** args, const char* delim) {
    char *token;
    unsigned int argcnt = 0; 
    token = strtok(cmd, delim);
    args[argcnt] = token;
    argcnt++;
    while(token != NULL && argcnt < maxArgs) {
        token = strtok(NULL, delim);
        args[argcnt] = token;
        if (token != NULL) {
            argcnt++;
        }
    }
    return argcnt;
}

void printHistory(char** cmdbuf, unsigned int* cmdcnt) {
    int numCmd = *cmdcnt;
    if (numCmd < MAX_HISTORY_LEN) {
        for (int i = 1; i <= numCmd; i++) {
            printf("%5d  %s\n", i, cmdbuf[i-1]);
        }
    }else {
        for (int i = numCmd-MAX_HISTORY_LEN+1; i <= numCmd; i++) {
            printf("%5d  %s\n", i, cmdbuf[i-1]);
        }
    }
    return;
}

void printLimitedHistory(char** cmdbuf, unsigned int* cmdcnt, long limitedNum) {
    int numCmd = *cmdcnt;
    if (numCmd < limitedNum) {
        if (numCmd < MAX_HISTORY_LEN) {
            for (int i = 1; i <= numCmd; i++) {
                printf("%5d  %s\n", i, cmdbuf[i-1]);
            }
        }else {
            for (int i = numCmd-MAX_HISTORY_LEN+1; i <= numCmd; i++) {
                printf("%5d  %s\n", i, cmdbuf[i-1]);
            }
        }
    }else {
        if (limitedNum < MAX_HISTORY_LEN) {
            for (int i = numCmd-limitedNum+1; i <= numCmd; i++) {
                printf("%5d  %s\n", i, cmdbuf[i-1]);
            }
        }else {
            for (int i = numCmd-MAX_HISTORY_LEN+1; i <= numCmd; i++) {
                printf("%5d  %s\n", i, cmdbuf[i-1]);
            }
        }
    }
    return;
}

void clearHistory(char** cmdbuf, unsigned int* cmdcnt) {
    int numCmd = *cmdcnt;
    for (int i = 0; i < numCmd; i++) {
        memset(cmdbuf[i], '\0', strlen(cmdbuf[i]));
    }
    *cmdcnt = 0;
    return;
}

void execShell(char* cmd, char** cmdbuf, unsigned int* cmdcnt) {
    const char* delim = " ";
    char** args = (char **)malloc(maxArgs * sizeof(char *));

    unsigned int argnum = 0;
    if (strncmp(cmd, "cd", 2) == 0) {
        argnum = parseArgs(cmd, args, delim);
        if (argnum > 2) {
            fprintf(stderr, "error: too many arguments\n");
        }else if (argnum == 2) {
            int ret = chdir(args[1]);
            if (ret != 0) {
                fprintf(stderr, "error: %s\n", strerror(errno));
            }
        }else {
            fprintf(stderr, "error: too few arguments\n");
        }
    }else if (strncmp(cmd, "history", 7) == 0) {
        argnum = parseArgs(cmd, args, delim);
        if (argnum == 1) {
            printHistory(cmdbuf, cmdcnt);
        }else if (argnum == 2) {
            if (strcmp(args[1], "-c") == 0) {
                clearHistory(cmdbuf, cmdcnt);
            }else {
                long num = strtol(args[1], NULL, 10);
                if (num <= 0 || errno == EINVAL || errno == ERANGE) {
                    fprintf(stderr, "error: invalid arguments\n");
                }else {
                    printLimitedHistory(cmdbuf, cmdcnt, num);
                }                
            }   
        }else {
            fprintf(stderr, "error: too many arguments\n");
        }
    }else if (strncmp(cmd, "exit", 4) == 0) {
        argnum = parseArgs(cmd, args, delim);
        for (int i = 0; i < MAX_CMDBUF_LEN; i++) {
            free(cmdbuf[i]);
        }
        free(cmd);
        free(args);
        if (argnum > 1) {
            fprintf(stderr, "error: too many arguments\n");
        }
        exit(EXIT_SUCCESS);
    }else {
        argnum = parseArgs(cmd, args, delim);
        pid_t pid = fork();
        if (pid == -1) {
            fprintf(stderr, "error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }else if (pid == 0) {
            int ret = execvp(args[0], &args[0]);
            if (ret == -1) {
                fprintf(stderr, "error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }else {
            wait(NULL);
        }
    }
    free(args);
    return;
}