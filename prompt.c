#include "headers.h"
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
