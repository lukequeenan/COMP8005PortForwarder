/*-----------------------------------------------------------------------------
 --	SOURCE FILE:    eventLoop.c - A port forwarder using libpcap and libnet
 --
 --	PROGRAM:		Port Forwarder
 --
 --	FUNCTIONS:		
 --                 static void createFilter(char *filter, char *ip, char externFilter);
 --                 void *pcapLoop(void *data);
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
 -- This file contains the functionality for creating a pcap loop, which
 -- includes creating a filter and a libnet packet for actual forwarding. There
 -- are two instances of this function running during program execution, each of
 -- them in their own thread.
 ----------------------------------------------------------------------------*/

#include "eventLoop.h"

/* Local prototypes */
static void createFilter(char *filter, char externFilter);

/*
 -- FUNCTION: pcapLoop
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Luke Queenan
 --
 -- PROGRAMMER: Luke Queenan
 --
 -- INTERFACE: void *pcapLoop(void *data);
 --
 -- RETURNS: void*
 --
 -- NOTES:
 -- The function called by each thread. Sets up the filter and the pcap
 -- monitoring function.
 */
void *pcapLoop(void *data)
{
    info *myInfo = (info*)data;
    pcap_t *handle;
    char errorBuffer[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;
    char *filter = malloc(sizeof(char) * FILTER_BUFFER);
    bpf_u_int32 mask;
    bpf_u_int32 net;

    /* Create the libnet context */
    myInfo->myPacket = libnet_init(LIBNET_RAW4, myInfo->outgoingNic, errorBuffer);
    if (myInfo->myPacket == NULL)
    {
        systemFatal("Unable to set up libnet context");
    }

    /* Create the filter */
    createFilter(filter, myInfo->externFilter);
    
    /* Get the properties of the device that we are listening on */
    if (pcap_lookupnet(myInfo->incomingNic, &net, &mask, errorBuffer) == -1)
    {
        systemFatal("Unable to get device settings on pcap_lookupnet");
    }

    /* Open the session in promiscuous mode */
    handle = pcap_open_live(myInfo->incomingNic, SNAP_LEN, 0, 0, errorBuffer);
    if (handle == NULL)
    {
        systemFatal("Unable to open live capture");
    }

    /* Parse the filter to the capture */
    if (pcap_compile(handle, &fp, filter, 0, net) == -1)
    {
        systemFatal("Unable to compile filter");
    }
    
    /* Set the filter on the listening device */
    if (pcap_setfilter(handle, &fp) == -1)
    {
        systemFatal("Unable to set filter");
    }

    /* Call pcap_loop and process packets as they are received */
    if (pcap_loop(handle, -1, forward, (u_char*)myInfo) == -1)
    {
        systemFatal("Error in pcap_loop");
    }

    /* Clean up and exit */
    free(filter);
    pcap_freecode(&fp);
    pcap_close(handle);
    libnet_destroy(myInfo->myPacket);
    pthread_exit(NULL);
}

/*
 -- FUNCTION: createFilter
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Luke Queenan
 --
 -- PROGRAMMER: Luke Queenan
 --
 -- INTERFACE: static void createFilter(char *filter, char externFilter);
 --
 -- RETURNS: void*
 --
 -- NOTES:
 -- This function creates the filter string used by the libpcap monitoring
 -- function.
 */
static void createFilter(char *filter, char externFilter)
{
    char *ports = NULL;
    char *internRules = NULL;
    if (externFilter == '1')
    {
        ports = malloc(sizeof(char) * FILTER_BUFFER);
        ports = rlExternToStr();
        snprintf(filter, FILTER_BUFFER, "%s", ports);
        free(ports);
    }
    else
    {
        internRules = rlInternToStr();
        snprintf(filter, FILTER_BUFFER, "%s", internRules);
        free(internRules);
    }
    
}
