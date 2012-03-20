#ifndef CLIENTHASH_H
#define CLIENTHASH_H

#include "uthash.h"

typedef struct {
    unsigned long clientIpPort;
    unsigned short serverPort;
    UT_hash_handle hh;
} CLIENT, *PCLIENT;

unsigned long combineIpPort(unsigned int ip, unsigned short port);
void clientAdd(unsigned int clientIp, unsigned short clientPort, unsigned short serverPort);
PCLIENT clientFind(unsigned int clientIp, unsigned short clientPort);
void clientDelete(unsigned int clientIp, unsigned short clientPort);
void clientDeleteAll();
void clientPrint();
int clientSort(PCLIENT a, PCLIENT b);
void clientSortById();

#endif
