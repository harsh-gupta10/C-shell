#include "headers.h"
void handle_sigint()
{
    // char PathToSave[1000];
    // AddCommandWithCondition("exit\n");
    // snprintf(PathToSave, sizeof(PathToSave), "%s/history.txt", ShellActivatedDir);
    // SaveEntry(PathToSave);
    // printf("\n");
    // kill()
    // bgProcess *Curr = ActiVitesListHead;
    // // bgProcess *Store = curr;
    // int status;
    // while (Curr != NULL)
    // {
    //     kill(Curr->PID, SIGTERM);
    //     Curr = Curr->Next;
    // }
    // printf("\n");
    // if (foreground_process_pid>0)
    // {
    //   foreground_process_pid==-1
    // }
    // else if(foreground_process_pid==-1)
    // {
    //      Display_name_A();
    // }
    // fflush(stdout);
    if (foreground_process_pid != -1)
    {
        kill(foreground_process_pid, SIGINT);
    }

    printf("\n");
    // Display_name_A(ShellActivatedDir);
    // fflush(stdout);
    // exit(0);
}

void handle_ctrlD()
{
    char PathToSave[1000];
    AddCommandWithCondition("exit\n");
    snprintf(PathToSave, sizeof(PathToSave), "%s/history.txt", ShellActivatedDir);
    SaveEntry(PathToSave);
    // printf("\n");
    // kill()
    bgProcess *Curr = ActiVitesListHead;
    // bgProcess *Store = curr;
    int status;
    while (Curr != NULL)
    {
        kill(Curr->PID, SIGTERM);
        Curr = Curr->Next;
    }
    exit(0);
    // break;
}

// void handle_sigTSTP(int sig) {
//     if (foreground_process_pid != -1) { // Check if a foreground process is running
//         kill(foreground_process_pid, SIGSTOP); // Send the SIGSTOP signal to stop it
//         printf("\nProcess with PID %d has been stopped.\n", foreground_process_pid);
//         // Update the process state in your data structures to "Stopped"
//     }
// }
// pid_t foreground_process_pid = -1;  // global variable to hold the PID of the foreground process

void handle_sigtstp(int sig)
{
    // if (foreground_process_pid != -1)
    // {
    //     // Stop the foreground process if it's running
    //     kill(foreground_process_pid, SIGSTOP);
    //     printf("\nProcess with PID %d has been stopped.\n", foreground_process_pid);
    //     foreground_process_pid = -1; // Reset the foreground PID
    // }
    // else
    // {
    //     printf("\nNo foreground process running.\n");
    // }
    // // Display your shell prompt again, for example:
    // // printf("> ");
    // Display_name_A(ShellActivatedDir);
    // fflush(stdout);
    kill(foreground_process_pid, SIGSTOP);
    kill(foreground_process_pid, SIGTSTP);
    printf("\n");
}