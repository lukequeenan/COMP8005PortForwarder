#ifndef CLIENTHASH_H
#define CLIENTHASH_H

#include "uthash.h"

typedef struct {
    unsigned long clientIpPort;
    unsigned int serverPort;
    UT_hash_handle hh;
} CLIENT, *PCLIENT;

unsigned long combineIpPort(unsigned int ip, unsigned int port);
void clientAdd(unsigned int clientIp, unsigned int clientPort, unsigned int serverPort);
PCLIENT clientFind(unsigned int clientIp, unsigned int clientPort);
void clientDelete(unsigned int clientIp, unsigned int clientPort);
void clientDeleteAll();
void clientPrint();
int clientSort(PCLIENT a, PCLIENT b);
void clientSortById();

#endif
