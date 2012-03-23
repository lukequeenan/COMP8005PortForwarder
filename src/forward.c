#include "forward.h"

/* Local prototypes */
//static void tcpPacket();
//static void udpPacket();

void forward(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    const struct sniff_ip *ip = NULL;
    const struct sniff_tcp *tcp = NULL;
    
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
            addRuleToMaps(ip->ip_src.s_addr, tcp->th_sport);
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
            /* Find the client to forward the packet to */
            if (srvFind(tcp->th_dport, &dst_ip.s_addr, &dport) == 0)
            {
                return;
            }
            /* Need to implement another rule find in the hash map to find this*/
            sport = htons(22); /* Testing!!!! NEED TO FIX ^*/
            
            /* We are sending packets out to the world */
            src_ip.s_addr = ip->ip_dst.s_addr;
        }
        
        /* Make the new TCP packet */
        ptag = libnet_build_tcp(
            htons(sport),                                      /* source port */
            htons(dport),                                      /* destination port */
            ntohl(tcp->th_seq),                                /* sequence number */
            ntohl(tcp->th_ack),                                /* acknowledgement num */
            tcp->th_flags,                                     /* control flags */
            tcp->th_win,                                      /* window size */
            0,                                          /* checksum */
            tcp->th_urp,                                /* urgent pointer */
            tcpHeaderSize + payloadSize,                /* TCP packet size */
            (u_char *)tcp + tcpHeaderSize,              /* payload */
            payloadSize,                                /* payload size */
            myInfo->myPacket,                           /* libnet handle */
            0);                                         /* libnet id */
    
        if (ptag == -1)
        {
            libnet_clear_packet(myInfo->myPacket);
            return;
        }
        
        ptag = libnet_build_ipv4(
            ipHeaderSize + tcpHeaderSize + payloadSize, /* length */
            ip->ip_tos,                                          /* TOS */
            ip->ip_id,                                        /* IP ID */
            0,                                          /* IP Frag */
            ip->ip_ttl,                                         /* TTL */
            ip->ip_p,                                /* protocol */
            0,                                          /* checksum */
            src_ip.s_addr,                                     /* source IP */
            dst_ip.s_addr,                                     /* destination IP */
            NULL,                                       /* payload */
            0,                                          /* payload size */
            myInfo->myPacket,                                          /* libnet handle */
            0);                                         /* libnet id */                                 
        
        if (ptag == -1)
        {
            libnet_clear_packet(myInfo->myPacket);
            return;
        }
        
        libnet_write(myInfo->myPacket);
        
        /* Clear the libnet system */
        libnet_clear_packet(myInfo->myPacket);
        return;
    }
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
