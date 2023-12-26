#include "headers.h"
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