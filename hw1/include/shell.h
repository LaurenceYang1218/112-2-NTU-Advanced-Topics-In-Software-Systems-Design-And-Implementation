#ifndef _SHELL_H_
#define _SHELL_H_

#define _POSIX_ARG_MAX 4096
#define MAX_CMD_LEN 4096
#define MAX_CMDBUF_LEN 10000
#define MAX_HISTORY_LEN 10

unsigned int parseArgs(char* cmd, char** args, const char* delim);
void execShell(char* cmd, char** cmdbuf, unsigned int *cmdcnt);
void printHistory(char** cmdbuf, unsigned int *cmdcnt);
void printLimitedHistory(char** cmdbuf, unsigned int *cmdcnt, long limitedNum);
void clearHistory(char** cmdbuf, unsigned int *cmdcnt);

#endif