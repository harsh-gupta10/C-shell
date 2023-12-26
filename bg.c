#include "headers.h"
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
