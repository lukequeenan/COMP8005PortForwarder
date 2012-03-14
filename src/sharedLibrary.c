#include <stdlib.h>
#include <stdio.h>

#include "sharedLibrary.h"

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