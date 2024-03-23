#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shell.h"

char* cmdbuf[MAX_CMDBUF_LEN];
char* pipeCmds[MAX_CMDBUF_LEN];

void signalHandler(int sig) {
    for (int i = 0; i < MAX_CMDBUF_LEN; i++) {
        free(cmdbuf[i]);
    }
    exit(EXIT_SUCCESS);
    return;
}

unsigned int parsePipeCmd(char* cmd, char** pipeCmds) {
    unsigned int cmdCnt = 0;
    char *temp = cmd, *needle = " | ", *ptr;
    while ((ptr = strstr(temp, needle)) != NULL) {
        strncpy(pipeCmds[cmdCnt++], temp, ptr-temp);
        temp = ptr + strlen(needle);     
    }
    strcpy(pipeCmds[cmdCnt++], temp);
    return cmdCnt;
}

int main(int argc, char *argv[]) {

    size_t maxCmdLen = MAX_CMD_LEN;
    signal(SIGINT, signalHandler);

    for (int i = 0; i < MAX_CMDBUF_LEN; i++) {
        cmdbuf[i] = (char *)malloc(MAX_CMD_LEN * sizeof(char));
    }

    unsigned int cmdcnt = 0;
    while(true) {
        putchar('$');
        int cmdlen = getline(&cmdbuf[cmdcnt], &maxCmdLen, stdin);

        if (cmdlen == 1 && cmdbuf[cmdcnt][cmdlen-1] == '\n') {
            continue;
        }

        if (cmdbuf[cmdcnt][cmdlen-1] == '\n') {
            cmdbuf[cmdcnt][cmdlen-1] = '\0';
        }

        char* currCmd = (char *)malloc(maxCmdLen * sizeof(char));
        strcpy(currCmd, cmdbuf[cmdcnt]);       
        if (cmdcnt > 0 && strcmp(cmdbuf[cmdcnt], cmdbuf[cmdcnt-1]) == 0) {
            memset(cmdbuf[cmdcnt], '\0', strlen(cmdbuf[cmdcnt]));
            cmdcnt--;   
        } 
        cmdcnt++;
        
        if (strchr(currCmd, '|') == NULL) {
            execShell(currCmd, cmdbuf, &cmdcnt);             
        }else {
            for (int i = 0; i < MAX_CMDBUF_LEN; i++) {
                pipeCmds[i] = (char *)malloc(MAX_CMD_LEN * sizeof(char));
            }
            unsigned int numPipeCmds = parsePipeCmd(currCmd, pipeCmds);
            const unsigned int numPipes = numPipeCmds-1;

            int pipeFd[numPipes][2];
            for (int i = 0; i < numPipes; i++) {
                int ret = pipe(pipeFd[i]);
                if (ret < 0) {
                    fprintf(stderr, "error: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
            }

            for (int i = 0; i < numPipeCmds; i++) {
                pid_t pid = fork();
                if (pid < 0) {
                    fprintf(stderr, "error: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }else if (pid == 0) {
                    if (i > 0) {
                        // connect input to pipe 
                        int ret = dup2(pipeFd[i-1][0], STDIN_FILENO);
                        if (ret < 0) {
                            fprintf(stderr, "error: %s\n", strerror(errno));
                            exit(EXIT_FAILURE);
                        }
                    }
                    if (i < numPipeCmds-1) {
                        // connect output to pipe
                        int ret = dup2(pipeFd[i][1], STDOUT_FILENO);
                        if (ret < 0) {
                            fprintf(stderr, "error: %s\n", strerror(errno));
                            exit(EXIT_FAILURE);
                        }
                    }
                    for (int j = 0; j < numPipes; j++) {
                        close(pipeFd[j][0]);
                        close(pipeFd[j][1]);
                    }
                    execShell(pipeCmds[i], cmdbuf, &cmdcnt);
                    exit(EXIT_SUCCESS);
                }else {
                    if (i == numPipeCmds-1) {
                        for (int j = 0; j < numPipes; j++) {
                            close(pipeFd[j][0]);
                            close(pipeFd[j][1]);
                        }
                        for (int j = 0; j < numPipeCmds; j++) {
                            wait(NULL);
                        }
                    }
                }
            }
            for (int i = 0; i < MAX_CMDBUF_LEN; i++) {
                free(pipeCmds[i]);
            }
        }
        free(currCmd);
    }

    for (int i = 0; i < MAX_CMDBUF_LEN; i++) {
        free(cmdbuf[i]);
    }

    return 0;
}