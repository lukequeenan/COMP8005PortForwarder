/*-----------------------------------------------------------------------------
 --	SOURCE FILE:    forward.c - A port forwarder using libpcap and libnet
 --
 --	PROGRAM:		Port Forwarder
 --
 --	FUNCTIONS:		
 --                 void forward(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
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
 -- This file contains the function that gets called when a packet matching one
 -- of our rules is detected. It deals with the actual breakdown and forwarding
 -- of packets between the two machines.
 ----------------------------------------------------------------------------*/
#include "forward.h"

/* Local prototypes */
//static void tcpPacket();
//static void udpPacket();


/*
 -- FUNCTION: forward
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Luke Queenan
 --
 -- PROGRAMMER: Luke Queenan
 --
 -- INTERFACE: void forward(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- This is the function that deals with the actual forwarding of packets. It
 -- is called by the libpcap loop when a packet matching the filter is detected.
 -- This function breaks the packet down and creates it using the libnet library
 -- calls. The packet is then forwarded to the relevent machine.
 */
void forward(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    const struct sniff_ip *ip = NULL;
    const struct sniff_tcp *tcp = NULL;
    const struct sniff_udp *udp = NULL;
    
    info *myInfo = (info*)args;
    
    int ipHeaderSize = 0;
    int tcpHeaderSize = 0;
    int payloadSize = 0;
    
    libnet_ptag_t ptag;
    
    u_short sport = 0;
    u_short dport = 0;
    struct in_addr src_ip;
    struct in_addr dst_ip;
    
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
        
        /* If we are the external filter, check to see if the SYN bit is set */
        if ((myInfo->externFilter == '1') && ((tcp->th_flags & TH_SYN) == TH_SYN))
        {
            /* Add the data to the map */
            addRuleToMaps(ip->ip_src.s_addr, tcp->th_sport, tcp->th_dport);
        }
        
        /* Get the size of the payload */
        payloadSize = ntohs(ip->ip_len) - (ipHeaderSize + tcpHeaderSize);
        
        /* Get the source and destination information from the map */
        if (myInfo->externFilter == '1')
        {
            /* We are sending packets to the internal machine */
            
            /* Get the forwarding machine's return port */
            if (cliFind(ip->ip_src.s_addr, tcp->th_sport, &sport) == 0)
            {
                return;
            }
            
            /* Get the internal machine's listening port and IP for this port */
            rlFind(tcp->th_dport, &dport, &dst_ip.s_addr);
            src_ip.s_addr = ip->ip_dst.s_addr;
        }
        else
        {
            /* We are sending packets out to the world */
            
            /* Find the client to forward the packet to */
            if (srvFind(tcp->th_dport, &dst_ip.s_addr, &dport, &sport) == 0)
            {
                return;
            }
            
            /* Set the IP address */
            src_ip.s_addr = ip->ip_dst.s_addr;
        }
        
        /* Make the new TCP header */
        ptag = libnet_build_tcp(
            htons(sport),                               /* source port */
            htons(dport),                               /* destination port */
            ntohl(tcp->th_seq),                         /* sequence number */
            ntohl(tcp->th_ack),                         /* acknowledgement num */
            tcp->th_flags,                              /* control flags */
            tcp->th_win,                                /* window size */
            0,                                          /* checksum */
            tcp->th_urp,                                /* urgent pointer */
            tcpHeaderSize + payloadSize,                /* TCP packet size */
            (u_char *)tcp + tcpHeaderSize,              /* payload */
            payloadSize,                                /* payload size */
            myInfo->myPacket,                           /* libnet handle */
            0);                                         /* libnet id */
        
        /* Error check */
        if (ptag == -1)
        {
            libnet_clear_packet(myInfo->myPacket);
            return;
        }
        
    }
    else if (ip->ip_p == IPPROTO_UDP)
    {
        /* Grab the UDP packet */
        udp = (struct sniff_udp*)(packet + SIZE_ETHERNET + ipHeaderSize);
        
        
        /* If we are the external filter, make sure that we have a rule added */
        if (myInfo->externFilter == '1')
        {
            addRuleToMaps(ip->ip_src.s_addr, udp->uh_sport, udp->uh_dport);
        }
        
        /* Get the payload size */
        payloadSize = ntohs(udp->uh_len) - SIZE_UDP_HEADER;
        
        /* Get the source and destination information from the map */
        if (myInfo->externFilter == '1')
        {
            /* We are sending packets to the internal machine */
            
            /* Get the forwarding machine's return port */
            if (cliFind(ip->ip_src.s_addr, udp->uh_sport, &sport) == 0)
            {
                return;
            }
            
            /* Get the internal machine's listening port and IP for this port */
            rlFind(udp->uh_dport, &dport, &dst_ip.s_addr);
            src_ip.s_addr = ip->ip_dst.s_addr;
        }
        else
        {
            /* We are sending packets out to the world */
            
            /* Find the client to forward the packet to */
            if (srvFind(udp->uh_dport, &dst_ip.s_addr, &dport, &sport) == 0)
            {
                return;
            }
            
            /* Set the IP address */
            src_ip.s_addr = ip->ip_dst.s_addr;
        }
        
        /* Make the new UDP header */
        ptag = libnet_build_udp(
            htons(sport),                           /* source port */
            htons(dport),                           /* destination port */
            ntohl(udp->uh_len),                     /* packet size */
            0,                                      /* checksum */
            (u_char *)udp + SIZE_UDP_HEADER,         /* payload */
            payloadSize,                            /* payload size */
            myInfo->myPacket,                       /* libnet handle */
            0);                                     /* libnet id */
        
        /* Error check */
        if (ptag == -1)
        {
            libnet_clear_packet(myInfo->myPacket);
            return;
        }
    }
    else
    {
        /* Protocol that we do not handle, exit */
        return;
    }
    
    /* Make the IP header */
    ptag = libnet_build_ipv4(
        ipHeaderSize + tcpHeaderSize + payloadSize, /* length */
        ip->ip_tos,                                 /* TOS */
        ip->ip_id,                                  /* IP ID */
        0,                                          /* IP Frag */
        ip->ip_ttl,                                 /* TTL */
        ip->ip_p,                                   /* protocol */
        0,                                          /* checksum */
        src_ip.s_addr,                              /* source IP */
        dst_ip.s_addr,                              /* destination IP */
        NULL,                                       /* payload */
        0,                                          /* payload size */
        myInfo->myPacket,                           /* libnet handle */
        0);                                         /* libnet id */                                 
    
    /* Error check */
    if (ptag == -1)
    {
        libnet_clear_packet(myInfo->myPacket);
        return;
    }
    
    /* Send the packet out */
    libnet_write(myInfo->myPacket);
    
    /* Clear the libnet system */
    libnet_clear_packet(myInfo->myPacket);
    return;
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
