#include "headers.h"
void ExecuteCommand(char *Command);
char QueueBuffer[MAX_ENTRIES][ENTRY_SIZE];
int QueueStart = 0;
int QueueEnd = 0;
char ShellActivatedDir[4024];
char Previous_directory[8000];
int WhileSeekNoTimesPrint = 0;
int FlagTimeGrtTwoSec = 0;
char PreviousCommand[2000];
int IsIOActivated = 0;
pid_t foreground_process_pid = -1;
bgProcess *ActiVitesListHead = NULL;
bgProcess *Head = NULL;




char *AndKiJagahSemiColon(char *PuraCommand)
{
    char *AnsArr = (char *)malloc(sizeof(char) * MAX_LEN_COMMAND);
    int i = 0;
    int ansArrIndex = 0;
    while (PuraCommand[i] != '\0')
    {
        if (PuraCommand[i] == '&')
        {
            AnsArr[ansArrIndex] = ' ';
            AnsArr[ansArrIndex + 1] = PuraCommand[i];
            AnsArr[ansArrIndex + 2] = '\n';
            AnsArr[ansArrIndex + 3] = ';';
            ansArrIndex = ansArrIndex + 4;
        }
        else
        {
            AnsArr[ansArrIndex] = PuraCommand[i];
            ansArrIndex++;
        }
        i++;
    }
    AnsArr[ansArrIndex] = '\0';
    return AnsArr;
}



// Return 1 if it end with that char and 0 if it not end with that char
int DoesEndWithChar(char *Commands, char SpecificChar)
{
    int ans = 0;
    int len = strlen(Commands);
    if (Commands[len - 1] == SpecificChar)
    {
        ans = 1;
    }
    return ans;
}



void PrintStrArray(char **StrArr, int NoOFStr)
{
    for (int i = 0; i < NoOFStr; i++)
    {
        printf("|%s|      ", StrArr[i]);
    }
    printf("\n");
}

void StoreCurrDirToPrev()
{
    char currentDir[3024];
    if (getcwd(currentDir, sizeof(currentDir)) == NULL)
    {
        perror("Cant get cwd");
    }
    strcpy(Previous_directory, currentDir);
}



// void ExecuteFgBg(char *Command)
// {
//     char *CommandCpy = (char *)malloc(sizeof(char) * 200);
//     strcpy(CommandCpy, Command);
//     PtrTokStruct tokenizeStruct = TokeniseInput(CommandCpy, " \t\n");
//     int NoOfPart = tokenizeStruct->i;
//     char **ArrOfPartOfCommand = tokenizeStruct->ArrOfPartsOfOriginalStr;
//     if (strcmp(ArrOfPartOfCommand[0], "fg") == 0)
//     {
//         int ProcessId = atoi(ArrOfPartOfCommand[1]);
//         kill(ProcessId, SIGCONT);
//         int Status;
//         waitpid(ProcessId, &Status, WUNTRACED);
//     }
//     else if (strcmp(ArrOfPartOfCommand[0], "bg") == 0)
//     {
//         int ProcessId = atoi(ArrOfPartOfCommand[1]);
//         kill(ProcessId, SIGCONT);
//     }
// }


int main()
{
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigtstp);

    // Keep accepting commands
    if (getcwd(ShellActivatedDir, sizeof(ShellActivatedDir)) == NULL)
    {
        perror("Cant get cwd");
    }
    //* At Start previous dir == Shell Activated dir yto avoid seg fault
    strcpy(Previous_directory, ShellActivatedDir);
    LoadHistory("history.txt");
    while (1)
    {
        // Print appropriate prompt with username, systemname and directory before accepting input
        Display_name_A(ShellActivatedDir);
        char input[4096];
        if (fgets(input, 4096, stdin) == NULL)
        {
            // handle_ctrlD();
            char PathToSave[1000];
            AddCommandWithCondition("exit\n");
            snprintf(PathToSave, sizeof(PathToSave), "%s/history.txt", ShellActivatedDir);
            SaveEntry(PathToSave);
            kill(0, SIGTERM);
            printf("\n");
            exit(0);
            // break;
        }
        if (strcmp(input, "exit\n") == 0)
        {
            // Exit the shell if the user types 'exit'
            break;
        }
        strcpy(PreviousCommand, input);
        checkBG();
        AddCommandWithCondition(input);
        char *Finalinput = AndKiJagahSemiColon(input);

        char **ArrOfCommands;
        ArrOfCommands = TokeniseInput(Finalinput, ";")->ArrOfPartsOfOriginalStr; //! & ke basis pe bhi
        int NoOfCommands = 0;
        while (ArrOfCommands[NoOfCommands] != NULL)
        {
            ExecuteCommand(ArrOfCommands[NoOfCommands]);
            NoOfCommands++;
        }
    }
}


