#ifndef SERVERHASH_H
#define SERVERHASH_H

#include "uthash.h"

typedef struct {
    int serverID;
    unsigned int clientIp;
    unsigned int clientPort;
    UT_hash_handle hh;
} SERVER, *PSERVER;

void serverAdd(unsigned int serverPort, unsigned int clientIp, unsigned int clientPort);
PSERVER serverFind(int serverPort);
void serverDelete(int serverPort);
void serverDeleteAll();
void serverPrint();
int serverSort(PSERVER a, PSERVER b);
void serverSortById();

#endif
