#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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

    return true;
}

bool calcCpuUsage(float *cpuUsage)
{
    char buffer[10000];
    FILE *fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        fprintf(stderr, "ERROR - cannot open file \"/proc/stat\"\n");
        return false;
    }

    if (fgets(buffer, 10000, fp) == NULL)
    {
        fprintf(stderr, "ERROR - cannot read file \"/proc/stat\"\n");
        return false;
    }
    fclose(fp);
    char *token = strtok(buffer, " ");
    float sum = 0, idle = 0;
    int i = 0;
    while (token != NULL)
    {
        token = strtok(NULL, " ");
        if (token != NULL)
        {
            sum += atoi(token);
        }
        if (i == 3)
        {
            idle = atoi(token);
        }

        i++;
    }

    *cpuUsage = 100 - ((idle * 100) / sum);

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
    float cpuUsage = 0;
    char hostName[100];

    if (!checkargs(argc, argv, &port) ||
        !calcCpuUsage(&cpuUsage) ||
        !fetchHostName(hostName))
    {
        return 1;
    }

    printf("%d\n", port);
    printf("%f\n", cpuUsage);
    printf("%s\n", hostName);

    return 0;
}