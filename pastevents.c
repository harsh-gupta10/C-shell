#include "headers.h"
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

