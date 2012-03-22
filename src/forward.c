#include "forward.h"

/* Local prototypes */
static void getHeadersTcp(struct sniff_ip *ip, struct sniff_tcp *tcp, u_char *myPacket);
unsigned short csum(unsigned short *buf, int nwords);
//static void tcpPacket();
//static void udpPacket();

void forward(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    /*const struct sniff_ethernet *ethernet = NULL;*/
    const struct sniff_ip *ip = NULL;
    const struct sniff_tcp *tcp = NULL;
    
    struct sniff_ip *myIp = NULL;
    struct sniff_tcp *myTcp = NULL;
    
    struct sockaddr_in sin;
    
    u_char *myPacket = NULL;
    
    info *myInfo = (info*)args;
    
    int ipHeaderSize = 0;
    int tcpHeaderSize = 0;
    int payloadSize = 0;
    int sentData = 0;
    
    unsigned int ipAddress = 0;
    
    /* Ethernet header */
    /*ethernet = (struct sniff_ethernet*)packet;*/
    
    /* Get the IP header and offset value */
    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    ipHeaderSize = IP_HL(ip) * 4;
    if (ipHeaderSize < 20)
    {
		return;
	}
    
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
        
        /* Apparently the filter means nothing! */
        inet_pton(AF_INET, myInfo->ip, &ipAddress);
        if ((myInfo->externFilter == '1') && (ipAddress == ip->ip_src.s_addr))
        {
            return;
        }
        
        /* If we are the external filter, check to see if the SYN bit is set */
        if ((myInfo->externFilter == '1') && ((tcp->th_flags & TH_SYN) == TH_SYN))
        {
            /* Add the data to the map */
            addRuleToMaps(ip->ip_src.s_addr, tcp->th_sport);
        }
        
        /* Get the size of the payload */
        payloadSize = ntohs(ip->ip_len) - (ipHeaderSize + tcpHeaderSize);
        
        /* Copy the packet over so we can edit it */
        myPacket = malloc(ipHeaderSize + tcpHeaderSize + payloadSize);
        memcpy(myPacket, packet + SIZE_ETHERNET, ipHeaderSize + tcpHeaderSize + payloadSize);
        
        /* Break the packet down */
        myIp = (struct sniff_ip*)myPacket;
        myTcp = (struct sniff_tcp*)(myPacket + ipHeaderSize);
        
        
        
        
        //getHeadersTcp(myIp, myTcp, myPacket);
        
        /* Set information that changes if we are filtering internal or external
           data */
        if (myInfo->externFilter == '1')
        {
            /* We are sending packets to the internal machine */
            
            /* Get the forwarding machine's return port */
            if (cliFind(ip->ip_src.s_addr, tcp->th_sport, &myTcp->th_sport) == 0)
            {
                free(myPacket);
                return;
            }
            /* Get the internal machine's listening port and IP for this port */
            rlFind(tcp->th_dport, &myTcp->th_dport, &myIp->ip_dst.s_addr);
            myIp->ip_src.s_addr = ip->ip_dst.s_addr;
        }
        else
        {
            /* Find the client to forward the packet to */
            if (srvFind(tcp->th_dport, &myIp->ip_dst.s_addr, &myTcp->th_dport) == 0)
            {
                free(myPacket);
                return;
            }
            
            /* We are sending packets out to the world */
            myIp->ip_src.s_addr = ip->ip_dst.s_addr;
        }
        
        /* Set information that remains the same for both filters */
        sin.sin_family = AF_INET;
        sin.sin_port = myTcp->th_dport;
        sin.sin_addr.s_addr = myIp->ip_dst.s_addr;
        
        /* Set the IP checksum to 0 */
        myIp->ip_sum = 0;
        
        /* Create and assign the the checksum */
        myTcp->th_sum = csum((unsigned short *) myPacket, ipHeaderSize +
                             tcpHeaderSize + payloadSize);
        
        /* Send the packet on its way to the internal machine */
        sentData = sendto(myInfo->rawSocket, myPacket, ipHeaderSize +
                          tcpHeaderSize + payloadSize, 0, 
                          (struct sockaddr *)&sin, sizeof(sin));

        if (sentData == -1)
        {
            systemFatal("Unable to send packet");
        }
        
        /* Clean up */
        free(myPacket);
        
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
    
    ip = (struct sniff_ip*)(myPacket);
    ipHeaderSize = IP_HL(ip) * 4;
    
    tcp = (struct sniff_tcp*)(myPacket + ipHeaderSize);
}

unsigned short csum(unsigned short *buf, int nwords)
{
    unsigned long sum;
    for(sum=0; nwords>0; nwords--)
    {
        sum += *buf++;
    }
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
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
