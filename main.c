#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

    if (!checkargs(argc, argv, &port))
    {
        return 1;
    }

    printf("%d", port);

    return 0;
}