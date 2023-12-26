#include "headers.h"
// tokenise command on basis of input and it changes original string
PtrTokStruct TokeniseInput(char *Input, char *WhatToRemove)
{
    PtrTokStruct final;
    final = (PtrTokStruct)malloc(sizeof(tokArrStr));
    char *token;

    // char *ArrOfCommands[MAX_NO_COMMAND];
    final->ArrOfPartsOfOriginalStr = (char **)malloc(sizeof(char *) * MAX_NO_COMMAND);
    token = strtok(Input, WhatToRemove); //? ; & ko nhi dekha
    final->i = 0;
    while (token != NULL)
    {
        final->ArrOfPartsOfOriginalStr[final->i] = token;
        token = strtok(NULL, WhatToRemove);
        final->i++;
    } /* code */
    final->ArrOfPartsOfOriginalStr[final->i] = NULL;
    return final;
}

char **tokenize_withQoutes(const char *str, const char *delimiters, int *token_count)
{
    char current_quote = '\0'; // '\0' means we're not inside quotes
    int start = 0;
    int capacity = 10; // initial capacity for token array
    int count = 0;

    char **tokens = malloc(capacity * sizeof(char *));
    if (!tokens)
    {
        perror("Failed to allocate memory");
        exit(1);
    }

    for (int i = 0; str[i] != '\0'; ++i)
    {
        // Handle quote characters
        if (str[i] == '"' || str[i] == '\'')
        {
            if (current_quote == '\0')
            {
                // Not inside quotes, set the current quote
                current_quote = str[i];
            }
            else if (current_quote == str[i])
            {
                // Encountered the closing quote
                current_quote = '\0';
            }
        }

        if (current_quote == '\0' && strchr(delimiters, str[i]))
        {
            if (i > start)
            {
                tokens[count] = strndup(&str[start], i - start);
                count++;

                if (count >= capacity)
                {
                    capacity *= 2;
                    tokens = realloc(tokens, capacity * sizeof(char *));
                    if (!tokens)
                    {
                        perror("Failed to reallocate memory");
                        exit(1);
                    }
                }
            }
            start = i + 1;
        }
    }

    if (str[start] != '\0')
    {
        tokens[count] = strdup(&str[start]);
        count++;
    }

    *token_count = count;
    for (int i = 0; i < count; i++)
    {
        if (tokens[i][0] == '\'' || tokens[i][0] == '\"')
        {
            tokens[i]++;
            tokens[i][strlen(tokens[i]) - 1] = '\0';
        }
    }
    return tokens;
}