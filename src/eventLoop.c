#include "eventLoop.h"

void *sniff(void *data)
{
    pcap_t *handle;
    char device[] = "em0";
    char errorBuffer[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;
    char filter_exp[] = "port 22";
    bpf_u_int32 mask;
    bpf_u_int32 net;
    
    /* Get the properties of the device that we are listening on */
    if (pcap_lookupnet(device, &net, &mask, errorBuffer) == -1)
    {
        systemFatal("Unable to get device settings on pcap_lookupnet");
    }
    
    /* Open the session in promiscuous mode */
    handle = pcap_open_live(device, SNAP_LEN, 0, 0, errorBuffer);
    if (handle == NULL)
    {
        systemFatal("Unable to open live capture");
    }
    
    /* Parse the filter to the capture */
    if (pcap_compile(handle, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1)
    {
        systemFatal("Unable to compile filter");
    }
    
    /* Set the filter on the listening device */
    if (pcap_setfilter(handle, &fp) == -1)
    {
        systemFatal("Unable to set filter");
    }
    
    /* Call pcap_loop and process packets as they are received */
    if (pcap_loop(handle, -1, forward, NULL) == -1)
    {
        systemFatal("Error in pcap_loop");
    }
    
    /* Clean up and exit */
    pcap_freecode(&fp);
    pcap_close(handle);
    pthread_exit(NULL);
}

