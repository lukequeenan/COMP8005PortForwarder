#include "forward.h"

void forward(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    const struct sniff_ethernet *ethernet = NULL;
    const struct sniff_ip *ip = NULL;
    const struct sniff_tcp *tcp = NULL;
    
    struct sniff_ip *myIp = NULL;
    struct sniff_tcp *myTcp = NULL;
    
    u_char *myPacket = NULL;
    
    int ipHeaderSize = 0;
    int tcpHeaderSize = 0;
    int payloadSize = 0;
    
    unsigned long ipAddress = 0;
    u_short port = 0;
    
    
    /* Ethernet header */
    ethernet = (struct sniff_ethernet*)packet;
    
    /* Get the IP header and offset value */
    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    ipHeaderSize = IP_HL(ip) * 4;
    if (ipHeaderSize < 20)
    {
		return;
	}
    
    /* Get the source IP */
    ipAddress = ip->ip_src.s_addr;
    
    /* Determine the protocol and handle it */
    if (ip->ip_p == IPPROTO_TCP)
    {
        /* Get tcp header along with its size */
        tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + ipHeaderSize);
        tcpHeaderSize = TH_OFF(tcp) * 4;
        if (tcpHeaderSize < 20)
        {
            return;
        }
        
        /* Grab the source port */
        port = tcp->th_sport;
        
        /* Get the size of the payload */
        payloadSize = ntohs(ip->ip_len) - (ipHeaderSize + tcpHeaderSize);
        
        /* Copy the packet over so we can edit it */
        myPacket = malloc(sizeof(u_char) * ipHeaderSize + tcpHeaderSize + payloadSize);
        memcpy(myPacket, packet + SIZE_ETHERNET, ipHeaderSize + tcpHeaderSize + payloadSize);
        getHeadersTcp(myIp, myTcp, myPacket);
        
        /* Set the data for forwarding */
        
        
        /* Send the packet on its way to the internal machine */
        
        return;        
    }
    else if (ip->ip_p == IPPROTO_UDP)
    {
        return;
    }
    else
    {
        return;
    }
}

static void getHeadersTcp(struct sniff_ip *ip, struct sniff_tcp *tcp, u_char *myPacket)
{
    int ipHeaderSize = 0;
    int tcpHeaderSize = 0;
    
    ip = (struct sniff_ip*)(myPacket);
    ipHeaderSize = IP_HL(ip) * 4;
    
    tcp = (struct sniff_tcp*)(myPacket + ipHeaderSize);
    tcpHeaderSize = TH_OFF(tcp) * 4;
}

static void tcpPacket(unsigned long ip, const struct sniff_tcp *tcp)
{
    const char *payload = NULL;
    int tcpHeaderSize = TH_OFF(tcp)*4;
}

static void udpPacket(unsigned long ip)
{
    ip++;
    return;
}