#include <arpa/inet.h>
//#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
#include <limits.h>

#include "sharedLibrary.h"
#include "eventLoop.h"

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
    char configFile[PATH_MAX] = {"../settings.conf"};
    info externInfo;
    info internInfo;
    pthread_attr_t attr;
    pthread_t threads[2];
    
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
    if (parseConfiguration(configFile, &externInfo, &internInfo) == 0)
    {
        fprintf(stderr, "No rules in configuration file\n");
        return 0;
    }
    
    /* Create the attributes for the threads */
    pthread_attr_init(&attr);
    
    /* Set the thread to joinable (implementation safe) */
    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0)
    {
        systemFatal("Unable to set threads to joinable");
    }
    
    /* Set the thread to system scope */
    if (pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM) != 0)
    {
        systemFatal("Unable to set threads to system scope");
    }
    
    /* Set the flag for external and internal filters */
    externInfo.externFilter = '1';
    internInfo.externFilter = '0';
    
    /* Create the threads */
    pthread_create(&threads[0], &attr, pcapLoop, (void *)&externInfo);
    pthread_create(&threads[1], &attr, pcapLoop, (void *)&internInfo);

    /* Reclaim some memory */
    pthread_attr_destroy(&attr);
    
    /* Reap the threads */
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    
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
    
    unsigned char ip = 0;
    
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
            if (sscanf(line, "%[^,],%[^\n]", externInfo->nic, internInfo->nic) == 2)
            {
                gotCardNames = 1;
            }
            continue;
        }
        /* Get the data and ensure that it's valid */
        if (sscanf(line, "%d, %[^,], %d, %[^\n]", &externPort, externIp, &internPort, internIp) == 4)
        {
            validSettings++;
            /* Convert ip addresses to network form */
            inet_pton(AF_INET, externIp, &externInfo->ip);
            inet_pton(AF_INET, externIp, &internInfo->ip);
            inet_pton(AF_INET, internIp, &ip);
            
            /* Add the data to the map */
            rlAdd(htonl(externPort), htonl(internPort), ip);
        }
    }
    fclose(file);
    printf("%s", rlToStr());
    return validSettings;
}
