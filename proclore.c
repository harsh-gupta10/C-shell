#include "headers.h"
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