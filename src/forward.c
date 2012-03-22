#include "forward.h"

/* Local prototypes */
static void getHeadersTcp(struct sniff_ip *ip, struct sniff_tcp *tcp, u_char *myPacket);
//static void tcpPacket();
//static void udpPacket();

void forward(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    char errorBuffer[LIBNET_ERRBUF_SIZE];
    int packetSize = 0;
    
    /* Allocate memory for the packet */
    packetSize = malloc(sizeof(char) * IP_MAXPACKET + ETH_H);
    
    
}

static void getHeadersTcp(struct sniff_ip *ip, struct sniff_tcp *tcp, u_char *myPacket)
{
    int ipHeaderSize = 0;
    
    ip = (struct sniff_ip*)(myPacket);
    ipHeaderSize = IP_HL(ip) * 4;
    
    tcp = (struct sniff_tcp*)(myPacket + ipHeaderSize);
}
/*
static void tcpPacket()
{
    return;
}

static void udpPacket()
{
    return;
}
*/
