/*-----------------------------------------------------------------------------
 --	SOURCE FILE:    main.c - A port forwarder using libpcap and libnet
 --
 --	PROGRAM:		Port Forwarder
 --
 --	FUNCTIONS:		
 --                 static int parseConfiguration(const char filePath[], info *externInfo, info *internInfo);
 --                 int main(int argc, char **argv);
 --
 --	DATE:			March 13, 2012
 --
 --	REVISIONS:		(Date and Description)
 --
 --	DESIGNERS:      Luke Queenan
 --
 --	PROGRAMMERS:	Luke Queenan
 --
 --	NOTES:
 -- The entry point for a port forwarder using libpcap and libnet. This file
 -- contains the functionality for reading and parsing a configuration file and
 -- creating two threads where the packet forwarding is done. One thread
 -- monitors the external side of the connection, while the other monitors the
 -- internal side. This allows for better scalability on machines with multiple
 -- cores and processors.
 ----------------------------------------------------------------------------*/
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "sharedLibrary.h"
#include "eventLoop.h"

#define IPSTRLENGTH      16
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
 -- INTERFACE: int main(argc, char **argv);
 --
 -- RETURNS: 0 on success
 --
 -- NOTES:
 -- This is the entry point for the port forwarder. It allows the user to
 -- specify a configuration file from the command line if the default file is
 -- not present. After calling a function to parse the file, the function
 -- creates two system scope threads and waits for them to finish. The two
 -- threads run the event loop code.
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

/*
 -- FUNCTION: parseConfiguration
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Luke Queenan
 --
 -- PROGRAMMER: Luke Queenan
 --
 -- INTERFACE: static int parseConfiguration(const char filePath[], info *externInfo, info *internInfo);
 --
 -- RETURNS: number of valid rules read
 --
 -- NOTES:
 -- This function takes a file path and two structs where the information will
 -- be held. The function reads the NIC cards and then the rules. The
 -- information is then stored in the structs. The number of valid rules read is
 -- returned.
 */
static int parseConfiguration(const char filePath[], info *externInfo, info *internInfo)
{
    int validSettings = 0;
    int gotCardNames = 0;
    char line[SETTINGS_BUFFER];
    FILE *file = NULL;

    unsigned int externPort = 0;
    unsigned int internPort = 0;
    char externIp[IPSTRLENGTH];
    char internIp[IPSTRLENGTH];
    char *forwarderIp;
    char *serverIp;
    unsigned int ip = 0;
    
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
            if (sscanf(line, "%[^,],%[^\n]", externInfo->incomingNic, internInfo->incomingNic) == 2)
            {

                memcpy(externInfo->outgoingNic, internInfo->incomingNic, 8);
                memcpy(internInfo->outgoingNic, externInfo->incomingNic, 8);
                gotCardNames = 1;
            }
            continue;
        }
        /* Get the data and ensure that it's valid */
        if (sscanf(line, "%d, %[^,], %d, %[^\n]", &externPort, externIp, &internPort, internIp) == 4)
        {
            forwarderIp = malloc(IPSTRLENGTH);
            serverIp = malloc(IPSTRLENGTH);
            validSettings++;

            /* Convert ip addresses to network form */
            memcpy(externInfo->ip, externIp, IPSTRLENGTH);
            memcpy(internInfo->ip, internIp, IPSTRLENGTH);
            memcpy(forwarderIp, externIp, IPSTRLENGTH);
            memcpy(serverIp, internIp, IPSTRLENGTH);
            inet_pton(AF_INET, internIp, &ip);

            /* Add the data to the map */
            rlAdd(htons(externPort), htons(internPort), ip, serverIp, forwarderIp);
        }
    }
    fclose(file);
    return validSettings;
}
