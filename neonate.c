#include "headers.h"

// ? NEOnate begin
int get_recent_pid()
{
    FILE *file;
    int pid;

    file = fopen("/proc/sys/kernel/ns_last_pid", "r");
    if (file == NULL)
    {
        perror("Error opening ns_last_pid");
        return -1;
    }
    fscanf(file, "%d", &pid);
    fclose(file);
    return pid;
}

void die(const char *s)
{
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

int stopPrinting = 0;

void handleSignal2(int signo)
{
    if (signo == SIGINT)
    {
        stopPrinting = 1;
    }
}

void ExecuteNeonate(char *Command)
{
    char CommandCopy[ENTRY_SIZE];
    strcpy(CommandCopy, Command);
    PtrTokStruct tokenizeStruct = TokeniseInput(CommandCopy, " \t\n");
    int NoOfPart = tokenizeStruct->i;
    char **ArrOfPartOfCommand = tokenizeStruct->ArrOfPartsOfOriginalStr;

    // struct termios oldt;
    char ch;
    int delay = atoi(ArrOfPartOfCommand[2]);
    // set_terminal_raw_mode(&oldt);
    enableRawMode();
    signal(SIGINT, handleSignal2);

    int PID = fork();
    if (PID < 0)
    {
        perror("IN FORK IN NEONATE");
    }
    else if (PID == 0)
    {
        while (!stopPrinting)
        {
            printf("%d\n", get_recent_pid());
            sleep(delay);
        }
        exit(0);
    }
    else
    {
        while (1)
        {
            read(STDIN_FILENO, &ch, 1);
            if (ch == 'x')
            {
                kill(PID, SIGINT);
                wait(NULL);
                break;
            }
        }
    }

    // do
    // {
    //     printf("%d\n", get_recent_pid());
    //     sleep(delay);
    //     ch = get_single_keypress();
    // } while (ch != 'x');
    // restore_terminal_mode(&oldt);
    disableRawMode();
}
// ? NEOnate END
