#include "headers.h"

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
