#include "eventLoop.h"

/* Local prototypes */
static void createFilter(char *filter, char *nic, char *ip, char externFilter);

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
    createFilter(filter, myInfo->incomingNic, myInfo->ip, myInfo->externFilter);
    
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

/* Need to grab the port filter from hashMap and attach the NIC to it */
static void createFilter(char *filter, char *nic, char *ip, char externFilter)
{
    char *ports = NULL;
    if (externFilter == '1')
    {
        ports = malloc(sizeof(char) * FILTER_BUFFER);
        ports = rlToStr();
        snprintf(filter, FILTER_BUFFER, "-i %s dst host %s and %s", nic, ip, ports);
        free(ports);
    }
    else
    {
        snprintf(filter, FILTER_BUFFER, "-i %s src host %s", nic, ip);
    }
    
}
