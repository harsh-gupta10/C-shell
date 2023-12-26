#include "headers.h"

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
                // exit(1);
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
                foreground_process_pid = -1;
            if (DoesBg == 1)
            {
                // isme wait nhi karna
                InsertProcess(PID, ArrOfPartOfPIPEDCommandSpace[0]);
                printf("%d\n", PID);
            }
            else
            {
                foreground_process_pid = PID;
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
