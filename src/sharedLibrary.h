#ifndef SHARED_LIBRARY_H
#define SHARED_LIBRARY_H

#include <libnet.h>

/* Defines */

/* Typedefs */

typedef struct
{
    char externFilter;
    char incomingNic[8];
    char outgoingNic[8];
    char ip[16];
    libnet_t *myPacket;
} info;

/* Function Prototypes */
void systemFatal(const char* message);
unsigned short addRuleToMaps(unsigned int clientIp, unsigned short clientPort);
void deleteRuleFromMap(unsigned int clientIp, unsigned short clientPort, unsigned short serverPort);
int cliFind(unsigned int clientIp, unsigned short clientPort, unsigned short *srvPort);
unsigned short srvFind(unsigned short serverPort, unsigned int *clientIp, unsigned short *clientPort);
int rlFind(unsigned short clientDestPort, unsigned short *serverDestPort, unsigned int *serverDestIp);
int rlAdd(unsigned short clientDestPort, unsigned short serverDestPort, unsigned int serverDestIp);
char* rlToStr();

#endif
