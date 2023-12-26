#include "headers.h"
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