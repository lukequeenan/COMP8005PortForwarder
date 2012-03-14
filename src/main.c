
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

#include "sharedLibrary.h"

#define SETTINGS_BUFFER 1024

/*
 -- FUNCTION: main
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Luke Queenan
 --
 -- PROGRAMMER: Luke Queenan
 --
 -- INTERFACE: int main(argc, char **argv)
 --
 -- RETURNS: 0 on success
 --
 -- NOTES:
 -- This is the entry point for the port forwarder. It will parse the user input
 -- and start the required functionality.
 */
int main(int argc, char **argv)
{
    int option = 0;
    int numberOfRules = 0;
    char configFile[PATH_MAX] = {"../settings"};
    item* rules = NULL;
    
    /* Get command line parameters */
    while ((option = getopt(argc, argv, "f:")) != -1)
    {
        switch (option)
        {
            case 'f':
                snprintf(configFile, sizeof(configFile), "%s", optarg);
                break;
                
            default:
                fprintf(stderr, "Usage: %s -f [config file location]\n", argv[0]);
                return 0;
        }
    }
    
    /* Parse the settings file and get all the rules */
    numberOfRules = parseConfiguration(configFile, rules);
    
    return 0;
}

int parseConfiguration(const char *filePath[], rule* rules)
{
    int validSettings = 0;
    char line[SETTINGS_BUFFER];
    FILE *file = NULL;
    item* current = rules;

    int externPort = 0;
    int internPort = 0;
    char ip[16];
    
    /* Open the configuration file */
    if ((file = fopen("../settings", "r")) == NULL)
    {
        systemFatal("Error opening configuration file");
    }
    
    /* While there are lines in the file... */
    while (fgets(line, sizeof(line), file))
    {
        /* Get the data and ensure that it's valid */
        if (sscanf(line, "%d,%s,%d", &externPort, ip, &internPort) == 3)
        {
            validSettings++;
            
            /* Allocate memory for the rule and set values */
            current = malloc(sizeof(item));
            current->externPort = externPort;
            current->ip = ip;
            current->internPort = internPort
            current->next = rules;
            
        }
    }
    
    return validSettings;
}
