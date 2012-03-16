#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "sharedLibrary.h"
#include "clientHash.h"


/*
 -- FUNCTION: systemFatal
 --
 -- DATE: March 12, 2011
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Aman Abdulla
 --
 -- PROGRAMMER: Luke Queenan
 --
 -- INTERFACE: static void systemFatal(const char* message);
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- This function displays an error message and shuts down the program.
 */
void systemFatal(const char* message)
{
    perror(message);
    exit(EXIT_FAILURE);
}


unsigned int RandomSourcePort() {
    unsigned int iseed = (unsigned int)time(NULL);
    srand (iseed);
    unsigned int port;
    do {
        port = rand() % 45000 + 20000;
    } while(serverFind(port) != 0);
    return port;
}
