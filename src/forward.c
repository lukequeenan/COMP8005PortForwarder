#include "forward.h"

void forward(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    const struct iphdr *ip;
    const struct tcphdr *tcp;
    const struct updhdr *udp;
    const char *payload;
    
    
}