#ifndef HEADERS_H_
#define HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include<stdbool.h>
#include <signal.h>
#include "prompt.h"
#include<sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <termios.h>

#define MAX_ENTRIES 16
// Maine baadme code chala ke dekah to MAX_ENTRIES 15   pe vo 14v hi entry le rha ths to 16 kar diya 
#define ENTRY_SIZE 1000
#define BUFFER_SIZE 10000
#define MAX_LEN_COMMAND 1024
#define MAX_NO_COMMAND 100

#define GREEN "\033[1;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[1;34m"
#define WHITE "\033[0;37m"
#define RESET "\033[0m"
#define BOLD "\033[1m"



typedef struct tokArrStr
{
    char **ArrOfPartsOfOriginalStr;
    int i;
}tokArrStr;


typedef tokArrStr * PtrTokStruct;


typedef struct IO
{
    int x;
    int fd_in;
    int fd_out;
} IO;

typedef struct bgProcess
{
    pid_t PID;
    char Name[MAX_LEN_COMMAND];
    struct bgProcess *Next;
} bgProcess;


void InsertProcess(pid_t PID, char *Name);
void RemoveProcess(pid_t PID);
void checkBG();
void ExecuteOtherCommand(char *Command);
void Display_name_A();
// char **TokeniseInput(char *Input,char *WhatToRemove);
PtrTokStruct TokeniseInput(char *Input,char *WhatToRemove);
int DoesEndWithChar(char *Commands, char SpecificChar);
char **RemoveStartEndSpace(char **ArrOfCommands, int noOfCommands);
void PrintStrArray(char **StrArr, int NoOFStr);
void StoreCurrDirToPrev();
void ExecuteWrap(char *Command);
void ExecutePeak(char *Command);
void ExecuteSeek(char *Command);
void addEntry(char *entry);
void LoadHistory(char *FileName);
void SaveEntry(char *FileName);
void handle_sigint();
void AddCommandWithCondition(char *command);
void ExecutePastevents(char *Command);
void ExecuteProclore(char *Command);
void ExecuteCommand(char *Command);


char **tokenize_withQoutes(const char *str, const char *delimiters, int *token_count);
IO IoRedirection(char *Command);
void IORegain(IO Saved);
void stripHTMLTags(char *str);
void iMan(const char *command_name);
void ExecuteiMan(char *Command);
int get_recent_pid();
void die(const char *s);
void disableRawMode();
void enableRawMode();
void handleSignal2(int signo);
void ExecuteNeonate(char *Command);
void handle_sigint();
void handle_ctrlD();
void handle_sigtstp(int sig);
void InsertProcessActivites(pid_t PID, char *Name);
char get_process_state(pid_t pid);
void PrintAllProcess();



extern char QueueBuffer[MAX_ENTRIES][ENTRY_SIZE];
extern int QueueStart ;
extern int QueueEnd ;
extern char ShellActivatedDir[4024];
extern char Previous_directory[8000];
extern int WhileSeekNoTimesPrint ;
extern int FlagTimeGrtTwoSec ;
extern char PreviousCommand[2000];
extern int IsIOActivated ;
extern pid_t foreground_process_pid;
extern bgProcess *ActiVitesListHead;
extern bgProcess *Head;


#endif