#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

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
    float prevSum = 0, prevIdle = 0;
    float sum = 0, idle = 0;

    for (int i = 0; i < 2; i++)
    {
        float tmpSum = 0, tmpIdle = 0;
        if (fgets(buffer, 10000, fp) == NULL)
        {
            fprintf(stderr, "ERROR - cannot read file \"/proc/stat\"\n");
            return false;
        }
        char *token = strtok(buffer, " ");
        int j = 0;

        while (token != NULL)
        {
            token = strtok(NULL, " ");
            if (token != NULL && j != 3 && j != 4)
            {
                tmpSum += atof(token);
            }
            if (j == 3 || j == 4)
            {
                tmpIdle = atof(token);
            }

            j++;
            if (j == 8)
            {
                break;
            }
        }

        if (i == 0)
        {
            prevSum = tmpSum;
            prevIdle = tmpIdle;
        }
        if (i == 1)
        {
            sum = tmpSum;
            idle = tmpIdle;
        }
        sleep(1);
    }
    fclose(fp);

    float totalD = 0, idleD = 0;
    totalD = (idle + sum) - (prevIdle + prevSum);
    idleD = idle - prevIdle;
    *cpuUsage = 100 - (((totalD - idleD) / totalD) * 100);

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
    char hostName[100], cpuName[1000];

    if (!checkargs(argc, argv, &port) ||
        !calcCpuUsage(&cpuUsage) ||
        !fetchHostName(hostName) ||
        !fetchCpuName(cpuName))
    {
        return 1;
    }

    printf("%d\n", port);
    printf("%f\n", cpuUsage);
    printf("%s", hostName);
    printf("%s", cpuName + 1); // we need to skip first character due to it being " "

    return 0;
}