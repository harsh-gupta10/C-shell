#include "headers.h"
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