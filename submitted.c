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
#define MAX_LEN_COMMAND 1024
#define MAX_NO_COMMAND 100
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

bgProcess *Head = NULL;
void InsertProcess(pid_t PID, char *Name)
{
    bgProcess *Newprocess = (bgProcess *)malloc(sizeof(bgProcess));
    Newprocess->PID = PID;
    strcpy(Newprocess->Name, Name);
    Newprocess->Next = Head;
    Head = Newprocess;
}

void RemoveProcess(pid_t PID)
{
    bgProcess *Current = Head;
    bgProcess *Prev = NULL;

    while (Current != NULL)
    {
        if (Current->PID != PID)
        {
            Prev = Current;
            Current = Current->Next;
        }
        else
        {
            break;
        }
    }
    if (Current == NULL)
    {
        printf("NOt is list Process\n");
        return;
    }
    if (Prev == NULL)
    {
        Head = Head->Next;
    }
    else
    {
        Prev->Next = Current->Next;
    }
    free(Current);
}

// Call it just After  display name
void checkBG()
{
    if (Head == NULL)
    {
        return;
    }
    bgProcess *Curr = Head;
    int status;
    while (Curr != NULL)
    {
        int result = waitpid(Curr->PID, &status, WNOHANG);
        if (result < 0)
        {
            perror("In BG\n");
            Curr = Curr->Next;
        }
        else if (result == 0)
        {
            Curr = Curr->Next;
        }
        else if (Curr->PID == result)
        {
            int Normal = WIFEXITED(status);
            if (Normal != 0)
            {
                printf("%s Exited Normally (%d)\n", Curr->Name, Curr->PID);
            }
            else
            {
                printf("%s Exited Abormally (%d)\n", Curr->Name, Curr->PID);
            }
            RemoveProcess(Curr->PID);
            Curr = Head;
        }
    }
}

// ? activities Start
bgProcess *ActiVitesListHead = NULL;
void InsertProcessActivites(pid_t PID, char *Name)
{
    bgProcess *Newprocess = (bgProcess *)malloc(sizeof(bgProcess));
    Newprocess->PID = PID;
    strcpy(Newprocess->Name, Name);
    Newprocess->Next = ActiVitesListHead;
    ActiVitesListHead = Newprocess;
}

char get_process_state(pid_t pid)
{
    char path[40], state;
    FILE *f;
    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    f = fopen(path, "r");
    if (f == NULL)
    {
        // printf("X");
        return 'X';
    }
    // while (fscanf(f, "State:\t%c", &state) != 1)
    // {
    //     fscanf(f, "%*[^\n]\n");
    // }
    char line[500];
    while (fgets(line, sizeof(line), f))
    {
        // printf("======>%s",line);
        if (strstr(line, "State:") != NULL)
        {
            sscanf(line, "State: %c", &state);
        }
    }
    // printf("%c",state);
    fclose(f);
    return state;
}

void PrintAllProcess()
{
    if (ActiVitesListHead == NULL)
    {
        return;
    }
    bgProcess *Curr = ActiVitesListHead;
    // bgProcess *Store = curr;
    int status;
    while (Curr != NULL)
    {
        char state_char = get_process_state(Curr->PID);
        // if (state_char == 'X')
        // {
        //     Curr = Curr->Next;
        //     continue;
        // }
        // const char *state_str;
        //  = (state_char == 'R' ? "Running" : (state_char == 'T' ? "Stopped" : "Unknown"));
        char *state_str = (char *)malloc(sizeof(char) * 100);
        // printf("%c",state_str);
        if (state_char == 'T')
        {
            strcpy(state_str, "Stopped");
        }
        else if (state_char == 'S')
        {
            strcpy(state_str, "Running");
        }
        else
        {
            Curr = Curr->Next;
            continue;
        }
        printf("%d : %s - %s\n", Curr->PID, Curr->Name, state_str);
        Curr = Curr->Next;
    }
}
// !else if (strcmp(ArrOfPartOfCommand[0], "activities") == 0)
// !{
// !    PrintAllProcess();
// !}
//! InsertProcessActivites(PID, ArrOfPartOfCommand[0]);

// ? activities End

void Display_name_A()
{
    char *Username = getlogin();
    char SystemName[1024];
    if (gethostname(SystemName, sizeof(SystemName)) != 0)
    {
        perror("Cant Get Host/System Name");
    }
    char currentDir[1024];
    if (getcwd(currentDir, sizeof(currentDir)) == NULL)
    {
        perror("Cant get cwd");
    }
    int DoesInHome = strcmp(currentDir, ShellActivatedDir); // in home if >0  else ==0 then home    OR     < 0 then outside home
    char relativeDir[1024];

    if (DoesInHome == 0)
    {
        strcpy(relativeDir, "~");
    }
    else if (DoesInHome > 0)
    {
        strcpy(relativeDir, "~");
        strcat(relativeDir, &currentDir[strlen(ShellActivatedDir)]);
    }
    else if (DoesInHome < 0)
    {
        strcpy(relativeDir, currentDir);
    }
    // printf("<%s@%s:%s>", Username, SystemName, relativeDir);
    printf(BOLD GREEN "<%s@%s" RESET, Username, SystemName);
    printf(":");
    printf(BOLD BLUE "%s" RESET, relativeDir);
    if (FlagTimeGrtTwoSec == 1)
    {
        PreviousCommand[strlen(PreviousCommand) - 1] = '\0';
        printf("%s", PreviousCommand);
        FlagTimeGrtTwoSec = 0;
    }

    printf(BOLD BLUE ">" RESET);
}

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

// tokenise command on basis of input and it changes original string
PtrTokStruct TokeniseInput(char *Input, char *WhatToRemove)
{
    PtrTokStruct final;
    final = (PtrTokStruct)malloc(sizeof(tokArrStr));
    char *token;

    // char *ArrOfCommands[MAX_NO_COMMAND];
    final->ArrOfPartsOfOriginalStr = (char **)malloc(sizeof(char *) * MAX_NO_COMMAND);
    token = strtok(Input, WhatToRemove); //? ; & ko nhi dekha
    final->i = 0;
    while (token != NULL)
    {
        final->ArrOfPartsOfOriginalStr[final->i] = token;
        token = strtok(NULL, WhatToRemove);
        final->i++;
    } /* code */
    final->ArrOfPartsOfOriginalStr[final->i] = NULL;
    return final;
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

//? is fn me thodi ye gadbad ho sakti h ki return me kuch change hio  nhi aaye
// char **RemoveStartEndSpace(char **ArrOfCommands, int noOfCommands)
// {
//     for (int i = 0; i < noOfCommands; i++)
//     {
//         int start = 0;
//         while (ArrOfCommands[i][start] == ' ')
//         {
//             ArrOfCommands[i]++;
//             start++;
//         }
//         int end = strlen(ArrOfCommands[i]) - 1;
//         while (ArrOfCommands[i][end] == ' ')
//         {
//             ArrOfCommands[i][end] = '\0';
//             end--;
//         }
//     }
//     return ArrOfCommands;
// }

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

void ExecuteWrap(char *Command)
{

    PtrTokStruct tokenizeStruct = TokeniseInput(Command, " \n\t");
    int i = tokenizeStruct->i;
    char **ArrOfPartsOfCommands = tokenizeStruct->ArrOfPartsOfOriginalStr;

    // If start with ~
    int index = 1;
    if (i == 1)
    {
        StoreCurrDirToPrev();
        chdir(ShellActivatedDir);
        printf("%s\n", ShellActivatedDir);
    }

    while (index < i)
    {
        if (strcmp(ArrOfPartsOfCommands[index], "-") == 0)
        {
            // char current_directory[8000]
            // StoreCurrDirToPrev();
            char currentDir[3024];
            if (getcwd(currentDir, sizeof(currentDir)) == NULL)
            {
                perror("Cant get cwd");
            }
            chdir(Previous_directory);
            strcpy(Previous_directory, currentDir);

            char NEWcurrentDir[3024];
            if (getcwd(NEWcurrentDir, sizeof(NEWcurrentDir)) == NULL)
            {
                perror("Cant get cwd");
            }
            printf("%s\n", NEWcurrentDir);
        }
        else if (ArrOfPartsOfCommands[index][0] == '~')
        {
            StoreCurrDirToPrev();
            if (strlen(ArrOfPartsOfCommands[index]) == 1) // only wrap ~
            {
                chdir(ShellActivatedDir);
            }
            else
            {
                chdir(ShellActivatedDir);
                ArrOfPartsOfCommands[index] = ArrOfPartsOfCommands[index] + 2; // To remove '~/'
                chdir(ArrOfPartsOfCommands[1]);
            }
            char currentDir[3024];
            if (getcwd(currentDir, sizeof(currentDir)) == NULL)
            {
                perror("Cant get cwd");
            }
            printf("%s\n", currentDir);
        }
        // // If start with -
        // if (ArrOfPartsOfCommands[1][0] == '-')
        // {

        // }
        // // If start with ..
        // if (ArrOfPartsOfCommands[1][0] == '..')
        // {

        // }
        else // for both relative and absolute path
        {
            StoreCurrDirToPrev();

            chdir(ArrOfPartsOfCommands[index]);
            char currentDir[1024];
            if (getcwd(currentDir, sizeof(currentDir)) == NULL)
            {
                perror("Cant get cwd");
            }
            printf("%s\n", currentDir);
        }
        index++;
    }
}

void ExecutePeak(char *Command)
{

    PtrTokStruct tokenizeStruct = TokeniseInput(Command, "\t \n");
    // int i= tokenizeStruct->i;
    char **ArrOfPartsOfCommands = tokenizeStruct->ArrOfPartsOfOriginalStr;

    // Is flags Present if yes then path in 3 if nop path in 2
    bool isL = false;
    bool isA = false;
    bool isLA = false;
    if (ArrOfPartsOfCommands[1] != NULL)
    {
        if (strcmp(ArrOfPartsOfCommands[1], "-l") == 0)
        {
            isL = true;
            if (ArrOfPartsOfCommands[2] != NULL && strcmp(ArrOfPartsOfCommands[2], "-a") == 0)
            {
                isA = true;
            }
        }
        if (strcmp(ArrOfPartsOfCommands[1], "-a") == 0)
        {
            isA = true;
            if (ArrOfPartsOfCommands[2] != NULL && strcmp(ArrOfPartsOfCommands[2], "-l") == 0)
            {
                isL = true;
            }
        }
        if (strcmp(ArrOfPartsOfCommands[1], "-la") == 0 || strcmp(ArrOfPartsOfCommands[1], "-al") == 0)
        {
            isLA = true;
        }
    }

    DIR *DirPointer;
    char currentDir[1024];
    if (getcwd(currentDir, sizeof(currentDir)) == NULL)
    {
        perror("Cant get cwd");
    }

    // int DoesInHome = strcmp(currentDir, ShellActivatedDir); // in home if >0  else ==0 then home    OR     < 0 then outside home

    char pathBeforefile[2000];
    //!!! Vo case missing h jisme -l and -a dono ho
    if ((isA == true) && (isL == true))
    {
        if (ArrOfPartsOfCommands[3] != NULL)
        {
            if (ArrOfPartsOfCommands[3][0] == '~')
            {
                // strcpy(pathBeforefile, ShellActivatedDir);
                snprintf(pathBeforefile, sizeof(pathBeforefile), "%s%s", ShellActivatedDir, ArrOfPartsOfCommands[3] + 1);
                DirPointer = opendir(pathBeforefile);
            }
            else if (strcmp(ArrOfPartsOfCommands[3], "-") == 0)
            {
                strcpy(pathBeforefile, Previous_directory);
                DirPointer = opendir(Previous_directory);
            }
            // else
            // {
            //     DirPointer = opendir(ArrOfPartsOfCommands[3]);
            // }
            else if (ArrOfPartsOfCommands[3][0] != '/')
            {
                snprintf(pathBeforefile, sizeof(pathBeforefile), "%s/%s", currentDir, ArrOfPartsOfCommands[3]);
                DirPointer = opendir(ArrOfPartsOfCommands[3]);
            }
            else
            {
                strcpy(pathBeforefile, ArrOfPartsOfCommands[3]);
                DirPointer = opendir(ArrOfPartsOfCommands[3]);
            }
        }
        else
        {
            DirPointer = opendir(currentDir);
            strcpy(pathBeforefile, currentDir);
        }
    }
    else if (isLA == true)
    {
        if (ArrOfPartsOfCommands[2] != NULL)
        {
            if (ArrOfPartsOfCommands[2][0] == '~')
            {
                // strcpy(pathBeforefile, ShellActivatedDir);
                snprintf(pathBeforefile, sizeof(pathBeforefile), "%s%s", ShellActivatedDir, ArrOfPartsOfCommands[2] + 1);
                DirPointer = opendir(pathBeforefile);
            }
            else if (strcmp(ArrOfPartsOfCommands[2], "-") == 0)
            {
                strcpy(pathBeforefile, Previous_directory);
                DirPointer = opendir(Previous_directory);
            }
            // else
            // {
            //     DirPointer = opendir(ArrOfPartsOfCommands[2]);
            // }
            else if (ArrOfPartsOfCommands[2][0] != '/')
            {
                snprintf(pathBeforefile, sizeof(pathBeforefile), "%s/%s", currentDir, ArrOfPartsOfCommands[2]);
                DirPointer = opendir(ArrOfPartsOfCommands[2]);
            }
            else
            {
                strcpy(pathBeforefile, ArrOfPartsOfCommands[2]);
                DirPointer = opendir(ArrOfPartsOfCommands[2]);
            }
        }
        else
        {
            DirPointer = opendir(currentDir);
            strcpy(pathBeforefile, currentDir);
        }
    }
    else if (isA == true || isL == true)
    {
        if (ArrOfPartsOfCommands[2] != NULL)
        {
            // DirPointer = opendir(ArrOfPartsOfCommands[2]);
            if (ArrOfPartsOfCommands[2][0] == '~')
            {
                // strcpy(pathBeforefile, ShellActivatedDir);
                snprintf(pathBeforefile, sizeof(pathBeforefile), "%s%s", ShellActivatedDir, ArrOfPartsOfCommands[2] + 1);
                DirPointer = opendir(pathBeforefile);
            }
            else if (strcmp(ArrOfPartsOfCommands[2], "-") == 0)
            {
                strcpy(pathBeforefile, Previous_directory);
                DirPointer = opendir(Previous_directory);
            }
            // else
            // {
            //     DirPointer = opendir(ArrOfPartsOfCommands[2]);
            // }
            else if (ArrOfPartsOfCommands[2][0] != '/')
            {
                snprintf(pathBeforefile, sizeof(pathBeforefile), "%s/%s", currentDir, ArrOfPartsOfCommands[2]);
                DirPointer = opendir(ArrOfPartsOfCommands[2]);
            }
            else
            {
                strcpy(pathBeforefile, ArrOfPartsOfCommands[2]);
                DirPointer = opendir(ArrOfPartsOfCommands[2]);
            }
        }
        else
        {
            DirPointer = opendir(currentDir);
            strcpy(pathBeforefile, currentDir);
        }
    }
    else
    {
        // DirPointer = opendir(ArrOfPartsOfCommands[1]);

        if (ArrOfPartsOfCommands[1] != NULL)
        {
            if (ArrOfPartsOfCommands[1][0] == '~')
            {
                // strcpy(pathBeforefile, ShellActivatedDir);
                snprintf(pathBeforefile, sizeof(pathBeforefile), "%s%s", ShellActivatedDir, ArrOfPartsOfCommands[1] + 1);
                DirPointer = opendir(pathBeforefile);
            }
            else if (strcmp(ArrOfPartsOfCommands[1], "-") == 0)
            {
                strcpy(pathBeforefile, Previous_directory);
                DirPointer = opendir(Previous_directory);
            }
            // else
            // {
            //     DirPointer = opendir(ArrOfPartsOfCommands[1]);
            // }
            // // DirPointer = opendir(ArrOfPartsOfCommands[1]);
            else if (ArrOfPartsOfCommands[1][0] != '/')
            {
                snprintf(pathBeforefile, sizeof(pathBeforefile), "%s/%s", currentDir, ArrOfPartsOfCommands[1]);
                DirPointer = opendir(ArrOfPartsOfCommands[1]);
            }
            else
            {
                strcpy(pathBeforefile, ArrOfPartsOfCommands[1]);
                DirPointer = opendir(ArrOfPartsOfCommands[1]);
            }
        }
        else
        {
            DirPointer = opendir(currentDir);
            strcpy(pathBeforefile, currentDir);
        }
    }

    if (DirPointer == NULL)
    {
        perror("Unable to open Dir");
        exit(EXIT_FAILURE);
    }
    // printf()
    char PathTillFile[1000];
    struct dirent *ent;
    if (((isL == true) && (isA == true)) || (isLA == true))
    {
        //!!!! Yet to be executed
        while ((ent = readdir(DirPointer)) != NULL)
        {
            struct stat file_stat;
            //??? Isme pura Absolute path dalna padega
            snprintf(PathTillFile, sizeof(PathTillFile), "%s/%s", pathBeforefile, ent->d_name);
            if (stat(PathTillFile, &file_stat) == -1)
            {
                perror("stat");
                continue; // Move to the next directory entry
            }
            // Check if it's a directory
            printf((S_ISDIR(file_stat.st_mode)) ? "d" : "-");
            // Check and print permissions for owner, group, and others
            printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
            printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
            // ... continue for all permissions
            printf(" ");
            struct passwd *user_info = getpwuid(file_stat.st_uid);
            struct group *group_info = getgrgid(file_stat.st_gid);
            if (user_info && group_info)
            {
                printf("%s %s ", user_info->pw_name, group_info->gr_name);
            }
            printf("%ld ", file_stat.st_size);
            char time_str[20];
            strftime(time_str, sizeof(time_str), "%b %d %H:%M", localtime(&file_stat.st_mtime));
            printf("%s ", time_str);
            if (IsIOActivated == 0)
            {
                if (S_ISDIR(file_stat.st_mode))
                {
                    // Dir
                    printf(BLUE "%s\n" RESET, ent->d_name);
                }
                else if (file_stat.st_mode & S_IXUSR)
                {
                    // executable
                    printf(GREEN "%s\n" RESET, ent->d_name);
                }
                // printf("%s\n", ent->d_name);
                else
                {
                    printf("%s\n", ent->d_name);
                }
            }
            else
            {
                printf("%s\n", ent->d_name);
            }
        }
    }

    else if (isA == true)
    {
        // printf("Yes -a is true\n");
        while ((ent = readdir(DirPointer)) != NULL)
        {
            struct stat file_stat;
            //??? Isme pura Absolute path dalna padega
            snprintf(PathTillFile, sizeof(PathTillFile), "%s/%s", pathBeforefile, ent->d_name);
            if (stat(PathTillFile, &file_stat) == -1)
            {
                perror("stat");
                continue; // Move to the next directory entry
            }
            if (IsIOActivated == 0)
            {
                if (S_ISDIR(file_stat.st_mode))
                {
                    // Dir
                    printf(BLUE "%s\n" RESET, ent->d_name);
                }
                else if (file_stat.st_mode & S_IXUSR)
                {
                    // executable
                    printf(GREEN "%s\n" RESET, ent->d_name);
                }
                // printf("%s\n", ent->d_name);
                else
                {
                    printf("%s\n", ent->d_name);
                }
            }
            else
            {
                printf("%s\n", ent->d_name);
            }
        }
    }
    else if (isL == true)
    {
        //!!!! Yet to be executed
        while ((ent = readdir(DirPointer)) != NULL)
        {
            if (ent->d_name[0] != '.')
            {

                struct stat file_stat;
                //??? Isme pura Absolute path dalna padega
                snprintf(PathTillFile, sizeof(PathTillFile), "%s/%s", pathBeforefile, ent->d_name);
                if (stat(PathTillFile, &file_stat) == -1)
                {
                    perror("stat");
                    continue; // Move to the next directory entry
                }
                // Check if it's a directory
                printf((S_ISDIR(file_stat.st_mode)) ? "d" : "-");
                // Check and print permissions for owner, group, and others
                printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
                printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
                printf(" ");

                // ... continue for all permissions
                struct passwd *user_info = getpwuid(file_stat.st_uid);
                struct group *group_info = getgrgid(file_stat.st_gid);
                if (user_info && group_info)
                {
                    printf("%s %s ", user_info->pw_name, group_info->gr_name);
                }
                printf("%ld ", file_stat.st_size);
                char time_str[20];
                strftime(time_str, sizeof(time_str), "%b %d %H:%M", localtime(&file_stat.st_mtime));
                printf("%s ", time_str);
                // if (stat(ent->d_name, &file_stat) == -1)
                // {
                //     perror("stat");
                //     continue; // Move to the next directory entry
                // }
                if (IsIOActivated == 0)
                {
                    if (S_ISDIR(file_stat.st_mode))
                    {
                        // Dir
                        printf(BLUE "%s\n" RESET, ent->d_name);
                    }
                    else if (file_stat.st_mode & S_IXUSR)
                    {
                        // executable
                        printf(GREEN "%s\n" RESET, ent->d_name);
                    }
                    // printf("%s\n", ent->d_name);
                    else
                    {
                        printf("%s\n", ent->d_name);
                    }
                }
                else
                {
                    printf("%s\n", ent->d_name);
                }
            }
        }
    }
    else
    {
        while ((ent = readdir(DirPointer)) != NULL)
        {

            if (ent->d_name[0] != '.')
            {
                struct stat file_stat;
                //??? Isme pura Absolute path dalna padega
                snprintf(PathTillFile, sizeof(PathTillFile), "%s/%s", pathBeforefile, ent->d_name);
                if (stat(PathTillFile, &file_stat) == -1)
                {
                    perror("stat");
                    continue; // Move to the next directory entry
                }
                // if (stat(ent->d_name, &file_stat) == -1)
                // {
                //     perror("stat");
                //     continue; // Move to the next directory entry
                // }
                if (IsIOActivated == 0)
                {
                    if (S_ISDIR(file_stat.st_mode))
                    {
                        // Dir
                        printf(BLUE "%s\n" RESET, ent->d_name);
                    }
                    else if (file_stat.st_mode & S_IXUSR)
                    {
                        // executable
                        printf(GREEN "%s\n" RESET, ent->d_name);
                    }
                    // printf("%s\n", ent->d_name);
                    else
                    {
                        printf("%s\n", ent->d_name);
                    }
                }
                else
                {
                    printf("%s\n", ent->d_name);
                }
            }
        }
    }
    closedir(DirPointer);
}

//
void addEntry(char *entry)
{
    strcpy(QueueBuffer[QueueEnd], entry);
    QueueEnd = (QueueEnd + 1) % MAX_ENTRIES;
    if (QueueStart == QueueEnd)
    {
        QueueStart = (QueueStart + 1) % MAX_ENTRIES;
    }
}

void LoadHistory(char *FileName)
{

    FILE *fp = fopen(FileName, "r");
    if (fp != NULL)
    {
        char entry[ENTRY_SIZE];
        while (fgets(entry, sizeof(entry), fp))
        {
            addEntry(entry);
        }
    }

    fclose(fp);
}

// Call At Last whn to save in file

void SaveEntry(char *FileName)
{
    FILE *fp = fopen(FileName, "w");
    int i = QueueStart;
    while ((i - 1) != QueueEnd)
    {
        fputs(QueueBuffer[i], fp);
        i = (i + 1) % MAX_ENTRIES;
    }
    //
    fclose(fp);
}
void handle_sigint()
{
    char PathToSave[1000];
    AddCommandWithCondition("exit\n");
    snprintf(PathToSave, sizeof(PathToSave), "%s/history.txt", ShellActivatedDir);
    SaveEntry(PathToSave);
    // printf("\n");
    // kill()
    // bgProcess *Curr = ActiVitesListHead;
    // // bgProcess *Store = curr;
    // int status;
    // while (Curr != NULL)
    // {
    //     kill(Curr->PID, SIGTERM);
    //     Curr = Curr->Next;
    // }
    kill(foreground_process_pid, SIGINT);
    printf("\n");
    // Display_name_A(ShellActivatedDir);
    // fflush(stdout);
    // exit(0);
}

void handle_ctrlD()
{
    char PathToSave[1000];
    AddCommandWithCondition("exit\n");
    snprintf(PathToSave, sizeof(PathToSave), "%s/history.txt", ShellActivatedDir);
    SaveEntry(PathToSave);
    // printf("\n");
    // kill()
    bgProcess *Curr = ActiVitesListHead;
    // bgProcess *Store = curr;
    int status;
    while (Curr != NULL)
    {
        kill(Curr->PID, SIGTERM);
        Curr = Curr->Next;
    }
    exit(0);
    // break;
}

// void handle_sigTSTP(int sig) {
//     if (foreground_process_pid != -1) { // Check if a foreground process is running
//         kill(foreground_process_pid, SIGSTOP); // Send the SIGSTOP signal to stop it
//         printf("\nProcess with PID %d has been stopped.\n", foreground_process_pid);
//         // Update the process state in your data structures to "Stopped"
//     }
// }
// pid_t foreground_process_pid = -1;  // global variable to hold the PID of the foreground process

void handle_sigtstp(int sig)
{
    // if (foreground_process_pid != -1)
    // {
    //     // Stop the foreground process if it's running
    //     kill(foreground_process_pid, SIGSTOP);
    //     printf("\nProcess with PID %d has been stopped.\n", foreground_process_pid);
    //     foreground_process_pid = -1; // Reset the foreground PID
    // }
    // else
    // {
    //     printf("\nNo foreground process running.\n");
    // }
    // // Display your shell prompt again, for example:
    // // printf("> ");
    // Display_name_A(ShellActivatedDir);
    // fflush(stdout);
    kill(foreground_process_pid, SIGSTOP);
    kill(foreground_process_pid, SIGTSTP);
    printf("\n");
}

void AddCommandWithCondition(char *command)
{
    char CommandCopy[ENTRY_SIZE];
    strcpy(CommandCopy, command);
    // PtrTokStruct tokenizeStruct=TokeniseInput(CommandCopy," \n\t");
    // int i= tokenizeStruct->i;
    // char **TokenisedCommand=tokenizeStruct->ArrOfPartsOfOriginalStr;
    if (strcmp(command, QueueBuffer[QueueEnd - 1]) == 0)
    {
        // DO NOTHING
        return;
    }
    //? sTORE ONLY VALID COMMAND
    // else if (strcmp(TokenisedCommand[0], "pastevents") == 0)
    else if (strstr(command, "pastevents") != NULL)
    {
        // Do Nothing
        return;
    }
    //
    else
    {
        addEntry(command);
        return;
    }
}

void ExecutePastevents(char *Command)
{
    char CommandCopy[ENTRY_SIZE];
    strcpy(CommandCopy, Command);
    PtrTokStruct tokenizeStruct = TokeniseInput(CommandCopy, " \n\t");
    // int i= tokenizeStruct->i;
    char **TokenisedCommand = tokenizeStruct->ArrOfPartsOfOriginalStr;

    if (TokenisedCommand[1] == NULL)
    {
        // Commnd=pastevents
        int index = QueueStart;
        while (index != QueueEnd)
        {
            printf("%s", QueueBuffer[index]);
            index = (index + 1) % MAX_ENTRIES;
        }
    }
    else if (strcmp(TokenisedCommand[1], "purge") == 0)
    {
        // Commnd=pastevents purge
        QueueStart = 0;
        QueueEnd = 0;
    }
    else if (strcmp(TokenisedCommand[1], "execute") == 0)
    {
        // Commnd=pastevents execute
        int ComandNo = atoi(TokenisedCommand[2]);
        ExecuteCommand(QueueBuffer[QueueEnd - ComandNo]);
        AddCommandWithCondition(QueueBuffer[QueueEnd - ComandNo]);
    }
}

// return total no of command at end

void ExecuteProclore(char *Command)
{

    PtrTokStruct tokenizeStruct = TokeniseInput(Command, " \n\t");
    int i = tokenizeStruct->i;
    char **ArrOfPartsOfCommands = tokenizeStruct->ArrOfPartsOfOriginalStr;

    //* Isi process ki nikalni h
    int PID;
    if (i == 1)
    {
        PID = getpid();
    }
    //* Dusra process Diya h
    else
    {
        PID = atoi(ArrOfPartsOfCommands[1]);
    }
    printf("pid : %d\n", PID);
    char PathToProcessFile[1024];
    // strcpy(PathToProcessFile,"/proc/");
    // strcat(PathToProcessFile,)
    snprintf(PathToProcessFile, sizeof(PathToProcessFile), "/proc/%d/status", PID);
    FILE *fp = fopen(PathToProcessFile, "r");
    if (fp != NULL)
    {
        char line[500];
        int a = 0, b = 0, c = 0;
        while (fgets(line, sizeof(line), fp))
        {
            // printf("======>%s",line);
            if (strstr(line, "State:") != NULL)
            {
                if (line[9] != '+')
                {
                    line[9] = '\0';
                }
                else
                {
                    line[10] = '\0';
                }
                printf("process %s\n", line);

                a = 1;
            }
            if (strstr(line, "NStgid:") != NULL)
            {
                printf("Process Group %s", line + 7);
                b = 1;
            }
            if (strstr(line, "VmSize:") != NULL)
            {
                printf("Virtual memory %s", line + 7);
                c = 1;
            }
            if ((a == 1) && (b == 1) && (c == 1))
            {
                break;
            }
        }
        fclose(fp);
    }
    snprintf(PathToProcessFile, sizeof(PathToProcessFile), "/proc/%d/exe", PID);
    ssize_t len = readlink(PathToProcessFile, PathToProcessFile, sizeof(PathToProcessFile) - 1);
    if (len != -1)
    {
        PathToProcessFile[len] = '\0';
        char relativeDir[1024];
        strcpy(relativeDir, "~");
        strcat(relativeDir, &PathToProcessFile[strlen(ShellActivatedDir)]);
        printf("executable path: %s\n", relativeDir);
    }
}

void SeekRecursiveOpener(char *WhereToFind, char *ThingToFind, bool isD, bool isF, bool isE, char *DirInWhichSeekCalled)
{
    DIR *dir;
    struct dirent *ent;
    char FullPath[1024];
    if ((dir = opendir(WhereToFind)) == NULL)
    {
        return;
    }
    while ((ent = readdir(dir)) != NULL)
    {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
        {
            continue;
        }
        struct stat statbuf;
        // statbuf.st_mode
        snprintf(FullPath, sizeof(FullPath), "%s/%s", WhereToFind, ent->d_name);
        char CurrentWorkingDir[1024];
        if (getcwd(CurrentWorkingDir, sizeof(CurrentWorkingDir)) == NULL)
        {
            perror("Error in getcwd");
        }
        // char PathStat[1000];
        // snprintf(PathStat,sizeof(PathStat),"%s/%s",CurrentWorkingDir,ent->d_name)
        stat(FullPath, &statbuf);

        if (strstr(ent->d_name, ThingToFind) != NULL)
        {
            if (isD == true)
            {
                if (S_ISDIR(statbuf.st_mode))
                {
                    // printf(".%s\n", FullPath + strlen(DirInWhichSeekCalled));
                    strcpy(Previous_directory, CurrentWorkingDir);
                    chdir(FullPath);
                    // WhileSeekNoTimesPrint++;
                }
            }
            if (isF == true)
            {
                if (S_ISREG(statbuf.st_mode))
                {
                    // printf(".%s\n", FullPath + strlen(DirInWhichSeekCalled));
                    FILE *fp = fopen(FullPath, "r");
                    char line[5000];
                    while (fgets(line, sizeof(line), fp))
                    {
                        printf("%s", line);
                    }
                    // WhileSeekNoTimesPrint++;
                }
            }
        }
        SeekRecursiveOpener(FullPath, ThingToFind, isD, isF, isE, DirInWhichSeekCalled);
    }
    closedir(dir);
}

void SeekRecursive(char *WhereToFind, char *ThingToFind, bool isD, bool isF, bool isE, char *DirInWhichSeekCalled)
{
    DIR *dir;
    struct dirent *ent;
    char FullPath[1024];
    if ((dir = opendir(WhereToFind)) == NULL)
    {
        return;
    }
    while ((ent = readdir(dir)) != NULL)
    {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
        {
            continue;
        }
        struct stat statbuf;
        // statbuf.st_mode
        snprintf(FullPath, sizeof(FullPath), "%s/%s", WhereToFind, ent->d_name);
        char CurrentWorkingDir[1024];
        if (getcwd(CurrentWorkingDir, sizeof(CurrentWorkingDir)) == NULL)
        {
            perror("Error in getcwd");
        }
        // char PathStat[1000];
        // snprintf(PathStat,sizeof(PathStat),"%s/%s",CurrentWorkingDir,ent->d_name)
        stat(FullPath, &statbuf);

        if (strstr(ent->d_name, ThingToFind) != NULL)
        {
            if (isD == true)
            {
                if (S_ISDIR(statbuf.st_mode))
                {
                    printf(".%s\n", FullPath + strlen(DirInWhichSeekCalled));
                    WhileSeekNoTimesPrint++;
                }
            }
            if (isF == true)
            {
                if (S_ISREG(statbuf.st_mode))
                {
                    printf(".%s\n", FullPath + strlen(DirInWhichSeekCalled));
                    WhileSeekNoTimesPrint++;
                }
            }
            if (isD == false && isF == false)
            {
                printf(".%s\n", FullPath + strlen(DirInWhichSeekCalled));
                WhileSeekNoTimesPrint++;
            }
        }
        SeekRecursive(FullPath, ThingToFind, isD, isF, isE, DirInWhichSeekCalled);
    }
    closedir(dir);
}

void ExecuteSeek(char *Command)
{
    bool isD = false, isF = false, isE = false;
    int NoOfFlag = 0;
    if (strstr(Command, "-d") != NULL)
    {
        isD = true;
        NoOfFlag++;
    }
    if (strstr(Command, "-f") != NULL)
    {
        isF = true;
        NoOfFlag++;
    }
    if (strstr(Command, "-e") != NULL)
    {
        isE = true;
        NoOfFlag++;
    }
    // Arg 1 to which we look
    // Arg 2 to where we look

    if ((isD == true) && (isF == true))
    {
        printf("Invalid flags!\n");
        return;
    }

    char CommandCopy[ENTRY_SIZE];
    strcpy(CommandCopy, Command);
    PtrTokStruct tokenizeStruct = TokeniseInput(CommandCopy, " \n\t");
    // int i = tokenizeStruct->i;
    char **ArrOfPartOfCommand = tokenizeStruct->ArrOfPartsOfOriginalStr;

    char *ToFind = ArrOfPartOfCommand[NoOfFlag + 1];
    char WhereFound[1024];
    char CurrentDir[1024];
    if (getcwd(CurrentDir, sizeof(CurrentDir)) == NULL)
    {
        perror("Error in getcwd");
    }
    if (ArrOfPartOfCommand[NoOfFlag + 2] != NULL)
    {
        // strcpy(WhereFound, ArrOfPartOfCommand[NoOfFlag + 2]);
        if (ArrOfPartOfCommand[NoOfFlag + 2][0] == '-')
        {
            strcpy(WhereFound, Previous_directory);
        }
        // else if (/* condition */)
        else if (ArrOfPartOfCommand[NoOfFlag + 2][0] == '~')
        {
            // snprintf(WhereFound, sizeof(WhereFound), "%s/%s", ShellActivatedDir, ArrOfPartOfCommand[NoOfFlag + 2] + 2);
            strcpy(WhereFound, ShellActivatedDir);
        }
        else
        {
            snprintf(WhereFound, sizeof(WhereFound), "%s/%s", CurrentDir, ArrOfPartOfCommand[NoOfFlag + 2]);
        }
    }
    else
    {
        strcpy(WhereFound, CurrentDir);
    }

    SeekRecursive(WhereFound, ToFind, isD, isF, isE, WhereFound);
    if (WhileSeekNoTimesPrint == 0)
    {
        printf("No match found!\n");
    }
    if ((isD == true) && (isE == true))
    {
        if (WhileSeekNoTimesPrint == 1)
        {
            SeekRecursiveOpener(WhereFound, ToFind, isD, isF, isE, WhereFound);
        }
    }
    else if ((isF == true) && (isE == true))
    {
        if (WhileSeekNoTimesPrint == 1)
        {
            SeekRecursiveOpener(WhereFound, ToFind, isD, isF, isE, WhereFound);
        }
    }
    WhileSeekNoTimesPrint = 0;
}

void stripHTMLTags(char *str)
{
    int insideTag = 0;
    int j = 0;
    for (int i = 0; str[i]; i++)
    {
        if (str[i] == '<')
        {
            insideTag = 1;
            continue;
        }
        if (insideTag)
        {
            if (str[i] == '>')
            {
                insideTag = 0;
            }
            continue;
        }
        str[j++] = str[i];
    }
    str[j] = '\0';
}

void iMan(const char *command_name)
{
    char *token = strtok_r(command_name, " \t\n", &command_name);
    struct addrinfo hints;
    struct addrinfo *res;
    int sockfd;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    char link_updated[4097];
    // snprintf(link_updated , 4097 , "man.he.net");
    strcpy(link_updated, "man.he.net");
    int status = getaddrinfo(link_updated, "http", &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "DNS resolution failed: %s\n", gai_strerror(status));
        // return 1;
    }
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1)
    {
        perror("socket");
        // return 1;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        perror("connect");
        // return 1;
    }

    char request[4096];
    // snprintf(request, sizeof(request), "GET /%s.1 HTTP/1.1\r\nHost: man.he.net\r\n\r\n",token);
    // snprintf(request, sizeof(request), "GET /man1/%s HTTP/1.1\r\nHost: man.he.net\r\nUser-Agent: iMan/1.0\r\n\r\n", token);
    snprintf(request, sizeof(request), "GET http://man.he.net/?topic=%s&section=all HTTP/1.1\r\nHost: man.he.net\r\n\r\n", command_name);

    if (send(sockfd, request, strlen(request), 0) == -1)
    {
        perror("send");
        // return 1;
    }
    printf("%s", request);
    char response[4096];
    int bytes_received;

    while ((bytes_received = recv(sockfd, response, sizeof(response) - 1, 0)) > 0)
    {
        response[bytes_received] = '\0';
        stripHTMLTags(response);
        printf("%s", response);
    }

    if (bytes_received == -1)
    {
        perror("recv");
        // return 1;
    }

    close(sockfd);
    freeaddrinfo(res);

    return command_name;
}
void ExecuteiMan(char *Command)
{
    // Remove iMan
    Command = Command + 1;

    // Remove Space
    int i = 0;
    while (Command[i] == ' ')
    {
        Command++;
    }
    iMan(Command);
}

IO IoRedirection(char *Command)
{
    char CommandCopy[ENTRY_SIZE];
    strcpy(CommandCopy, Command);
    PtrTokStruct tokenizeStruct = TokeniseInput(CommandCopy, " \n\t");
    // int i= tokenizeStruct->i;
    char **ArrOfPartOfCommand = tokenizeStruct->ArrOfPartsOfOriginalStr;
    IO Ans;
    Ans.fd_in = dup(STDIN_FILENO);
    Ans.fd_out = dup(STDOUT_FILENO);
    char *input_file = NULL, *output_file = NULL;
    int append_mode = 0;
    int index = 0;
    while (ArrOfPartOfCommand[index] != NULL)
    {
        if (strcmp(ArrOfPartOfCommand[index], "<") == 0)
        {
            input_file = ArrOfPartOfCommand[index + 1];
        }
        else if (strcmp(ArrOfPartOfCommand[index], ">") == 0)
        {
            output_file = ArrOfPartOfCommand[index + 1];
        }
        else if (strcmp(ArrOfPartOfCommand[index], ">>") == 0)
        {
            append_mode = 1;
            output_file = ArrOfPartOfCommand[index + 1];
        }
        index++;
    }
    if (input_file)
    {
        Ans.x = 1;
        int fd_in = open(input_file, O_RDONLY);
        if (fd_in < 0)
        {
            perror("Error opening input file");
            // exit(EXIT_FAILURE);
            Ans.x = -999;
            return Ans;
        }
        // int saved_stdin = dup(STDIN_FILENO);
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
        IsIOActivated = 1;
    }
    if (output_file)
    {
        int fd_out;
        if (append_mode)
        {
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            Ans.x = 1;
            IsIOActivated = 1;
        }
        else
        {
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            Ans.x = 2;
            IsIOActivated = 1;
        }
        if (fd_out < 0)
        {
            perror("Error opening output file");
            exit(EXIT_FAILURE);
        }
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
    }
    return Ans;
}

void IORegain(IO Saved)
{
    dup2(Saved.fd_in, STDIN_FILENO);
    close(Saved.fd_in);
    dup2(Saved.fd_out, STDOUT_FILENO);
    close(Saved.fd_out);
    IsIOActivated = 0;
}

void ExecuteFgBg(char *Command)
{
    char *CommandCpy = (char *)malloc(sizeof(char) * 200);
    strcpy(CommandCpy, Command);
    PtrTokStruct tokenizeStruct = TokeniseInput(CommandCpy, " \t\n");
    int NoOfPart = tokenizeStruct->i;
    char **ArrOfPartOfCommand = tokenizeStruct->ArrOfPartsOfOriginalStr;
    if (strcmp(ArrOfPartOfCommand[0], "fg") == 0)
    {
        int ProcessId = atoi(ArrOfPartOfCommand[1]);
        kill(ProcessId, SIGCONT);
        int Status;
        waitpid(ProcessId, &Status, WUNTRACED);
    }
    else if (strcmp(ArrOfPartOfCommand[0], "bg") == 0)
    {
        int ProcessId = atoi(ArrOfPartOfCommand[1]);
        kill(ProcessId, SIGCONT);
    }
}

char **tokenize_withQoutes(const char *str, const char *delimiters, int *token_count)
{
    char current_quote = '\0'; // '\0' means we're not inside quotes
    int start = 0;
    int capacity = 10; // initial capacity for token array
    int count = 0;

    char **tokens = malloc(capacity * sizeof(char *));
    if (!tokens)
    {
        perror("Failed to allocate memory");
        exit(1);
    }

    for (int i = 0; str[i] != '\0'; ++i)
    {
        // Handle quote characters
        if (str[i] == '"' || str[i] == '\'')
        {
            if (current_quote == '\0')
            {
                // Not inside quotes, set the current quote
                current_quote = str[i];
            }
            else if (current_quote == str[i])
            {
                // Encountered the closing quote
                current_quote = '\0';
            }
        }

        if (current_quote == '\0' && strchr(delimiters, str[i]))
        {
            if (i > start)
            {
                tokens[count] = strndup(&str[start], i - start);
                count++;

                if (count >= capacity)
                {
                    capacity *= 2;
                    tokens = realloc(tokens, capacity * sizeof(char *));
                    if (!tokens)
                    {
                        perror("Failed to reallocate memory");
                        exit(1);
                    }
                }
            }
            start = i + 1;
        }
    }

    if (str[start] != '\0')
    {
        tokens[count] = strdup(&str[start]);
        count++;
    }

    *token_count = count;
    for (int i = 0; i < count; i++)
    {
        if (tokens[i][0] == '\'' || tokens[i][0] == '\"')
        {
            tokens[i]++;
            tokens[i][strlen(tokens[i]) - 1] = '\0';
        }
    }
    return tokens;
}

// ? NEOnate begin
int get_recent_pid()
{
    FILE *file;
    int pid;

    file = fopen("/proc/sys/kernel/ns_last_pid", "r");
    if (file == NULL)
    {
        perror("Error opening ns_last_pid");
        return -1;
    }
    fscanf(file, "%d", &pid);
    fclose(file);
    return pid;
}

void die(const char *s)
{
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

int stopPrinting = 0;

void handleSignal2(int signo)
{
    if (signo == SIGINT)
    {
        stopPrinting = 1;
    }
}

void ExecuteNeonate(char *Command)
{
    char CommandCopy[ENTRY_SIZE];
    strcpy(CommandCopy, Command);
    PtrTokStruct tokenizeStruct = TokeniseInput(CommandCopy, " \t\n");
    int NoOfPart = tokenizeStruct->i;
    char **ArrOfPartOfCommand = tokenizeStruct->ArrOfPartsOfOriginalStr;

    // struct termios oldt;
    char ch;
    int delay = atoi(ArrOfPartOfCommand[2]);
    // set_terminal_raw_mode(&oldt);
    enableRawMode();
    signal(SIGINT, handleSignal2);

    int PID = fork();
    if (PID < 0)
    {
        perror("IN FORK IN NEONATE");
    }
    else if (PID == 0)
    {
        while (!stopPrinting)
        {
            printf("%d\n", get_recent_pid());
            sleep(delay);
        }
        exit(0);
    }
    else
    {
        while (1)
        {
            read(STDIN_FILENO, &ch, 1);
            if (ch == 'x')
            {
                kill(PID, SIGINT);
                wait(NULL);
                break;
            }
        }
    }

    // do
    // {
    //     printf("%d\n", get_recent_pid());
    //     sleep(delay);
    //     ch = get_single_keypress();
    // } while (ch != 'x');
    // restore_terminal_mode(&oldt);
    disableRawMode();
}
// ? NEOnate END

void ExecuteCommand(char *Command)
{
    // ? IO Start
    int IndexI = 0;
    char CommandCopy2[ENTRY_SIZE];
    strcpy(CommandCopy2, Command);
    IO SavedIO = IoRedirection(CommandCopy2);
    if (SavedIO.x == -999)
    {
        return;
    }

    while (Command[IndexI] != '\0')
    {
        if (Command[IndexI] == '<' || Command[IndexI] == '>' || (Command[IndexI] == '>' && Command[IndexI + 1] == '>'))
        {
            Command[IndexI] = '\0';
            break;
        }
        IndexI++;
    }
    // ? IO End
    //

    //
    // ? PIPE START
    char CommandCopyPipe[ENTRY_SIZE];
    strcpy(CommandCopyPipe, Command);
    PtrTokStruct tokenizeStructPIPE = TokeniseInput(CommandCopyPipe, "|\n");
    int NoOfPipedPart = tokenizeStructPIPE->i;
    char **ArrOfPartOfCommandPIPE = tokenizeStructPIPE->ArrOfPartsOfOriginalStr;
    int fd[2];
    int fdd = 0;
    // ? PIPE END

    pid_t PID;

    for (int i = 0; i < NoOfPipedPart; i++)
    {

        //? ExecuteOtherCommandPart Start
        while (ArrOfPartOfCommandPIPE[i][strlen(ArrOfPartOfCommandPIPE[i]) - 1] == '\n')
        {
            ArrOfPartOfCommandPIPE[i][strlen(ArrOfPartOfCommandPIPE[i]) - 1] = '\0';
        }
        char PipedPartOfCommand[ENTRY_SIZE];
        strcpy(PipedPartOfCommand, ArrOfPartOfCommandPIPE[i]);
        // !!!!!!PtrTokStruct tokenizeStruct = TokeniseInput(PipedPartOfCommand, " \n\t");
        int iPipeSpace;
        char **ArrOfPartOfPIPEDCommandSpace = tokenize_withQoutes(PipedPartOfCommand, " \n\t", &iPipeSpace);

        int DoesBg = DoesEndWithChar(ArrOfPartOfCommandPIPE[i], '&');
        if (DoesBg == 1)
        {
            ArrOfPartOfCommandPIPE[i][strlen(ArrOfPartOfCommandPIPE[i]) - 1] = '\0';
            int end = strlen(ArrOfPartOfCommandPIPE[i]) - 1;
            while (ArrOfPartOfCommandPIPE[i][end] == ' ')
            {
                ArrOfPartOfCommandPIPE[i][end] = '\0';
                end--;
            }
        }

        // if & in last ArrOfPartOfCommand then remove it
        if (DoesBg == 1)
        {
            ArrOfPartOfPIPEDCommandSpace[iPipeSpace - 1] = NULL;
        }
        else
        {
            ArrOfPartOfPIPEDCommandSpace[iPipeSpace] = NULL;
        }
        //? ExecuteOtherCommandPart End

        // if (strcmp(ArrOfPartOfPIPEDCommandSpace[0], "activities") == 0)
        // {
        //     printf("Executiong Act\n");
        //     PrintAllProcess();
        // }
        // if (strcmp(ArrOfPartOfPIPEDCommandSpace[0], "fg") == 0 || strcmp(ArrOfPartOfPIPEDCommandSpace[0], "bg") == 0)
        // {
        //     ExecuteFgBg(ArrOfPartOfCommandPIPE[i]);
        // }
        // if (strcmp(ArrOfPartOfPIPEDCommandSpace[0], "fg") == 0)
        // {
        //     // ExecuteFgBg(ArrOfPartOfCommandPIPE[i]);
        //     int ProcessId = atoi(ArrOfPartOfPIPEDCommandSpace[1]);
        //     int Status;
        //     if(kill(ProcessId, SIGCONT)==-1)
        //     {
        //         perror("No process found");
        //     }
        //     waitpid(ProcessId, &Status, WUNTRACED);
        // }
        // else if (strcmp(ArrOfPartOfPIPEDCommandSpace[0], "bg") == 0)
        // {
        //     // ExecuteFgBg(ArrOfPartOfCommandPIPE[i]);
        //     int ProcessId = atoi(ArrOfPartOfPIPEDCommandSpace[1]);
        //     kill(ProcessId, SIGCONT);
        // }

        pipe(fd);
        PID = fork();
        if (PID < 0)
        {
            perror("Error in fork");
        }
        // child process
        else if (PID == 0)
        {
            // foreground_process_pid = getpid();
            //? PIPE Start
            dup2(fdd, 0);
            if (ArrOfPartOfCommandPIPE[i + 1] != NULL)
                dup2(fd[1], 1);
            close(fd[0]);
            // ? PIPE End

            if (ArrOfPartOfCommandPIPE[0] == NULL)
            {
                return;
            }
            if (strcmp(ArrOfPartOfPIPEDCommandSpace[0], "peek") == 0)
            {
                // printf("Executing Peek\n");
                ExecutePeak(ArrOfPartOfCommandPIPE[i]);
            }
            else if (strcmp(ArrOfPartOfPIPEDCommandSpace[0], "warp") == 0)
            {
                // printf("Executing warp\n");
                ExecuteWrap(ArrOfPartOfCommandPIPE[i]);
            }
            else if (strcmp(ArrOfPartOfPIPEDCommandSpace[0], "pastevents") == 0)
            {
                // printf("Executing pastevents\n");
                ExecutePastevents(ArrOfPartOfCommandPIPE[i]);
            }
            else if (strcmp(ArrOfPartOfPIPEDCommandSpace[0], "proclore") == 0)
            {
                // printf("Executing proclore\n");
                ExecuteProclore(ArrOfPartOfCommandPIPE[i]);
            }
            else if (strcmp(ArrOfPartOfPIPEDCommandSpace[0], "seek") == 0)
            {
                // printf("Executing seek\n");
                ExecuteSeek(ArrOfPartOfCommandPIPE[i]);
            }
            else if (strcmp(ArrOfPartOfPIPEDCommandSpace[0], "activities") == 0)
            {
                PrintAllProcess();
            }
            else if (strcmp(ArrOfPartOfPIPEDCommandSpace[0], "ping") == 0)
            {
                // PrintAllProcess();
                int processID = atoi(ArrOfPartOfPIPEDCommandSpace[1]);
                int signal = atoi(ArrOfPartOfPIPEDCommandSpace[2]);
                signal = (signal % 32);
                kill(processID, signal);
            }
            else if (strcmp(ArrOfPartOfPIPEDCommandSpace[0], "iMan") == 0)
            {
                ExecuteiMan(ArrOfPartOfCommandPIPE[i]);
            }
            else if (strcmp(ArrOfPartOfPIPEDCommandSpace[0], "fg") == 0)
            {
                // ExecuteFgBg(ArrOfPartOfCommandPIPE[i]);
                // int ProcessId = atoi(ArrOfPartOfPIPEDCommandSpace[1]);
                // int Status;
                // kill(ProcessId, SIGCONT);
                // waitpid(ProcessId, &Status, 0);
                int ProcessId = atoi(ArrOfPartOfPIPEDCommandSpace[1]);
                int Status;
                if (kill(ProcessId, SIGCONT) == -1)
                {
                    perror("No process found");
                }
                waitpid(ProcessId, &Status, WUNTRACED);
            }
            else if (strcmp(ArrOfPartOfPIPEDCommandSpace[0], "bg") == 0)
            {
                // ExecuteFgBg(ArrOfPartOfCommandPIPE[i]);
                int ProcessId = atoi(ArrOfPartOfPIPEDCommandSpace[1]);
                // kill(ProcessId, SIGCONT);
                if (kill(ProcessId, SIGCONT) == -1)
                {
                    perror("No process found");
                }
            }
            else if (strcmp(ArrOfPartOfPIPEDCommandSpace[0], "neonate") == 0)
            {
                // PrintAllProcess();
                ExecuteNeonate(ArrOfPartOfCommandPIPE[i]);
            }
            else
            {
                // ExecuteOtherCommand(Command);
                int RetExe = execvp(ArrOfPartOfPIPEDCommandSpace[0], ArrOfPartOfPIPEDCommandSpace);
                if (RetExe == -1)
                {
                    // perror("Error in Execvp\n");
                    printf("ERROR : '%s' is not a valid command\n", ArrOfPartOfPIPEDCommandSpace[0]);
                }
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            InsertProcessActivites(PID, ArrOfPartOfCommandPIPE[i]);
            foreground_process_pid = PID;
            if (DoesBg == 1)
            {
                // isme wait nhi karna
                InsertProcess(PID, ArrOfPartOfPIPEDCommandSpace[0]);
                printf("%d\n", PID);
            }
            else
            {
                struct timespec start, end;
                clock_gettime(CLOCK_MONOTONIC, &start);
                waitpid(PID, NULL, WUNTRACED);
                clock_gettime(CLOCK_MONOTONIC, &end);
                double TimeDiff = end.tv_sec - start.tv_sec;
                if (TimeDiff > 2)
                {
                    // printf("%s  %lf", ArrOfPartOfCommand[0], TimeDiff);
                    FlagTimeGrtTwoSec = 1;
                }
            }
            close(fd[1]);
            fdd = fd[0];
        }
    }
    IORegain(SavedIO);
}




int main()
{
    signal(SIGINT, handle_sigint);
    // struct sigaction sa;
    // sa.sa_handler = handle_sigTSTP;
    // sa.sa_flags = SA_RESTART;
    // sigaction(SIGTSTP, &sa, NULL);
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
        // printf("%s\n\n\n",input);

        int NoOfCommands = 0;
        while (ArrOfCommands[NoOfCommands] != NULL)
        {
            ExecuteCommand(ArrOfCommands[NoOfCommands]);
            NoOfCommands++;
        }
    }
}
