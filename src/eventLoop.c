#include "eventLoop.h"

/* Local prototypes */
static int createRawSocketTcp();
static void createFilter(char *filter, char *nic);

void *pcapLoop(void *data)
{
    info *myInfo = (info*)data;
    pcap_t *handle;
    char errorBuffer[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;
    char *filter = NULL;
    bpf_u_int32 mask;
    bpf_u_int32 net;
    
    /* Create the raw socket for sending data through */
    myInfo->rawSocket = createRawSocketTcp();
    
    /* Create the filter */
    createFilter(filter, myInfo->nic);

    /* Get the properties of the device that we are listening on */
    if (pcap_lookupnet(myInfo->nic, &net, &mask, errorBuffer) == -1)
    {
        systemFatal("Unable to get device settings on pcap_lookupnet");
    }
    
    /* Open the session in promiscuous mode */
    handle = pcap_open_live(myInfo->nic, SNAP_LEN, 0, 0, errorBuffer);
    if (handle == NULL)
    {
        systemFatal("Unable to open live capture");
    }
    
    /* Parse the filter to the capture */
    if (pcap_compile(handle, &fp, filter, 0, PCAP_NETMASK_UNKNOWN) == -1)
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
    pthread_exit(NULL);
}

static int createRawSocketTcp()
{
    int rawSocket = 0;
    int one = 1;
    const int *val = &one;
    
    /* Create the raw socket for sending data through */
    rawSocket = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);

    if (rawSocket == -1)
    {
        systemFatal("Unable to create raw socket");
    }
    
    if (setsockopt(rawSocket, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) == -1)
    {
        systemFatal("Unable to set raw socket options");
    }
    
    return rawSocket;
}

/* Need to grab the port filter from hashMap and attach the NIC to it */
static void createFilter(char *filter, char *nic)
{
    char *ports = malloc(sizeof(char) * FILTER_BUFFER);
    ports = rlToStr();
    filter = malloc(sizeof(char) * FILTER_BUFFER);
    snprintf(filter, FILTER_BUFFER, "-i %s %s", nic, ports);
    free(ports);
}
