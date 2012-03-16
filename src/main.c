
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

#include "sharedLibrary.h"
#include "mapHash.h"

#define SETTINGS_BUFFER 1024

/* Local Prototypes */
static int parseConfiguration(const char filePath[]);

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
    numberOfRules = parseConfiguration(configFile);
    
    return 0;
}

static int parseConfiguration(const char filePath[])
{
    int validSettings = 0;
    char line[SETTINGS_BUFFER];
    FILE *file = NULL;
    PMAP rule = NULL;

    int externPort = 0;
    int internPort = 0;
    char ip[16];
    
    /* Open the configuration file */
    if ((file = fopen(filePath, "r")) == NULL)
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
            rule->clientPort = internPort;
            rule->clientIp = inet_addr(ip);
            rule->serverPort = externPort;
            mapAdd(rule);
        }
    }
    
    return validSettings;
}
