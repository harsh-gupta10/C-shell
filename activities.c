#include "headers.h"
// ? activities Start
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
