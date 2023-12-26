#include "headers.h"

void stripHTMLTags(char *str)
{
    int insideTag = 0;
    int j = 0;
    for (int i = 0; str[i]; i++)
    {
        if (str[i] == '<')
        {
            insideTag = 1;
            continue;
        }
        if (insideTag)
        {
            if (str[i] == '>')
            {
                insideTag = 0;
            }
            continue;
        }
        str[j++] = str[i];
    }
    str[j] = '\0';
}

void iMan(const char *command_name)
{
    char *token = strtok_r(command_name, " \t\n", &command_name);
    struct addrinfo hints;
    struct addrinfo *res;
    int sockfd;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    char link_updated[4097];
    // snprintf(link_updated , 4097 , "man.he.net");
    strcpy(link_updated, "man.he.net");
    int status = getaddrinfo(link_updated, "http", &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "DNS resolution failed: %s\n", gai_strerror(status));
        // return 1;
    }
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1)
    {
        perror("socket");
        // return 1;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        perror("connect");
        // return 1;
    }

    char request[4096];
    // snprintf(request, sizeof(request), "GET /%s.1 HTTP/1.1\r\nHost: man.he.net\r\n\r\n",token);
    // snprintf(request, sizeof(request), "GET /man1/%s HTTP/1.1\r\nHost: man.he.net\r\nUser-Agent: iMan/1.0\r\n\r\n", token);
    snprintf(request, sizeof(request), "GET http://man.he.net/?topic=%s&section=all HTTP/1.1\r\nHost: man.he.net\r\n\r\n", command_name);

    if (send(sockfd, request, strlen(request), 0) == -1)
    {
        perror("send");
        // return 1;
    }
    printf("%s", request);
    char response[4096];
    int bytes_received;

    while ((bytes_received = recv(sockfd, response, sizeof(response) - 1, 0)) > 0)
    {
        response[bytes_received] = '\0';
        stripHTMLTags(response);
        printf("%s", response);
    }

    if (bytes_received == -1)
    {
        perror("recv");
        // return 1;
    }

    close(sockfd);
    freeaddrinfo(res);

    return command_name;
}
void ExecuteiMan(char *Command)
{
    // Remove iMan
    Command = Command + 1;

    // Remove Space
    int i = 0;
    while (Command[i] == ' ')
    {
        Command++;
    }
    iMan(Command);
}
