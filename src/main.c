
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "sharedLibrary.h"

#define SETTINGS_BUFFER 1024

/* Local Prototypes */
static int parseConfiguration(const char filePath[], info *externInfo, info *internInfo);

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
    char configFile[PATH_MAX] = {"../settings.conf"};
    info externInfo;
    info internInfo;
    
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
    numberOfRules = parseConfiguration(configFile, &externInfo, &internInfo);
    
    return 0;
}

static int parseConfiguration(const char filePath[], info *externInfo, info *internInfo)
{
    int validSettings = 0;
    int gotCardNames = 0;
    char line[SETTINGS_BUFFER];
    FILE *file = NULL;

    int externPort = 0;
    int internPort = 0;
    char externIp[16];
    char internIp[16];
    
    /* Open the configuration file */
    if ((file = fopen(filePath, "r")) == NULL)
    {
        systemFatal("Error opening configuration file");
    }
    
    /* While there are rule lines in the file */
    while (fgets(line, sizeof(line), file))
    {
        /* Deal with commented lines */
        if (line[0] == '#')
        {
            continue;
        }
        /* Card names should be the first line of data, so get them */
        if (gotCardNames == 0)
        {
            if (sscanf(line, "%s,%s", externInfo->nic, internInfo->nic) == 2)
            {
                gotCardNames = 1;
            }
        }
        /* Get the data and ensure that it's valid */
        if (sscanf(line, "%d,%s,%d,%s", &externPort, externIp, &internPort, internIp) == 4)
        {
            validSettings++;
            memcpy(externInfo->ip, externIp, sizeof(char) * 16);
            memcpy(externInfo->ip, internIp, sizeof(char) * 16);
            /* TODO: need to make the filter here! */
        }
    }
    
    return validSettings;
}
