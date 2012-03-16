#ifndef FORWARD_H
#define FORWARD_H

#define __USE_BSD
#define __FAVOR_BSD

#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <pcap.h>

#include "sharedLibrary.h"

void forward(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

#endif