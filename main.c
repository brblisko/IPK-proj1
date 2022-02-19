#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

bool fetchCpuName(char *cpuName)
{
    FILE *fp = popen("cat /proc/cpuinfo | grep \"model name\" | head -n 1 | awk -F  \":\" \'/1/ {print $2}\'", "r");
    if (fp == NULL)
    {
        fprintf(stderr, "ERROR - cannot open file \"/proc/cpuinfo\"\n");
        return false;
    }
    if (fgets(cpuName, 1000, fp) == NULL)
    {
        fprintf(stderr, "ERROR - cannot read file \"/proc/cpuinfo\"\n");
        return false;
    }
    pclose(fp);
    if (cpuName == NULL)
    {
        fprintf(stderr, "ERROR - unexpected error");
        return false;
    }
    return true;
}

bool fetchHostName(char *hostName)
{
    FILE *fp = fopen("/proc/sys/kernel/hostname", "r");
    if (fp == NULL)
    {
        fprintf(stderr, "ERROR - cannot open file \"/proc/sys/kernel/hostname\"\n");
        return false;
    }
    if (fgets(hostName, 100, fp) == NULL)
    {
        fprintf(stderr, "ERROR - cannot read file \"/proc/sys/kernel/hostname\"\n");
        return false;
    }
    fclose(fp);

    return true;
}

bool calcCpuUsage(int *cpuUsage)
{
    int data[2][10];
    for (int i = 0; i < 2; i++)
    {
        FILE *fp = fopen("/proc/stat", "r");
        if (fp == NULL)
        {
            fprintf(stderr, "ERROR - cannot open file \"/proc/stat\"\n");
            return false;
        }
        char buffer[10000];
        int j = 0;

        if (fgets(buffer, 10000, fp) == NULL)
        {
            fprintf(stderr, "ERROR - cannot read file \"/proc/stat\"\n");
            return false;
        }

        char *token = strtok(buffer, " ");

        while (token != NULL)
        {
            token = strtok(NULL, " ");
            if (token != NULL)
            {
                data[i][j] = atoi(token);
            }
            j++;
        }

        fclose(fp);
        sleep(1);
    }

    int idle[2] = {0, 0};
    int total[2] = {0, 0};

    for (int i = 0; i < 2; i++)
    {
        idle[i] = data[i][3] + data[i][4];
        for (int j = 0; j < 10; j++)
        {
            total[i] += data[i][j];
        }
    }
    int totaldif = total[1] - total[0];
    int idledif = idle[1] - idle[0];

    *cpuUsage = 100 * (totaldif - idledif) / totaldif;

    return true;
}

bool checkargs(int argc, char **argv, int *port)
{
    if (argc != 2)
    {
        fprintf(stderr, "ERROR - to use this program enter one argumet that resembles port number\n");
        return false;
    }

    char *endPtr;
    *port = strtol(argv[1], &endPtr, 10);
    if (*endPtr != '\0')
    {
        fprintf(stderr, "ERROR - could not convert \"%s\" to integer\nPlease enter integer\n", argv[1]);
        return false;
    }
    return true;
}

int main(int argc, char **argv)
{
    int port = 0;
    int cpuUsage = 0;
    char hostName[100], cpuName[1000];

    if (!checkargs(argc, argv, &port))
    {
        return 1;
    }

    int mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int reuse = 1;
    setsockopt(mySocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const char *)&reuse, sizeof(int));
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    bind(mySocket, (struct sockaddr *)&server, sizeof(server));
    listen(mySocket, 256);

    while (true)
    {

        int clientFd = accept(mySocket, NULL, NULL);
        char buffer[10000];
        if (read(clientFd, buffer, 10000) == -1)
        {
            fprintf(stderr, "ERROR");
            return 1;
        }

        if (!strncmp(buffer, "GET /hostname", 13))
        {
            if (!fetchHostName(hostName))
            {
                return 1;
            }

            char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n";

            write(clientFd, response, strlen(response));
            write(clientFd, hostName, strlen(hostName));
        }
        else if (!strncmp(buffer, "GET /cpu-name", 13))
        {
            if (!fetchCpuName(cpuName))
            {
                return 1;
            }
            char response[10000];
            strcpy(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n");
            strcat(response, cpuName + 1);

            write(clientFd, response, strlen(response));
        }
        else if (!strncmp(buffer, "GET /load", 9))
        {
            if (!calcCpuUsage(&cpuUsage))
            {
                return 1;
            }
            char tmp[80];
            sprintf(tmp, "%d", cpuUsage);
            char response[10000];
            strcpy(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n");
            strcat(response, tmp);
            strcat(response, " %\n");

            write(clientFd, response, strlen(response));
        }
        else
        {
            char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n400 Bad Request";
            write(clientFd, response, sizeof(response) - 1);
        }
        close(clientFd);
    }

    return 0;
}